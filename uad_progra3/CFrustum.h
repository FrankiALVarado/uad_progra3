#pragma once

#ifndef CFRUSTUM_H
#define CFRUSTUM_H

#include "Include/MathHelper.h"
#include "AABB.h"
#include "Include/CVector3.h"
#include "CPlane.h"

/*
*/
class CFrustum
{
private:

	enum FRUSTUM_PLANE_ID
	{
		TOP_PLANE = 0,
		BOTTOM_PLANE,
		LEFT_PLANE,
		RIGHT_PLANE,
		NEAR_PLANE,
		FAR_PLANE
	};

	CPlane m_planes[6];

public:

	enum class FRUSTUM_VISIBILITY_TEST_RESULT
	{
		OUTSIDE = 0,
		INTERSECT,
		INSIDE
	};

	CFrustum();
	~CFrustum();

	FRUSTUM_VISIBILITY_TEST_RESULT isAABBVisible(AABB &aabb);
	FRUSTUM_VISIBILITY_TEST_RESULT isPointVisible(CVector3 &point);

	void update(CVector3 eyePos, CVector3 lookAtPos, CVector3 upVector, CVector3 right, float nearD, float farD, float fov, float aspectRatio);
	void update(MathHelper::Matrix4 *projMatrix, MathHelper::Matrix4 *viewMatrix);
};

#endif
