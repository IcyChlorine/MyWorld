#include"GameMath.h"

vec operator+(const vec& v1, const vec& v2)
{
	return vec(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
vec operator-(const vec& v1, const vec& v2)
{
	return vec(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
vec operator*(const float& k, const vec& v)
{
	return vec(k*v.x, k*v.y, k*v.z);
}
vec& operator+=(vec& v, const vec& dv)
{
	v = v + dv;
	return v;
}
vec& operator-=(vec& v, const vec& dv)
{
	v = v - dv;
	return v;
}
vec operator*=(vec& v, const T& k)
{
	v = k*v;
	return v;
}