#pragma once
#include"..\stdafx.h"
#ifndef _GAME_MATH_H
#define _GAME_MATH_H

inline float rad(float deg) { return deg / 180.0f*3.141592f; }

//三维坐标，重载了一些常用运算
struct vec
{
	vec() {}
	vec(float x, float y, float z) :x{ x }, y{ y }, z{ z } {}
	float x{ 0 }, y{ 0 }, z{ 0 };
	float& operator[](int idx)
	{
		if (idx == 0) return x;
		if (idx == 1) return y;
		if (idx == 2) return z;
		
	}
};
vec operator+(const vec& v1, const vec& v2);
vec operator-(const vec& v1, const vec& v2);
vec operator*(const float& k, const vec& v);
vec& operator+=(vec& v, const vec& dv);
vec& operator-=(vec& v, const vec& dv);
vec operator*=(vec& v, const float& k);
ostream& operator<<(ostream& out, vec& v);
istream& operator>>(istream& in, vec& v);

#endif // !_GAME_MATH_H

