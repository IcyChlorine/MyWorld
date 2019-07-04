#pragma once
#ifndef _MAP_H
#include"..\stdafx.h"
#include"GameMath.h"
#include"Entity.h"

using namespace std;

const int chunk_size = 16;
class Chunk
{
public:
	vec<int> r;//起点位置
	int arr[16][256][16];
	list<Entity> entities;

	//Chunk(string filename = "")
	//{
	//	memset(arr, 0, sizeof(arr));
	//	Init(filename);
	//}
	Chunk(vec<int>& r) :r{ r }
	{
		memset(arr, 0, sizeof(arr));
		char filename[99];
		sprintf(filename, "map\\%d-%d-%d.chk", (int)r.x, (int)r.y, (int)r.z);
		Init(filename);
	}
	~Chunk()
	{
		char filename[99];
		sprintf(filename, "map\\%d-%d-%d.chk", r.x, r.y, r.z);
		Save(filename);
	}
	void Init(string filename="")//初始化区块
	{
		//初始化地形——仅在不从文件中读取，第一次创建区块时使用
		if (filename == "")
		{
			memset(arr, 0, sizeof(arr));
			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					arr[i][0][j] = 1;
					arr[i][1][j] = 1;
				}
			}
			for (int i = 0; i < 16; i++)
				arr[i][1][0] = 2,
				arr[0][1][i] = 2;
		}
		else//从文件中读取的情形
		{
			ifstream fin(filename);
			if (!fin)//文件不存在，那只好再重新建一个
			{
				Init();
			}
			else//从文件中读取
			{
				fin >> (*this);
			}
		}
	}
	void Save(string filename)
	{
		fstream fout(filename);
		fout << *this;
	}

	inline int& rl_at(int x, int y, int z)//按相对坐标取数据
	{
		return arr[x][y][z];
	}
	inline int& abs_at(int x, int y, int z)//按绝对坐标
	{
		return arr[x - r.x][y - r.y][z - r.z];
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
public:
	friend ostream& operator<<(ostream& out, Chunk& c)
	{
		out << c.r << endl;
		//每个区块存储在16棵八分树中，每棵树存储了[16][16][16]的数据
		for (int i = 0; i < 16; i++)
		{
			out << i << " " << endl;
			c._CodeOctTree(out, 0, 16 * i, 0, 16);
			out << endl;
			//读取数据
		}
		return out;
	}
	friend istream& operator>>(istream& in, Chunk& c)
	{
		in >> c.r;
		//每个区块存储在16棵八分树中，每棵树存储了[16][16][16]的数据
		for (int i = 0; i < 16; i++)
		{
			in >> i;
			c._DecodeOctTree(in, 0, 16 * i, 0, 16);
			//读取数据
		}
		return in;
	}
};
class Map
{
private:
	deque<deque<Chunk*>> data;//arraies that really save data
										//第一维对应x方向，第二维对应z方向
	int loading_radius;			//加载半径，即实时加载（2r-1)*(2r-1)个区块
	vec<int> r;	//加载的那部分地图的中心区块所在位置


public:
	Map(int loading_radius = 2);	//初始在(0,0,0)
	~Map(void);

	//都是绝对坐标
	int& operator()(int x, int y, int z);
	int& operator()(float x, float y, float z);
	inline int& at(int x, int y, int z) { return this->operator()(x, y, z); }
	inline int& at(float x, float y, float z) { return this->operator()(x, y, z); }
	template<typename T>
	inline int& at(vec<T> r) { return this->operator()(r.x, r.y, r.z); }
	

	//先把画方块的函数放到class里面
	void RenderCube(float x, float y, float z, float a, int id);
	//渲染以c为中心，半径为range的区域
	void RenderAround(vec<float> c, int Range = 16);
	// Set the location of central Chunk,Data[5]
	bool LoadAround(int x, int y, int z, bool SmartLoad = true);
	inline bool LoadAround(vec<float> r) { return LoadAround(r.x, r.y, r.z); }
};


#endif // !_MAP_H

