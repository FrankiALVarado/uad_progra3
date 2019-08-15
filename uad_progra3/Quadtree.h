#pragma once
#include <vector>
#include "AABB.h"
template<class t> 
class QuadtreeNode
{
private:
	QuadtreeNode<t> * m_n1;
	QuadtreeNode<t> * m_n2;
	QuadtreeNode<t> * m_n3;
	QuadtreeNode<t> * m_n4;
	std::vector<t*> m_data;
	AABB m_boundingBox;
public:
	QuadtreeNode();
	~QuadtreeNode();
};


template<class t>
class Quadtree
{
private:
	QuadtreeNode<t> *m_root;
public:
	Quadtree();
	~Quadtree();
};

template<class t>
inline QuadtreeNode<t>::QuadtreeNode()
{
}

template<class t>
inline QuadtreeNode<t>::~QuadtreeNode()
{
}
