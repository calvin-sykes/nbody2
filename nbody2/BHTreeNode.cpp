#include "BHTreeNode.h"
#include "Constants.h"
#include "Error.h"

#include <cassert>
#include <functional>

namespace nbody
{
	std::vector<ParticleData> BHTreeNode::s_renegades;
	DebugStats BHTreeNode::s_stat = { 0, 0, 0, 0 };
	double BHTreeNode::s_bh_theta = 0.9;
	//double BHTreeNode::s_bh_theta = 0;

	BHTreeNode::BHTreeNode(Quad const& q, size_t const level, BHTreeNode const* parent)
		: m_level(level),
		m_body(),
		m_mass(0),
		m_rcrit_sq((q.getLength() / s_bh_theta) * (q.getLength() / s_bh_theta)),
		m_centre_mass(),
		m_quad(q),
		m_parent(parent),
		m_num(0),
		m_subdivided(false)
	{
		m_daughters[0] = m_daughters[1] = m_daughters[2] = m_daughters[3] = nullptr;
		s_stat.m_node_ct++;
		if (level > s_stat.m_max_level)
		{
			s_stat.m_max_level++;
		}
	}

	BHTreeNode::~BHTreeNode()
	{
		for (size_t i = 0; i < NUM_DAUGHTERS; i++)
		{
			delete m_daughters[i];
			m_daughters[i] = nullptr;
		}
	}

	void BHTreeNode::reset(Quad const& q)
	{
		if (!isRoot())
			throw MAKE_ERROR("Non-root node attempted to reset tree");

		for (size_t i = 0; i < NUM_DAUGHTERS; i++)
		{
			delete m_daughters[i];
			m_daughters[i] = nullptr;
		}

		m_quad = q;
		m_rcrit_sq = (q.getLength() / s_bh_theta) * (q.getLength() / s_bh_theta);
		m_num = 0;
		m_mass = 0;
		m_centre_mass = {};

		treeStatReset();

		s_renegades.clear();
	}

	bool BHTreeNode::isRoot() const
	{
		return m_parent == nullptr;
	}

	bool BHTreeNode::isExternal() const
	{
		return m_daughters[0] == nullptr
			&& m_daughters[1] == nullptr
			&& m_daughters[2] == nullptr
			&& m_daughters[3] == nullptr;
	}

	bool BHTreeNode::wasSubdivided() const
	{
		return m_subdivided;
	}

	size_t BHTreeNode::getNumBodies() const
	{
		return m_num;
	}

	size_t BHTreeNode::getNumRenegades()
	{
		return s_renegades.size();
	}

	Quad const & BHTreeNode::getQuad() const
	{
		return m_quad;
	}

	double BHTreeNode::getTheta()
	{
		return s_bh_theta;
	}

	void BHTreeNode::setTheta(double theta)
	{
		s_bh_theta = theta;
	}

	size_t BHTreeNode::getLevel() const
	{
		return m_level;
	}

	DebugStats const& BHTreeNode::getStats()
	{
		return s_stat;
	}

	void BHTreeNode::forceCalcStatReset() const
	{
		if (!isRoot())
			throw MAKE_ERROR("Non-root node attempted to reset statistics");

		s_stat.m_num_calc = 0;

		std::function<void(BHTreeNode const*)> reset_subdivide_flags =
			[&reset_subdivide_flags](BHTreeNode const* node) -> void {
			node->m_subdivided = false;

			for (auto const& d : node->m_daughters)
			{
				if (d)
					reset_subdivide_flags(d);
			}
		};

		reset_subdivide_flags(this);
	}

	void BHTreeNode::treeStatReset() const
	{
		if (!isRoot())
			throw MAKE_ERROR("Non-root node attempted to reset statistics");

		s_stat.m_node_ct = 0;
		s_stat.m_body_ct = 0;
		s_stat.m_max_level = 0;
	}

	const Vector2d & BHTreeNode::getCentreMass() const
	{
		return m_centre_mass;
	}

	BHTreeNode * BHTreeNode::createDaughter(Quad const& q) const
	{
		return new BHTreeNode(q, m_level + 1, this);
	}

	void BHTreeNode::insert(ParticleData const& new_body, size_t level)
	{
		auto const& p1 = *new_body.m_state;

		// which daughter, would contain this body?
		auto which_daughter = m_quad.whichDaughter(new_body.m_state->pos);
		if (which_daughter == Daughter::NONE)
		{
			// Outside root node -> put in renegades vector
			if (isRoot())
				s_renegades.push_back(new_body);
			else // shouldn't happen
			{
				std::stringstream ss;
				ss << "Particle at " << new_body.m_state->pos
					<< "is outside node centre (" << m_quad.getPos()
					<< "), length" << m_quad.getLength();
				throw MAKE_ERROR(ss.str());
			}
			return;
		}
		// If this node contains bodies
		if (m_num > 0)
		{
			// if tree only contains one body it is currently external
			// now daughter nodes need to be created
			if (m_num == 1)
			{
				assert(isExternal() || isRoot());

				auto const& p2 = *(m_body.m_state);

				// same coordinates - unphysical
				// place in renegades vector
				if (p1.pos == p2.pos)
				{
					s_renegades.push_back(new_body);
				}
				else
				{
					// recursively add current body to correct daughter
					auto current_daughter = m_quad.whichDaughter(p2.pos);
					m_daughters[static_cast<size_t>(current_daughter)] = createDaughter(m_quad.createDaughter(current_daughter));
					m_daughters[static_cast<size_t>(current_daughter)]->insert(m_body, level + 1);
					// node is no longer external
					m_body.reset();
					s_stat.m_body_ct--;
				}
			}
			// create daughter for new body if it does not exist
			if (!m_daughters[static_cast<size_t>(which_daughter)])
				m_daughters[static_cast<size_t>(which_daughter)] = createDaughter(m_quad.createDaughter(which_daughter));
			// add new body
			m_daughters[static_cast<size_t>(which_daughter)]->insert(new_body, level + 1);
		}
		else // m_num == 0
		{
			// store body in this node
			m_body = new_body;
			s_stat.m_body_ct++;

		}

		m_num++;
	}

	void BHTreeNode::computeMassDistribution()
	{
		if (m_num == 1)
		{
			assert(!m_body.isNull());
			auto ps = m_body.m_state;
			auto pa = m_body.m_aux_state;

			m_mass = pa->mass;
			m_centre_mass = ps->pos;
		}
		else
		{
			m_mass = 0;
			m_centre_mass = {};

			for (auto d : m_daughters)
			{
				if (d)
				{
					d->computeMassDistribution();
					m_mass += d->m_mass;
					m_centre_mass += d->m_centre_mass * d->m_mass;
				}
			}

			m_centre_mass /= m_mass;
		}
	}

	Vector2d BHTreeNode::calcForce(ParticleData const & p)
	{
		auto acc = calcTreeForce(p);

		if (s_renegades.size())
		{
			for (auto const& r : s_renegades)
			{
				acc += calcAccel(p, r);
			}
		}

		return acc;
	}

	// accel caused by p2 on p1
	Vector2d BHTreeNode::calcAccel(ParticleData const & p1, ParticleData const & p2) const
	{
		if (p1 == p2)
			return {};

		auto const& r1 = p1.m_state->pos;
		auto const& r2 = p2.m_state->pos;
		auto m2 = p2.m_aux_state->mass;

		auto rel_pos = r2 - r1; // relative position vector r
		auto rel_pos_mag_sq = rel_pos.mag_sq(); // |r|**2
		auto unit_vec = (1 / sqrt(rel_pos_mag_sq)) * rel_pos; // rhat = r/|r|
		rel_pos_mag_sq = std::max(rel_pos_mag_sq, Constants::SOFTENING * Constants::SOFTENING);
															  // F = (G m1 m2 / (|r|**2) * r_hat
		return (Constants::G * m2 / rel_pos_mag_sq) * unit_vec;// a = F / m1
	}

	Vector2d BHTreeNode::calcTreeForce(ParticleData const& p)
	{
		Vector2d acc;

		// if this node is leaf, use direct calculation
		if (m_num == 1)
		{
			acc = calcAccel(p, m_body);
			s_stat.m_num_calc++;
		}
		else // m_num > 1
		{
			auto rel_pos = m_centre_mass - p.m_state->pos;
			auto rel_pos_mag_sq = rel_pos.mag_sq();
			// if node is far enough, use BH approx
			if (rel_pos_mag_sq > m_rcrit_sq)
			{
				m_subdivided = false;

				// construct 'combined particle'
				auto combined_state = ParticleState{ m_centre_mass, {0, 0} };
				auto combined_aux_state = ParticleAuxState{ m_mass };
				acc = calcAccel(p, { &combined_state, &combined_aux_state });

				s_stat.m_num_calc++;
			}	
			else // try daughters
			{
				m_subdivided = true;

				for (auto d : m_daughters)
				{
					if (d)
						acc += d->calcTreeForce(p);
				}
			}
		}

		return acc;
	}
}