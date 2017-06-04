#include "QuadManager.h"

namespace nbody
{
	QuadManager::QuadManager()
	{

	}

	QuadManager::~QuadManager()
	{

	}
	void QuadManager::update(BHTree * ptr)
	{

	}

	void QuadManager::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(vtx_array);
	}
}
