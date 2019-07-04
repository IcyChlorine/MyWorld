#pragma once
#include"..\stdafx.h"
#ifndef _GAME_MATH_H
#define _GAME_MATH_H

inline float rad(float deg) { return deg / 180.0f*3.141592f; }

//三维坐标，重载了一些常用运算
template<typename T = float>
struct vec
{
	vec() {}
	vec(T x, T y, T z) :x{ x }, y{ y }, z{ z } {}
	T x{ 0 }, y{ 0 }, z{ 0 };
	T& operator[](int idx)
	{
		if (idx == 0) return x;
		if (idx == 1) return y;
		if (idx == 2) return z;
	}
};
template<typename T>
bool operator==(const vec<T>& v1, const vec<T>& v2);
template<typename T>
vec<T> operator+(const vec<T>& v1, const vec<T>& v2);
template<typename T>
vec<T> operator-(const vec<T>& v1, const vec<T>& v2);
template<typename T>
vec<T> operator*(const T& k, const vec<T>& v);
template<typename T>
vec<T> operator*( const vec<T>& v, const T& k);

template<typename T>
vec<T>& operator+=(vec<T>& v, const vec<T>& dv);
template<typename T>
vec<T>& operator-=(vec<T>& v, const vec<T>& dv);
template<typename T>
vec<T> operator*=(vec<T>& v, const T& k);
template<typename T>
ostream& operator<<(ostream& out, vec<T>& v);
template<typename T>
istream& operator>>(istream& in, vec<T>& v);

template<typename T>
bool operator==(const vec<T>& v1, const vec<T>& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}
template<typename T>
vec<T> operator+(const vec<T>& v1, const vec<T>& v2)
{
	return vec<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
template<typename T>
vec<T> operator-(const vec<T>& v1, const vec<T>& v2)
{
	return vec<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
template<typename T>
vec<T> operator*(const T& k, const vec<T>& v)
{
	return vec<T>(k*v.x, k*v.y, k*v.z);
}
template<typename T>
vec<T> operator*(const vec<T>& v, const T& k)
{
	return vec<T>(k*v.x, k*v.y, k*v.z);
}

template<typename T>
vec<T>& operator+=(vec<T>& v, const vec<T>& dv)
{
	v = v + dv;
	return v;
}
template<typename T>
vec<T>& operator-=(vec<T>& v, const vec<T>& dv)
{
	v = v - dv;
	return v;
}
template<typename T>
vec<T> operator*=(vec<T>& v, const T& k)
{
	v = k*v;
	return v;
}
template<typename T>
ostream& operator<<(ostream& out, vec<T>& v)
{
	out << v.x << " " << v.y << " " << v.z;
	return out;
}
template<typename T>
istream& operator >> (istream& in, vec<T>& v)
{
	in >> v.x >> v.y >> v.z;
	return in;
}

#endif // !_GAME_MATH_H

