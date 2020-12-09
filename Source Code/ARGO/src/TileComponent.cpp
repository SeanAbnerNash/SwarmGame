#include "stdafx.h"
#include "TileComponent.h"

TileComponent::TileComponent(int t_index) :
	Component(ComponentType::Tile),
	m_index(t_index),
	m_neighbours{nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr}
{
}

Neighbours* TileComponent::getNeighbours()
{
	return &m_neighbours;
}

int TileComponent::getIndex() const
{
	return m_index;
}
