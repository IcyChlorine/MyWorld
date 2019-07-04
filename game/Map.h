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
	vec<int> r;//���λ��
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
	void Init(string filename="")//��ʼ������
	{
		//��ʼ�����Ρ������ڲ����ļ��ж�ȡ����һ�δ�������ʱʹ��
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
		else//���ļ��ж�ȡ������
		{
			ifstream fin(filename);
			if (!fin)//�ļ������ڣ���ֻ�������½�һ��
			{
				Init();
			}
			else//���ļ��ж�ȡ
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

	inline int& rl_at(int x, int y, int z)//���������ȡ����
	{
		return arr[x][y][z];
	}
	inline int& abs_at(int x, int y, int z)//����������
	{
		return arr[x - r.x][y - r.y][z - r.z];
	}

private:
	// ��һ���ֵ�ͼ����ת��Ϊ�˷������˷���д��out�У����Լ������������
	// ת���Ĳ����� [rx,rx+a),[ry,ry+a),[rz,rz+a); r��ʾ���������
	void _CodeOctTree(ostream& out, int rx, int ry, int rz, int a)
	{
		int u = arr[rx][ry][rz];
		bool flag = true;//��һ���ֵ������Ƿ���ȫһ�� true����һ����ֻҪ��һ�����Ϳ��Ա�ʾ
						 //false������ȫһ������Ҫ������
		for (int dx = 0; dx < a&&flag; dx++)
			for (int dy = 0; dy < a&&flag; dy++)
				for (int dz = 0; dz < a&&flag; dz++)
					if (arr[rx + dx][ry + dy][rz + dz] != u)
						flag = false;
		if (flag)
			out << u << " ";//Ϊ�˶��뷽�㣬�����������֮���һ���ո�
		else
		{
			out << -1 << " ";//-1�ǽ���־������û��Ҷ��
			_CodeOctTree(out, rx + a / 2, ry + a / 2, rz + a / 2, a / 2);
			_CodeOctTree(out, rx + a / 2, ry + a / 2, rz, a / 2);
			_CodeOctTree(out, rx + a / 2, ry, rz + a / 2, a / 2);
			_CodeOctTree(out, rx + a / 2, ry, rz, a / 2);
			_CodeOctTree(out, rx, ry + a / 2, rz + a / 2, a / 2);
			_CodeOctTree(out, rx, ry + a / 2, rz, a / 2);
			_CodeOctTree(out, rx, ry, rz + a / 2, a / 2);
			_CodeOctTree(out, rx, ry, rz, a / 2);
			//�ݹ����
		}
		return;
	}

	// ��CodeOctTree���Ӧ�����ڽ���һ�ð˷�������in�ж�ȡ��д��c��
	void _DecodeOctTree(istream& in, int rx, int ry, int rz, int a)
	{
		int u;
		in >> u;
		if (u == -1)//������Ҷ�ӽڵ�
		{
			_DecodeOctTree(in, rx + a / 2, ry + a / 2, rz + a / 2, a / 2);
			_DecodeOctTree(in, rx + a / 2, ry + a / 2, rz, a / 2);
			_DecodeOctTree(in, rx + a / 2, ry, rz + a / 2, a / 2);
			_DecodeOctTree(in, rx + a / 2, ry, rz, a / 2);
			_DecodeOctTree(in, rx, ry + a / 2, rz + a / 2, a / 2);
			_DecodeOctTree(in, rx, ry + a / 2, rz, a / 2);
			_DecodeOctTree(in, rx, ry, rz + a / 2, a / 2);
			_DecodeOctTree(in, rx, ry, rz, a / 2);
			//�ݹ����
		}
		else
			for (int dx = 0; dx < a; dx++)
				for (int dy = 0; dy < a; dy++)
					for (int dz = 0; dz < a; dz++)
						arr[rx + dx][ry + dy][rz + dz] = u;
		//����һ��������u
		return;
	}
public:
	friend ostream& operator<<(ostream& out, Chunk& c)
	{
		out << c.r << endl;
		//ÿ������洢��16�ð˷����У�ÿ�����洢��[16][16][16]������
		for (int i = 0; i < 16; i++)
		{
			out << i << " " << endl;
			c._CodeOctTree(out, 0, 16 * i, 0, 16);
			out << endl;
			//��ȡ����
		}
		return out;
	}
	friend istream& operator>>(istream& in, Chunk& c)
	{
		in >> c.r;
		//ÿ������洢��16�ð˷����У�ÿ�����洢��[16][16][16]������
		for (int i = 0; i < 16; i++)
		{
			in >> i;
			c._DecodeOctTree(in, 0, 16 * i, 0, 16);
			//��ȡ����
		}
		return in;
	}
};
class Map
{
private:
	deque<deque<Chunk*>> data;//arraies that really save data
										//��һά��Ӧx���򣬵ڶ�ά��Ӧz����
	int loading_radius;			//���ذ뾶����ʵʱ���أ�2r-1)*(2r-1)������
	vec<int> r;	//���ص��ǲ��ֵ�ͼ��������������λ��


public:
	Map(int loading_radius = 2);	//��ʼ��(0,0,0)
	~Map(void);

	//���Ǿ�������
	int& operator()(int x, int y, int z);
	int& operator()(float x, float y, float z);
	inline int& at(int x, int y, int z) { return this->operator()(x, y, z); }
	inline int& at(float x, float y, float z) { return this->operator()(x, y, z); }
	template<typename T>
	inline int& at(vec<T> r) { return this->operator()(r.x, r.y, r.z); }
	

	//�Ȱѻ�����ĺ����ŵ�class����
	void RenderCube(float x, float y, float z, float a, int id);
	//��Ⱦ��cΪ���ģ��뾶Ϊrange������
	void RenderAround(vec<float> c, int Range = 16);
	// Set the location of central Chunk,Data[5]
	bool LoadAround(int x, int y, int z, bool SmartLoad = true);
	inline bool LoadAround(vec<float> r) { return LoadAround(r.x, r.y, r.z); }
};


#endif // !_MAP_H

