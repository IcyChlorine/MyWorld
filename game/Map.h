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
	vec r;//���λ��
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
	int& rl_at(int x, int y, int z)//���������ȡ����
	{
		return arr[x][y][z];
	}
	int& abs_at(int x, int y, int z)//����������
	{
		return arr[x - (int)r.x][y - (int)r.y][z - (int)r.z];
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

	friend ostream& operator<<(ostream& out, Chunk& c)
	{
		out << r << endl;
		//ÿ������洢��16�ð˷����У�ÿ�����洢��[16][16][16]������
		for (int i = 0; i < 16; i++)
		{
			out << i << " " << endl;
			CodeOctTree(fout, c, 0, 16 * i, 0, 16);
			out << endl;
			//��ȡ����
		}
		return out;
	}
	friend istream& operator>>(istream& in, Chunk& c)
	{
		in >> r;
		//ÿ������洢��16�ð˷����У�ÿ�����洢��[16][16][16]������
		for (int i = 0; i < 16; i++)
		{
			in >> i;
			_DecodeOctTree(in, 0, 16 * i, 0, 16);
			//��ȡ����
		}
		return in;
	}
};
/*class Map
{
private:
Chunk* Data;	//arraies that really save data
int a;			//�߳�������ͼ�����ڴ��м���a*a������
int vOrigin[3];	//��ͼԭ��λ��/ƫ�ƴ�С
public:
Map(int _a=3);	//��ʼ��(0,0,0)
~Map(void);

int& operator()(int x,int y,int z);
int& operator()(float x, float y, float z);
//int& at(vec r){return this->operator()(r) }

// ���ļ��м������飬���������򴴽�֮
Chunk LoadChunk(int x, int y, int z);
bool InitChunk(Chunk c);
// ����һ������
bool SaveChunk(int x,int y,int z,Chunk c);

//��Ⱦ��cΪ���ģ��뾶Ϊrange������
void RenderAround(cord c,int Range=16);
// Set the location of central Chunk,Data[5]
bool LoadAround(int x, int y, int z,bool SmartLoad=true);
// ��һ���ֵ�ͼ����ת��Ϊ�˷������˷���д��out�У����Լ������������
void CodeOctTree(ostream& out, Chunk c, int x0, int y0, int z0, int a);
// ��CodeOctTree���Ӧ�����ڽ���һ�ð˷�������in�ж�ȡ��д��c��
void DecodeOctTree(istream& in, Chunk c, int x0, int y0, int z0, int a);
// �����������ڼ��ص�����
void Save();
};*/


#endif // !_MAP_H

