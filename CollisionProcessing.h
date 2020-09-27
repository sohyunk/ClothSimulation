#pragma once

#include "PlaneCloth.h"
#include "Ball.h"
#include "Vec3.h"

class CollisionProcessing
{
public:
	PlaneCloth				*particle1;
	Ball					*particle2;
	double					m_Restitution;
	double					m_Penetration;
	Vec3<double>			m_ContactNormal;

public:
	CollisionProcessing();
	CollisionProcessing(PlaneCloth *p1, Ball *p2);
	~CollisionProcessing();

public:
	void		Resolve();
	double		CalculateSeparatingVeclocity(int i);
	void		ResolveVeclocity(int i);
	void		ResolveInterpenetration(int i);
};

