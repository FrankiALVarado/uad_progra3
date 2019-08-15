#pragma once
#include "Include/CVector3.h"
class AABB
{
public:
	AABB();
	~AABB();
	CVector3 m_points[4];
};

