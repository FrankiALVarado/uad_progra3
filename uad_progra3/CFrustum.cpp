#include "stdafx.h"
#include "CFrustum.h"

/*
*/
CFrustum::CFrustum()
{
}

/*
*/
CFrustum::~CFrustum()
{

}

/*
There are different ways to compute the frustum information.

  + A geometric approach takes the camera position and lookAt vectors, as well as the near and far planes and the fov and aspect ratio.
	The frustum points in world space are computed as a function of these variables.
	https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
	-----------------------------------------------------------------------------------------------------------------------------------

	The apex of the pyramid is the position of the camera (px,py,pz). A view ray can be computed with direction d = l - p, where l = (lx,ly,lz) and origin p = (px,py,pz).
	The near and far planes are perpendicular to the view ray and are placed at distances nearDist and farDist.
	The rectangular boundaries of the view frustum in those planes, near and far, have dimensions that are a function of the distance and the fov (vertical field of view),
	and ratio (ratio between the horizontal and vertical fields of view).

	The height and width of the rectangular boundary on the near plane are defined as follows:

	Hnear = 2 * tan(fov / 2) * nearDist
	Wnear = Hnear * ratio

	The same reasoning can be applied to the far plane:

	Hfar = 2 * tan(fov / 2) * farDist
	Wfar = Hfar * ratio

	The planes are defined such that the normal points towards the inside of the view frustum. Testing if an object is inside the view frustum is performed
	by computing on which side of the plane the object resides. This can be done computing the signed distance from the point ot the plane.
	If it is on the side that the normal is pointing, i.e. the signed distance is positive, then it is on the right side of the respective plane.
	If an object is on the right side of all six planes then the object is inside the frustum.

  + Another approach is to compute the frustum using the projection matrix and the inverse of the movelview matrix.
	Hartmann/Gribbs method (http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf)
	---------------------------------------------------------------------------------------------------------------

*/

/*
  Geometric approach of extracting the frustum information
*/
void CFrustum::update(CVector3 eyePos, CVector3 lookAtPos, CVector3 upVector, CVector3 right, float nearDistance, float farDistance, float fov, float aspectRatio)
{
	/*
	eyePos   - the camera position
	forward  - a vector with the direction of the camera's view ray. In here it is assumed that this vector has been normalized
	nearDist - the distance from the camera to the near plane
	Hnear    - the "height" of the near plane
	Wnear    - the "width" of the near plane
	farDist  - the distance from the camera to the far plane
	Hfar     - the "height" of the far plane
	Wfar     - the "width" of the far plane
	*/

	// Calculate forward vector from eye to target
	CVector3 forwardVector = lookAtPos - eyePos;
	// Normalize vector
	forwardVector.normalize();

	float tanFov2 = tanf((fov * PI_OVER_180) / 2.0f);

	float Hnear = 2 * tanFov2 * nearDistance;
	float Wnear = Hnear * aspectRatio;

	float Hfar = 2 * tanFov2 * farDistance;
	float Wfar = Hfar * aspectRatio;

	CVector3 farCenter = eyePos + (forwardVector * farDistance);
	float    HFarOver2 = Hfar / 2.0f;
	float    WFarOver2 = Wfar / 2.0f;
	CVector3 upHFarOver2 = upVector * HFarOver2;
	CVector3 rightWFarOver2 = right * WFarOver2;

	// Compute the 4 corners of the frustum on the far plane
	CVector3 farTopLeft = farCenter + upHFarOver2 - rightWFarOver2;
	CVector3 farTopRight = farCenter + upHFarOver2 + rightWFarOver2;
	CVector3 farBottomLeft = farCenter - upHFarOver2 - rightWFarOver2;
	CVector3 farBottomRight = farCenter - upHFarOver2 + rightWFarOver2;

	CVector3 nearCenter = eyePos + (forwardVector * nearDistance);
	float    HNearOver2 = Hnear / 2.0f;
	float    WNearOver2 = Wnear / 2.0f;
	CVector3 upHNearOver2 = upVector * HNearOver2;
	CVector3 rightWNearOver2 = right * WNearOver2;

	// Compute the 4 corners of the frustum on the near plane
	CVector3 nearTopLeft = nearCenter + upHNearOver2 - rightWNearOver2;
	CVector3 nearTopRight = nearCenter + upHNearOver2 + rightWNearOver2;
	CVector3 nearBottomLeft = nearCenter - upHNearOver2 - rightWNearOver2;
	CVector3 nearBottomRight = nearCenter - upHNearOver2 + rightWNearOver2;

	// Compute the six planes. Need to pass the points in counter clock wise order to setPoints
	m_planes[TOP_PLANE].setPoints(nearTopRight, nearTopLeft, farTopLeft);
	m_planes[BOTTOM_PLANE].setPoints(nearBottomLeft, nearBottomRight, farBottomRight);
	m_planes[LEFT_PLANE].setPoints(nearTopLeft, nearBottomLeft, farBottomLeft);
	m_planes[RIGHT_PLANE].setPoints(nearBottomRight, nearTopRight, farBottomRight);
	m_planes[NEAR_PLANE].setPoints(nearTopLeft, nearTopRight, nearBottomRight);
	m_planes[FAR_PLANE].setPoints(farTopRight, farTopLeft, farBottomLeft);
}

/*
   Hartmann/Gribbs method
   TO-DO
   (Use the other update() for now, should give the same results...)
*/
void CFrustum::update(MathHelper::Matrix4 *projMatrix, MathHelper::Matrix4 *viewMatrix)
{

}

/*
*/
CFrustum::FRUSTUM_VISIBILITY_TEST_RESULT CFrustum::isAABBVisible(AABB &aabb)
{
	CFrustum::FRUSTUM_VISIBILITY_TEST_RESULT result = FRUSTUM_VISIBILITY_TEST_RESULT::INSIDE;
	int visibleCorners = 0;

	// If any of the 8 corners of the AABB is INSIDE ALL PLANES of the frustum, then the AABB is at least partially visible
	for (int i = 0; i < 8; ++i)
	{
		if (isPointVisible(aabb.m_points[i]) == FRUSTUM_VISIBILITY_TEST_RESULT::INSIDE)
		{
			++visibleCorners;
		}
	}

	// No corners visible, AABB is outside the frustum
	if (visibleCorners == 0)
	{
		result = FRUSTUM_VISIBILITY_TEST_RESULT::OUTSIDE;
	}
	// At least one corner visible but not all 8, AABB is intersecting the frustum
	else if (visibleCorners < 8)
	{
		result = FRUSTUM_VISIBILITY_TEST_RESULT::INTERSECT;
	}

	return result;
}

/*
*/
CFrustum::FRUSTUM_VISIBILITY_TEST_RESULT CFrustum::isPointVisible(CVector3 &point)
{
	CFrustum::FRUSTUM_VISIBILITY_TEST_RESULT result = FRUSTUM_VISIBILITY_TEST_RESULT::INSIDE;

	// If the point is INSIDE ALL planes of the frustum, it is visible
	for (int i = 0; i < 6; ++i)
	{
		if (m_planes[i].distanceToPoint(point) < 0.0f)
		{
			return FRUSTUM_VISIBILITY_TEST_RESULT::OUTSIDE;
		}
	}

	return result;
}
