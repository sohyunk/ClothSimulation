#include "Ball.h"

Ball::Ball()
{
	InitBall();
}

Ball::~Ball()
{

}

void Ball::InitBall()
{
	is_move = false;
	m_BallInvMass = 1.0;
	m_BallRadius = 2.0;
	m_BallTime = 0;
	m_BallPos = Vec3<double>(3, -4.7, 3.0);
	m_BallVel = 0;
}

void Ball::DrawBall()
{
	glTranslatef(m_BallPos.m_dX, m_BallPos.m_dY, m_BallPos.m_dZ);
	glColor3f(0.2f, 0.8f, 0.5f);
	glPolygonMode(GL_FRONT, GL_FILL);
	glutSolidSphere(m_BallRadius - 0.02, 36, 36);
}

void Ball::SetPosition()
{
	m_BallTime++;
	double nextXPos = cos(m_BallTime / 70.0) * 4;
	m_BallVel = (m_BallPos.GetX() - nextXPos) / m_BallTime;
	m_BallPos.SetX(nextXPos);
}