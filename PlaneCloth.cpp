#include "PlaneCloth.h"

float planeSquaresize = 6.0;

PlaneCloth::PlaneCloth()
{
}

PlaneCloth::PlaneCloth(int width, int height)
{
	m_Res[0] = width;
	m_Res[1] = height;
	int size = m_Res[0] * m_Res[1];

	m_Pos.resize(size);
	m_NewPos.resize(size);
	m_Vel.resize(size);
	m_InvMass.resize(size);

	dt = 0.02;
	horiStrutRestLength = planeSquaresize / (double)m_Res[0];
	vertStrutRestLength = planeSquaresize / (double)m_Res[1];

	Init();
}

PlaneCloth::~PlaneCloth()
{
}

void PlaneCloth::Init()
{
	for (int i = 0; i < m_Res[0]; i++)
	{
		for (int j = 0; j < m_Res[1]; j++)
		{
			int index = j * m_Res[0] + i;
			m_InvMass[index] = 1.0;
			m_Pos[index].Set(planeSquaresize * i / (double)m_Res[0], 0.0, planeSquaresize * j / (double)m_Res[1]);
			m_Restitution = 0.5;
		}
	}
}

void PlaneCloth::Draw()
{
	glColor3f(0.2, 0.6, 0.8);
	glDisable(GL_LIGHTING);
	//¾Õ
	glPolygonMode(GL_FRONT, GL_LINE);
	for (int i = 0; i < m_Res[0] - 1; i++)
	{
		for (int j = 0; j < m_Res[1] - 1; j++)
		{
			auto p00 = GetPos(i, j);
			auto p10 = GetPos(i + 1, j);
			auto p11 = GetPos(i + 1, j + 1);
			auto p01 = GetPos(i, j + 1);

			glBegin(GL_TRIANGLES);
			glVertex3f(p10.x(), p10.y(), p10.z());
			glVertex3f(p00.x(), p00.y(), p00.z());
			glVertex3f(p01.x(), p01.y(), p01.z());

			glVertex3f(p10.x(), p10.y(), p10.z());
			glVertex3f(p01.x(), p01.y(), p01.z());
			glVertex3f(p11.x(), p11.y(), p11.z());
			glEnd();
		}
	}

	// µÚ
	glColor3f(1, 1, 0.3);
	glPolygonMode(GL_BACK, GL_FILL);
	for (int i = 0; i < m_Res[0] - 1; i++)
	{
		for (int j = 0; j < m_Res[1] - 1; j++)
		{
			auto p00 = GetPos(i, j);
			auto p10 = GetPos(i + 1, j);
			auto p11 = GetPos(i + 1, j + 1);
			auto p01 = GetPos(i, j + 1);

			glBegin(GL_TRIANGLES);
			glVertex3f(p00.x(), p00.y(), p00.z());
			glVertex3f(p10.x(), p10.y(), p10.z());
			glVertex3f(p01.x(), p01.y(), p01.z());

			glVertex3f(p10.x(), p10.y(), p10.z());
			glVertex3f(p11.x(), p11.y(), p11.z());
			glVertex3f(p01.x(), p01.y(), p01.z());
			glEnd();
		}
	}

	DrawOutline();
}

void PlaneCloth::DrawOutline()
{
	glColor3f(0, 0, 0);
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	for (int i = 0; i < m_Res[0] - 1; i++)
	{
		for (int j = 0; j < m_Res[1] - 1; j++)
		{
			auto p00 = GetPos(i, j);
			auto p10 = GetPos(i + 1, j);
			auto p11 = GetPos(i + 1, j + 1);
			auto p01 = GetPos(i, j + 1);

			glVertex3f(p00.x(), p00.y(), p00.z());
			glVertex3f(p10.x(), p10.y(), p10.z());

			glVertex3f(p10.x(), p10.y(), p10.z());
			glVertex3f(p01.x(), p01.y(), p01.z());

			glVertex3f(p01.x(), p01.y(), p01.z());
			glVertex3f(p00.x(), p00.y(), p00.z());

			glVertex3f(p10.x(), p10.y(), p10.z());
			glVertex3f(p11.x(), p11.y(), p11.z());

			glVertex3f(p11.x(), p11.y(), p11.z());
			glVertex3f(p01.x(), p01.y(), p01.z());
		}
	}
	glEnd();
	glLineWidth(1.0f);
}

Vec3<double> PlaneCloth::calcTriNormal(Vec3<double> p1, Vec3<double> p2, Vec3<double> p3)
{
	Vec3<double> v1 = p2 - p1;
	Vec3<double> v2 = p3 - p1;

	return v1.Cross(v2);
}

void PlaneCloth::Simulation()
{
	Vec3<double> gravity(0.0, -9.8, 0.0);
	double damping = 0.99;

	// Apply external force
	for (int i = 0; i < m_Res[0]; i++)
	{
		for (int j = 0; j < m_Res[1]; j++)
		{
			int index = j * m_Res[0] + i;
			m_Vel[index] += gravity * dt * m_InvMass[index] + dt;
			m_Vel[index] *= damping;
			m_NewPos[index] = m_Pos[index] + (m_Vel[index] * dt);
		}
	}

	// Constraint projection
	int iter = 5;
	for (int k = 0; k < iter; k++)
	{
		setStructuralSpring();
		setShearingSpring();
		setBending();
	}	
	Integrate();
}

void PlaneCloth::Integrate()
{
	for (int i = 1; i < m_Res[1]; i++)
	{
		for (int j = 0; j < m_Res[0]; j++)
		{
			int index = j * m_Res[0] + i;
			m_Vel[index] = (m_NewPos[index] - m_Pos[index]) / dt;
			m_Pos[index] = m_NewPos[index];
		}
	}
}

void PlaneCloth::setStructuralSpring()
{
	// Vertical
	for (int i = 0; i < m_Res[1]; i++)
	{
		for (int j = 0; j < m_Res[0] - 1; j++)
		{
			int index0 = j * m_Res[0] + i;
			int index1 = (j + 1) * m_Res[0] + i;
			double c_p1p2 = (m_NewPos[index0] - m_NewPos[index1]).Length() - horiStrutRestLength;
			Vec3<double> dp1 = (m_NewPos[index0] - m_NewPos[index1]);
			Vec3<double> dp2 = (m_NewPos[index0] - m_NewPos[index1]);
			dp1.Normalize();
			dp2.Normalize();
			dp1 *= -m_InvMass[index0] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			dp2 *= m_InvMass[index1] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			m_NewPos[index0] += dp1;
			m_NewPos[index1] += dp2;
		}
	}
	// and horizontal structural springs
	for (int i = 0; i < m_Res[1] - 1; i++)
	{
		for (int j = 0; j < m_Res[0]; j++)
		{
			int index0 = j * m_Res[0] + i;
			int index1 = j * m_Res[0] + (i + 1);
			float c_p1p2 = (m_NewPos[index0] - m_NewPos[index1]).Length() - vertStrutRestLength;
			Vec3<double> dp1 = m_NewPos[index0] - m_NewPos[index1];
			Vec3<double> dp2 = m_NewPos[index0] - m_NewPos[index1];
			dp1.Normalize();
			dp2.Normalize();
			dp1 *= -m_InvMass[index0] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			dp2 *= m_InvMass[index1] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			m_NewPos[index0] += dp1;
			m_NewPos[index1] += dp2;
		}
	}
}

void PlaneCloth::setShearingSpring()
{
	for (int i = 0; i < m_Res[1] - 1; i++)
	{
		for (int j = 0; j < m_Res[0] - 1; j++)
		{
			int index0 = j * m_Res[0] + i;
			int index1 = (j + 1) * m_Res[0] + (i + 1);
			double c_p1p2 = (m_NewPos[index0] - m_NewPos[index1]).Length() - sqrt(vertStrutRestLength * vertStrutRestLength + horiStrutRestLength * horiStrutRestLength);
			Vec3<double> dp1 = (m_NewPos[index0] - m_NewPos[index1]);
			Vec3<double> dp2 = (m_NewPos[index0] - m_NewPos[index1]);
			dp1.Normalize();
			dp2.Normalize();
			dp1 *= -m_InvMass[index0] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			dp2 *= m_InvMass[index1] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			m_NewPos[index0] += dp1;
			m_NewPos[index1] += dp2;
		}
	}
	for (int i = 0; i < m_Res[1] - 1; i++)
	{
		for (int j = 0; j < m_Res[0] - 1; j++)
		{
			int index0 = (j + 1) * m_Res[0] + i;
			int index1 = j * m_Res[0] + (i + 1);
			double c_p1p2 = (m_NewPos[index0] - m_NewPos[index1]).Length() - sqrt(vertStrutRestLength * vertStrutRestLength + horiStrutRestLength * horiStrutRestLength);
			Vec3<double> dp1 = (m_NewPos[index0] - m_NewPos[index1]);
			Vec3<double> dp2 = (m_NewPos[index0] - m_NewPos[index1]);
			dp1.Normalize();
			dp2.Normalize();
			dp1 *= -m_InvMass[index0] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			dp2 *= m_InvMass[index1] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			m_NewPos[index0] += dp1;
			m_NewPos[index1] += dp2;
		}
	}
}

void PlaneCloth::setBending()
{
	for (int i = 0; i < m_Res[1] - 2; i++)
	{
		for (int j = 0; j < m_Res[0]; j++)
		{
			int index0 = j * m_Res[0] + i;
			int index1 = j * m_Res[0] + (i + 2);
			double c_p1p2 = (m_NewPos[index0] - m_NewPos[index1]).Length() - vertStrutRestLength * 2;
			Vec3<double> dp1 = (m_NewPos[index0] - m_NewPos[index1]);
			Vec3<double> dp2 = (m_NewPos[index0] - m_NewPos[index1]);
			dp1.Normalize();
			dp2.Normalize();
			dp1 *= -m_InvMass[index0] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			dp2 *= m_InvMass[index1] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			m_NewPos[index0] += dp1;
			m_NewPos[index1] += dp2;
		}
	}
	for (int i = 0; i < m_Res[1]; i++)
	{
		for (int j = 0; j < m_Res[0] - 2; j++)
		{
			int index0 = j * m_Res[0] + i;
			int index1 = (j + 2) * m_Res[0] + i;
			double c_p1p2 = (m_NewPos[index0] - m_NewPos[index1]).Length() - horiStrutRestLength * 2;
			Vec3<double> dp1 = (m_NewPos[index0] - m_NewPos[index1]);
			Vec3<double> dp2 = (m_NewPos[index0] - m_NewPos[index1]);
			dp1.Normalize();
			dp2.Normalize();
			dp1 *= -m_InvMass[index0] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			dp2 *= m_InvMass[index1] / (m_InvMass[index0] + m_InvMass[index1]) * c_p1p2;
			m_NewPos[index0] += dp1;
			m_NewPos[index1] += dp2;
		}
	}
}