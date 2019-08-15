#pragma once
#include "Include/CVector3.h"
class CCamara
{
private:
	CVector3 m_pos;
	CVector3 m_rightvec;
	CVector3 m_upvec;
	CVector3 m_lookAt;
public:
	CCamara();
	~CCamara();
};

