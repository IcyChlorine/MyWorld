#pragma once
#include "..\stdafx.h"
#include"GameMath.h"
#ifndef _ENTITY_H
#define _ENTITY_H

class CollisionBox//碰撞箱对象
{
public:
	vec<float> r1, r2;//正方体碰撞箱的两个顶点坐标
	bool CollisionTest(const CollisionBox& c2) const//两个碰撞箱是否“碰撞”，即是否重叠
	{
		const CollisionBox& c1 = *this;
		if (min(c1.r1.x, c1.r2.x) > max(c2.r1.x, c2.r2.x) || max(c1.r1.x, c1.r2.x) < min(c2.r1.x, c2.r2.x))
			return false;
		if (min(c1.r1.y, c1.r2.y) > max(c2.r1.y, c2.r2.y) || max(c1.r1.y, c1.r2.y) < min(c2.r1.y, c2.r2.y))
			return false;
		if (min(c1.r1.z, c1.r2.z) > max(c2.r1.z, c2.r2.z) || max(c1.r1.z, c1.r2.z) < min(c2.r1.z, c2.r2.z))
			return false;
		//else
		return true;
	}
	bool CollisionTest(const vec<float>& r) const//与一个点的碰撞检测
	{
		if (r.x<min(r1.x, r2.x) || r.x>max(r1.x, r2.x))
			return false;
		if (r.x<min(r1.y, r2.y) || r.y>max(r1.y, r2.y))
			return false;
		if (r.z<min(r1.z, r2.z) || r.z>max(r1.z, r2.z))
			return false;
		return true;
	}
};
inline bool CollisionTest(const CollisionBox& c1, const CollisionBox& c2)
{
	return c1.CollisionTest(c2);
}

class Entity:public MyClass
{
public:
	vec<float> r;//位矢
	CollisionBox cls_box;
	bool collision_box_relative_to_r = true;//碰撞箱是不是本地坐标
public:
	Entity();
	Entity(void* father) :MyClass{ father } {}
	virtual ~Entity();

	bool CollisionTest(Entity& e2)//碰撞检测
	{
		Entity& e1 = *this;
		if (e1.collision_box_relative_to_r)
		{
			e1.cls_box.r1 += e1.r;
			e1.cls_box.r1 += e1.r;
		}
		if (e2.collision_box_relative_to_r)
		{
			e2.cls_box.r1 += e2.r;
			e2.cls_box.r1 += e2.r;
		}
		bool ans = e1.cls_box.CollisionTest(e2.cls_box);
		if (e1.collision_box_relative_to_r)
		{
			e1.cls_box.r1 -= e1.r;
			e1.cls_box.r1 -= e1.r;
		}
		if (e2.collision_box_relative_to_r)
		{
			e2.cls_box.r1 -= e2.r;
			e2.cls_box.r1 -= e2.r;
		}
	}
};

#endif