#include "Character.h"



Character::Character()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	theta = 0.0f;
	phi = 0.0f;
	//pace = 0.0f;
	//rot_pace = 0.0f;
}


Character::~Character()
{
}


// 返回角色的坐标
cord Character::GetCord()
{
	cord c;
	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}


void Character::Move(float dx, float dy, float dz)
{
	x += dx;
	y += dy;
	z += dz;
}


// 提供OpenGL中的视角变换
void Character::Transform(int Mode)
{
	if (Mode == 0)
	{
		glLoadIdentity();
		
		glRotatef(-phi, 1.0f, 0.0f, 0.0f);//注意符号
		glRotatef(-theta, 0.0f, 1.0f, 0.0f);	//glrotate以逆时针旋转为正
		glTranslatef(-x, -y, -z);
	}
}


void Character::Rotate(float dtheta, float dphi)
{
	theta += dtheta*rot_pace;
	phi += dphi*rot_pace;
	//IF MODE==1
	if (phi > 90.0f) phi = 90.0f;
	if (phi < -90.0f)	phi = -90.0f;
}


void Character::MoveForward(float d)
{
	x -= d*pace*sin(DEGTORAD(theta));
	z -= d*pace*cos(DEGTORAD(theta));
}


void Character::MoveRight(float d)
{
	//出于某种未知的原因，这里没法用宏，不然就会算错，即使宏中加了括号
	x -= d*pace*sin(DEGTORAD(theta - 90.0f));
	z -= d*pace*cos(DEGTORAD(theta - 90.0f));
}


void Character::MoveUp(float d)
{
	y += d*pace;
}


// //返回一个指向前的单位向量，以三维坐标形式表示(cord)
cord Character::GetForward()
{
	cord c=cord();
	c.x = -sin(DEGTORAD(theta));
	c.z = -cos(DEGTORAD(theta));
	c.x *= cos(DEGTORAD(phi));
	c.z *= cos(DEGTORAD(phi));
	c.y = sin(DEGTORAD(phi));
	//三角函数的基本运用，画画图就知道了
	return c;
}

cord operator*(Matrix T, cord v)
{
	assert(T.row() == 3 && T.col() == 3);
	cord c;
	c.x = T(1, 1)*v.x + T(1, 2)*v.y + T(1, 3)*v.z;
	c.y = T(2, 1)*v.x + T(2, 2)*v.y + T(2, 3)*v.z;
	c.z = T(3, 1)*v.x + T(3, 2)*v.y + T(3, 3)*v.z;
	return cord();
}

Matrix GetRotationMatrix(float theta, float phi)
{
	vector<float> v_phi =
		{ 1,0,0,
		0,cos(phi),-sin(phi),
		0,sin(phi),cos(phi) };
	vector<float> v_theta = 
		{cos(theta),0,-sin(phi),
		0,1,0,
		sin(theta),0,cos(theta) };
	Matrix T1(3, 3,v_phi);
	Matrix T2(3, 3,v_theta);

	return T1*T2;
}
