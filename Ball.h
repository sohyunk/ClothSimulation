#pragma once

#include "Vec3.h"
#include <vector>
#include "GL\glut.h"

using namespace std;

class Ball
{
public:
	bool			is_move;
	double			m_BallInvMass;
	double			m_BallRadius;
	double			m_BallTime;
	Vec3<double>	m_BallPos;
	double	m_BallVel;

public:
	Ball();
	~Ball();

public:
	void InitBall();
	void DrawBall();
	void SetPosition();
};

