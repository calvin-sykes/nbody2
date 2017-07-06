#include "BHTreeNode.h"
#include "Constants.h"
#include "Error.h"

#include <cassert>
#include <functional>

namespace nbody
{
	std::vector<ParticleData> BHTreeNode::s_renegades;
	std::vector<BHTreeNode const*> BHTreeNode::s_crit_cells;
	DebugStats BHTreeNode::s_stat = { 0, 0, 0, 0, 0 };
	double constexpr BHTreeNode::s_THETA;
	size_t constexpr BHTreeNode::s_CRIT_SIZE;

	BHTreeNode::BHTreeNode(Quad const& q, size_t const level, BHTreeNode const* parent) :
		m_more(nullptr),
		m_next(nullptr),
		m_level(level),
		m_body(),
		m_c_state(),
		m_c_aux_state(),
		m_rcrit_sq((q.getLength() / s_THETA) * (q.getLength() / s_THETA)),
		m_quad(q),
		m_parent(parent),
		m_num(0),
		m_subdivided(false)
	{
		m_daughters[0] = m_daughters[1] = m_daughters[2] = m_daughters[3] = nullptr;
		s_stat.m_node_ct++;
		if (level > s_stat.m_max_level)
			s_stat.m_max_level++;
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
		m_rcrit_sq = (q.getLength() / s_THETA) * (q.getLength() / s_THETA);
		m_num = 0;
		m_c_state = {};
		m_c_aux_state = {};

		treeStatReset();

		s_renegades.clear();
		s_crit_cells.clear();
	}

	bool BHTreeNode::isRoot() const
	{
		return m_parent == nullptr;
	}

	bool BHTreeNode::isExternal() const
	{
		return m_num == 1;
	}

	bool BHTreeNode::wasSubdivided() const
	{
		return m_subdivided;
	}

	double BHTreeNode::getMass() const
	{
		return m_c_aux_state.mass;
	}

	size_t BHTreeNode::getLevel() const
	{
		return m_level;
	}

	size_t BHTreeNode::getNumBodies() const
	{
		return m_num;
	}

	Quad const & BHTreeNode::getQuad() const
	{
		return m_quad;
	}

	const Vector2d & BHTreeNode::getCentreMass() const
	{
		return m_c_state.pos;
	}

	size_t BHTreeNode::getNumRenegades()
	{
		return s_renegades.size();
	}

	double BHTreeNode::getTheta()
	{
		return s_THETA;
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
			[&reset_subdivide_flags](BHTreeNode const* node)
		{
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
		s_stat.m_num_crit_size = 0;
	}

	BHTreeNode const* BHTreeNode::getHovered(Vector2d const & pos) const
	{
		if (isRoot() && !m_quad.contains(pos))
			return nullptr;

		assert(m_quad.contains(pos));

		auto which = m_daughters[static_cast<size_t>(m_quad.whichDaughter(pos))];

		return which ? which->getHovered(pos) : this;
	}

	BHTreeNode const * BHTreeNode::getParent() const
	{
		return m_parent;
	}

	BHTreeNode * BHTreeNode::createDaughter(Daughter const which) const
	{
		auto daughter_quad = m_quad.createDaughter(which);
		return new BHTreeNode(daughter_quad, m_level + 1, this);
	}

	void BHTreeNode::insert(ParticleData const& new_body)
	{
		auto const& p1 = *new_body.m_state;

		// which daughter would contain this body?
		auto which_daughter = m_quad.whichDaughter(new_body.m_state->pos);
		if (which_daughter == NONE)
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
			if (isExternal())
			{
				assert(isExternal() || isRoot());

				auto const& p2 = *m_body.m_state;

				// same coordinates - unphysical
				// place in renegades vector
				if (p1.pos == p2.pos)
					s_renegades.push_back(new_body);
				else // p1.pos != p2.pos
				{
					// recursively add current body to correct daughter
					auto current_daughter = m_quad.whichDaughter(p2.pos);
					m_daughters[current_daughter] = createDaughter(current_daughter);
					m_daughters[current_daughter]->insert(m_body);
					// node is no longer external
					m_body.reset();
					s_stat.m_body_ct--;
				}
			}
			// create daughter for new body if it does not exist
			if (!m_daughters[which_daughter])
				m_daughters[which_daughter] = createDaughter(which_daughter);
			// add new body
			m_daughters[which_daughter]->insert(new_body);
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
		// preallocate space to prevent repeated reallocations
		if (isRoot())
			s_crit_cells.reserve(static_cast<size_t>(m_num / s_CRIT_SIZE * 1.1));

		m_c_state = {};		// initialise centre of mass
		m_c_aux_state = {}; // and total mass

		for (auto d : m_daughters)
		{
			if (d)
			{
				d->computeMassDistribution();
				// contribution to centre of mass and total mass from daughters
				m_c_state.pos += d->m_c_state.pos * d->m_c_aux_state.mass;
				m_c_aux_state.mass += d->m_c_aux_state.mass;
			}
		}

		// test whether node is first in hierarchy to be smaller than critical size
		if (m_num < s_CRIT_SIZE && m_parent->m_num > s_CRIT_SIZE)
		{
			s_crit_cells.push_back(this);
			s_stat.m_num_crit_size++;
		}

		if (isExternal())
		{
			assert(m_body.isNotNull());

			// for external node, mass and centre of mass are mass and position of body
			m_c_state = *m_body.m_state;
			m_c_aux_state = *m_body.m_aux_state;
		}
		else // !isExternal() 
			m_c_state.pos /= m_c_aux_state.mass;
	}

	void BHTreeNode::threadTree(BHTreeNode * next)
	{
		// this node's next always points to next sibling passed in
		m_next = next;

		if (!isExternal())
		{
			auto n_daughters = 0;
			// NB extra size of array
			// so that loop below automatically threads final daughter to this node's next sibling
			BHTreeNode * actual_daughters[NUM_DAUGHTERS + 1];
			for (auto const & d : m_daughters)
			{
				if (d)
					actual_daughters[n_daughters++] = d;
			}
			// more points to first daughter
			m_more = actual_daughters[0];
			actual_daughters[n_daughters] = next;
			for (auto i = 0; i < n_daughters; i++)
			{
				// each daughter receives its sibling as next, except for final one
				// which receives this node's sibling instead
				actual_daughters[i]->threadTree(actual_daughters[i + 1]);
			}
		}
	}

	void BHTreeNode::calcForces() const
	{
		assert(isRoot());

		auto len = s_crit_cells.size();
#pragma omp parallel for schedule(static)
		for (auto i = 0; i < len; i++)
		{
			auto cell = s_crit_cells[i];
			auto ilist = cell->makeInteractionList(this);

			// discover bodies in group
			std::vector<std::reference_wrapper<ParticleData const>> bodies;
			bodies.reserve(cell->m_num);

			for (auto q = cell; q != cell->m_next; )
			{
				if (q->isExternal())
				{
					bodies.emplace_back(q->m_body);
					q = q->m_next;
				}
				else
					q = q->m_more;
			}

			std::for_each(bodies.begin(), bodies.end(), [&](auto &a)
			{
				auto b = a.get();
				b.m_deriv_state->acc = {};
				for (auto const& elem : ilist)
					b.m_deriv_state->acc += calcAccel(b, elem);
				for (auto const& b2 : bodies)
					b.m_deriv_state->acc += calcAccel(b, b2);
				if (s_renegades.size())
					for (auto const& r : s_renegades)
						b.m_deriv_state->acc += calcAccel(b, r);
			});
		}
	}

	std::forward_list<ParticleData> BHTreeNode::makeInteractionList(BHTreeNode const * root) const
	{
		assert(isRoot());

		std::forward_list<ParticleData> ilist;

		for (auto q = root; q != root->m_next; )
		{
			// ignore self-interactions
			if (q == this)
			{
				q = q->m_next;
				continue;
			}

			// if this node is leaf, use direct calculation
			if (q->isExternal())
			{
				ilist.push_front(q->m_body);

				q = q->m_next;
				s_stat.m_num_calc++;
			}
			else // !q->isExternal()
			{
				// if node is far enough, use BH approx
				if (accept(q))
				{
					// construct 'combined particle'
					ilist.emplace_front(&q->m_c_state, &q->m_c_aux_state);

					q->m_subdivided = false;
					q = q->m_next;
					s_stat.m_num_calc++;
				}
				else // try daughters
				{
					q->m_subdivided = true;
					q = q->m_more;
				}
			}
		}
		return ilist;
	}

	// accel caused by p2 on p1
	Vector2d BHTreeNode::calcAccel(ParticleData const & p1, ParticleData const & p2)
	{
		auto const& s1 = *p1.m_state;
		auto const& s2 = *p2.m_state;

		// ignore self-interactions
		if (s1.pos == s2.pos)
			return {};

		auto const& r1 = s1.pos;
		auto const& r2 = s2.pos;
		auto m2 = p2.m_aux_state->mass;

		auto rel_pos = r2 - r1; // relative position vector r
		auto rel_pos_mag_sq = rel_pos.mag_sq(); // |r|**2
		auto unit_vec = (1 / sqrt(rel_pos_mag_sq)) * rel_pos; // rhat = r/|r|
		rel_pos_mag_sq = std::max(rel_pos_mag_sq, Constants::SOFTENING * Constants::SOFTENING);
		// F = (G m1 m2 / (|r|**2) * r_hat
		return (Constants::G * m2 / rel_pos_mag_sq) * unit_vec;	// a = F / m1
	}

	bool BHTreeNode::accept(BHTreeNode const * n) const
	{
		/*auto group_centre = group->m_quad.getPos();
		auto group_half_len = group->m_quad.getLength() / 2.0;
		auto delta_pos = n->getCentreMass() - group_centre;

		auto contact_x = std::min(abs(delta_pos.x), group_half_len);
		auto contact_y = std::min(abs(delta_pos.y), group_half_len);
		contact_x *= delta_pos.x > 0 ? 1 : -1;
		contact_y *= delta_pos.y > 0 ? 1 : -1;

		auto contact = Vector2d{ contact_x, contact_y } +group_centre;

		auto rel_pos = n->getCentreMass() - contact;
		auto rel_pos_mag_sq = rel_pos.mag_sq();

		return rel_pos_mag_sq > n->m_rcrit_sq;*/

		auto delta = n->getCentreMass() - n->getQuad().getPos();
		auto delta_sq = delta.mag_sq();
		auto rel_pos = getCentreMass() - n->getCentreMass();
		auto rel_pos_mag_sq = rel_pos.mag_sq();

		return rel_pos_mag_sq > n->m_rcrit_sq + delta_sq;
	}
}