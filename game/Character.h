#pragma once
#include<cmath>
#include"Matrix.h"
#include<glut.h>
#define PI 3.141592f

//#define DEGTORAD(x) (x*PI/180.0f)
inline float DEGTORAD(float x);
float DEGTORAD(float x)
{
	return x*PI / 180.0f;
}
struct cord
{
	float x, y, z;
	cord(float x = 0.0f, float y = 0.0f, float z = 0.0f) :x(x), y(x), z(z) {}
};
cord operator* (Matrix T,cord v);
Matrix GetRotationMatrix(float theta, float phi);

class Character
{
public:
	Character();
	~Character();
	// 返回角色的坐标
	cord GetCord();
private:
	float x,y,z;
	float theta,phi;
	//theta 和 phi 记录了人物视角旋转的角度，theta代表向左转了多少度，phi代表向上转了多少度
	const float pace=1.0f;
	const float rot_pace=1.0f;
public:
	void Move(float dx, float dy, float dz);
	// 提供OpenGL中的视角变换
	void Transform(int Mode=0);
	void Rotate(float dtheta, float dphi);
	void MoveForward(float d);
	void MoveRight(float d);
	void MoveUp(float d);
	// //返回一个指向前的向量，以三维坐标形式表示(cord)
	cord GetForward();
	float GetTheta() { return theta; }
	float GetPhi() { return phi; }
};

