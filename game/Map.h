#pragma once
#ifndef _MAP_H
#include"..\stdafx.h"
#include"GameMath.h"
#include"Entity.h"

using namespace std;
typedef int(*Chunk)[256][16];
class Chunk
{
public:
	vec r;//起点位置
	int arr[16][256][16];
	list<Entity> entities;

	Chunk()
	{
		memset(arr, 0, sizeof(arr));
	}
	Chunk(vec& r) :r{ r }
	{
		memset(arr, 0, sizeof(arr));
	}
	int& rl_at(int x, int y, int z)//按相对坐标取数据
	{
		return arr[x][y][z];
	}
	int& abs_at(int x, int y, int z)//按绝对坐标
	{
		return arr[x - (int)r.x][y - (int)r.y][z - (int)r.z];
	}

private:
	// 将一部分地图数据转化为八分树，八分树写到out中，用以减少数据冗余度
	// 转化的部分是 [rx,rx+a),[ry,ry+a),[rz,rz+a); r表示是相对坐标
	void _CodeOctTree(ostream& out, int rx, int ry, int rz, int a)
	{
		int u = arr[rx][ry][rz];
		bool flag = true;//这一部分的数据是否完全一样 true代表一样，只要用一个结点就可以表示
						 //false代表不完全一样，需要建子树
		for (int dx = 0; dx < a&&flag; dx++)
			for (int dy = 0; dy < a&&flag; dy++)
				for (int dz = 0; dz < a&&flag; dz++)
					if (arr[rx + dx][ry + dy][rz + dz] != u)
						flag = false;
		if (flag)
			out << u << " ";//为了读入方便，两个结点数据之间加一个空格
		else
		{
			out << -1 << " ";//-1是结点标志，代表还没到叶子
			_CodeOctTree(out, rx + a / 2, ry + a / 2, rz + a / 2, a / 2);
			_CodeOctTree(out, rx + a / 2, ry + a / 2, rz, a / 2);
			_CodeOctTree(out, rx + a / 2, ry, rz + a / 2, a / 2);
			_CodeOctTree(out, rx + a / 2, ry, rz, a / 2);
			_CodeOctTree(out, rx, ry + a / 2, rz + a / 2, a / 2);
			_CodeOctTree(out, rx, ry + a / 2, rz, a / 2);
			_CodeOctTree(out, rx, ry, rz + a / 2, a / 2);
			_CodeOctTree(out, rx, ry, rz, a / 2);
			//递归调用
		}
		return;
	}


	// 与CodeOctTree相对应，用于解析一棵八分树，从in中读取，写到c中
	void _DecodeOctTree(istream& in, int rx, int ry, int rz, int a)
	{
		int u;
		in >> u;
		if (u == -1)//遇到非叶子节点
		{
			_DecodeOctTree(in, rx + a / 2, ry + a / 2, rz + a / 2, a / 2);
			_DecodeOctTree(in, rx + a / 2, ry + a / 2, rz, a / 2);
			_DecodeOctTree(in, rx + a / 2, ry, rz + a / 2, a / 2);
			_DecodeOctTree(in, rx + a / 2, ry, rz, a / 2);
			_DecodeOctTree(in, rx, ry + a / 2, rz + a / 2, a / 2);
			_DecodeOctTree(in, rx, ry + a / 2, rz, a / 2);
			_DecodeOctTree(in, rx, ry, rz + a / 2, a / 2);
			_DecodeOctTree(in, rx, ry, rz, a / 2);
			//递归调用
		}
		else
			for (int dx = 0; dx < a; dx++)
				for (int dy = 0; dy < a; dy++)
					for (int dz = 0; dz < a; dz++)
						arr[rx + dx][ry + dy][rz + dz] = u;
		//将这一区域填满u
		return;
	}

	friend ostream& operator<<(ostream& out, Chunk& c)
	{
		out << r << endl;
		//每个区块存储在16棵八分树中，每棵树存储了[16][16][16]的数据
		for (int i = 0; i < 16; i++)
		{
			out << i << " " << endl;
			CodeOctTree(fout, c, 0, 16 * i, 0, 16);
			out << endl;
			//读取数据
		}
		return out;
	}
	friend istream& operator>>(istream& in, Chunk& c)
	{
		in >> r;
		//每个区块存储在16棵八分树中，每棵树存储了[16][16][16]的数据
		for (int i = 0; i < 16; i++)
		{
			in >> i;
			_DecodeOctTree(in, 0, 16 * i, 0, 16);
			//读取数据
		}
		return in;
	}
};
/*class Map
{
private:
Chunk* Data;	//arraies that really save data
int a;			//边长，即地图将在内存中加载a*a个区块
int vOrigin[3];	//地图原点位置/偏移大小
public:
Map(int _a=3);	//初始在(0,0,0)
~Map(void);

int& operator()(int x,int y,int z);
int& operator()(float x, float y, float z);
//int& at(vec r){return this->operator()(r) }

// 从文件中加载区块，若不存在则创建之
Chunk LoadChunk(int x, int y, int z);
bool InitChunk(Chunk c);
// 保存一个区块
bool SaveChunk(int x,int y,int z,Chunk c);

//渲染以c为中心，半径为range的区域
void RenderAround(cord c,int Range=16);
// Set the location of central Chunk,Data[5]
bool LoadAround(int x, int y, int z,bool SmartLoad=true);
// 将一部分地图数据转化为八分树，八分树写到out中，用以减少数据冗余度
void CodeOctTree(ostream& out, Chunk c, int x0, int y0, int z0, int a);
// 与CodeOctTree相对应，用于解析一棵八分树，从in中读取，写到c中
void DecodeOctTree(istream& in, Chunk c, int x0, int y0, int z0, int a);
// 保存所有现在加载的区块
void Save();
};*/


#endif // !_MAP_H

