#include "CollisionProcessing.h"

CollisionProcessing::CollisionProcessing()
{
}

CollisionProcessing::CollisionProcessing(PlaneCloth *p1, Ball *p2)
{
	particle1 = p1;
	particle2 = p2;
	m_Restitution = 0.5;
}

CollisionProcessing::~CollisionProcessing()
{
}

void CollisionProcessing::Resolve()
{
	for (int i = 0; i < particle1->m_Pos.size(); i++)
	{
		m_ContactNormal = particle1->m_Pos[i] - particle2->m_BallPos;
		m_Penetration = m_ContactNormal.Length();
		m_ContactNormal.Normalize();

		//ResolveVeclocity(i);
		//ResolveInterpenetration(i);

		if (m_Penetration < particle2->m_BallRadius)
		{
			ResolveVeclocity(i);
			ResolveInterpenetration(i);
		}
	}
}

double	CollisionProcessing::CalculateSeparatingVeclocity(int i)
{
	Vec3<double> relativeVel = particle1->m_Vel[i];
	if (particle2)
	{
		if (particle2->is_move)
		{
			relativeVel -= particle2->m_BallVel;
		}
	}
	return relativeVel.Dot(m_ContactNormal);
}

// Invmass 1.0으로 설정해둬서 일단 제외하고 계산
void CollisionProcessing::ResolveVeclocity(int i)
{
	double separtingVel = CalculateSeparatingVeclocity(i);
	if (separtingVel > 0) { return; }
	double newSepartingVel = -separtingVel * m_Restitution;
	double deltaVel = newSepartingVel - separtingVel;

	double impulse = deltaVel / 2.0;
	Vec3<double> impulsePerInvMass = m_ContactNormal * impulse;
	particle1->m_Vel[i] = particle1->m_Vel[i] + impulsePerInvMass;
}

void CollisionProcessing::ResolveInterpenetration(int i)
{
	Vec3<double> movePerInvMass = m_ContactNormal * ((particle2->m_BallRadius - m_Penetration));
	particle1->m_Pos[i] = particle1->m_Pos[i] + movePerInvMass;
}