#ifndef __PLANE_CLOTH_H__
#define __PLANE_CLOTH_H__

using namespace std;

#include "Vec3.h"
#include <vector>
#include "GL\glut.h"

class PlaneCloth
{
public:
	double					dt;
	double					horiStrutRestLength;
	double					vertStrutRestLength;
	int						m_Res[2];

	double					m_Restitution;		// 일단 0.5로 설정하기

	vector<double>			m_InvMass;
	vector<Vec3<double>>	m_Pos;
	vector<Vec3<double>>	m_NewPos;
	vector<Vec3<double>>	m_Vel;
public:
	PlaneCloth();
	PlaneCloth(int width, int height);
	~PlaneCloth();
public:
	inline Vec3<double>		GetPos(int i, int j) { return m_Pos[j * m_Res[0] + i]; }

	inline void AddVel(int i, int j, Vec3<double> v) { m_Vel[j * m_Res[0] + i] += v; }
public:
	void Init(void);
	void Draw(void);
	void DrawOutline(void);

	void Simulation();
	void Integrate();
	Vec3<double> calcTriNormal(Vec3<double> p1, Vec3<double> p2, Vec3<double> p3);

	void setStructuralSpring();
	void setShearingSpring();
	void setBending();
};

#endif