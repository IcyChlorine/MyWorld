//#include "Map.h"
#include "GameClasses.h"
//#include "render.h"

Map::Map(int _a)
{
	if (!(_a % 2))_a++;
	//为了方便起见，地图由a*a个区块组成的话，a最好是奇数
	a=_a;
	LoadAround(0, 0, 0);
}

Map::~Map(void)
{	
	Save();
	for(int i=0;i<a*a;i++)
		delete Data[i];
	delete[] Data;
}

int& Map::operator()(int x,int y,int z)
{
	static int rx,rz;
	static int tmp = 0;
	//relative coordinate，相对于地图原点的坐标
	//地图原点在地图一角，不是中心
	//地图原点：vOrigin
	rx=x-vOrigin[0];
	rz=z-vOrigin[2];
	if (rx < 0 || rz < 0 || y >= 256 || y < 0 || rx >= 16 * a || rz >= 16 * a)
		//MessageBox(NULL,"Map Access Violation.","",MB_OK);
		return tmp;//返回一个可以随意更改的变量
	return (Data[(rz/16)*a+(rx/16)])[rx%16][y][rz%16];
}
int& Map::operator()(float x, float y, float z)
{
	return (*this)((int)floor(x), (int)floor(y), (int)floor(z));
}

// 从文件中加载区块，若不存在则创建之
Chunk Map::LoadChunk(int x, int y, int z)
{
	char str[99];
	sprintf(str, "map\\%d-%d-%d.chk", x, y, z);
	ifstream fin(str);
	Chunk c = new int[16][256][16];
	if (!fin)//区块不存在，则创建之
	{
		Chunk c = new int[16][256][16];
		InitChunk(c);
		return c;
	}
	else
	{
		fin >> x >> y >> z;
		//每个区块存储在16棵八分树中，每棵树存储了[16][16][16]的数据
		for (int i = 0; i < 16; i++)
		{
			fin >> i;
			DecodeOctTree(fin, c, 0, 16 * i, 0, 16);
			//读取数据
		}
		fin.close();
		return c;
	}
}

//初始化一个区块
bool Map::InitChunk(Chunk c)
{
	memset(c, 0, sizeof(int) * 16 * 256 * 16);
	/*for(int i=0;i<16;i++)
	{for(int j=0;j<256;j++)
	{for(int k=0;k<16;k++)
	{
		(Data[Index])[i][j][k]=0;
	}}}*/
	for(int i=0;i<16;i++)
	{for(int j=0;j<16;j++)
	{
		c[i][0][j] = 1;
		c[i][1][j] = 1;
	}}
	for (int i = 0; i < 16; i++)
		c[i][1][0] = 2,
		c[0][1][i] = 2;
		
	return true;
}

// 保存一个区块
bool Map::SaveChunk(int x, int y, int z, Chunk c)
{
	char str[99];
	sprintf(str, "map\\%d-%d-%d.chk", x, y, z);
	ofstream fout(str);

	fout << x << " " << y << " " << z << " "<<endl;
	//每个区块存储在16棵八分树中，每棵树存储了[16][16][16]的数据
	for (int i = 0; i < 16; i++)
	{
		fout << i << " "<<endl;
		CodeOctTree(fout, c, 0, 16 * i, 0, 16);
		fout << endl;
		//读取数据
	}
	fout.close();
	return true;
}

//每个方块的纹理有三个，[0]是上表面纹理，[1]是侧面纹理，[2]是底面纹理
//其中，[1] [2]为零代表侧面/底面纹理和上表面纹理一样
void Map::RenderCube(float x, float y, float z, float a, int id)
{
	/*if (id == 7)
	{
		glEnable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}*///正在测试的透明效果


	if (!id) return;
	glBindTexture(GL_TEXTURE_2D, pGame->cube_tex[id][0]);	//选择纹理
	glBegin(GL_QUADS);
	// 上面
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + a, y + a, z + a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + a, y + a, z + 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 0, y + a, z + 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 0, y + a, z + a);

	// 下面
	if (pGame->cube_tex[id][2])
	{
		glEnd();
		glBindTexture(GL_TEXTURE_2D, pGame->cube_tex[id][2]);	//选择纹理
		glBegin(GL_QUADS);
	}

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + a, y + 0, z + a);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 0, y + 0, z + a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 0, y + 0, z + 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + a, y + 0, z + 0);

	// 后面
	if (pGame->cube_tex[id][1] || pGame->cube_tex[id][2])
	{
		glEnd();
		glBindTexture(GL_TEXTURE_2D, pGame->cube_tex[id][1]);	//选择纹理
		glBegin(GL_QUADS);
	}

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + a, y + a, z + 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + a, y + 0, z + 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 0, y + 0, z + 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 0, y + a, z + 0);
	// 前面
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + a, y + a, z + a);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 0, y + a, z + a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 0, y + 0, z + a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + a, y + 0, z + a);
	// 左面
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 0, y + a, z + a);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 0, y + a, z + 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 0, y + 0, z + 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 0, y + 0, z + a);
	// 右面
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + a, y + a, z + a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + a, y + 0, z + a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + a, y + 0, z + 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + a, y + a, z + 0);
	glEnd();

	/*if (id == 7)
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}*///正在测试的透明效果

	return;
}
// 画！
void Map::RenderAround(vec c,int Range)
{
	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, texture);				// 选择纹理

	static int LBorder=(c.z-16>=0)?(c.z-16):0;
	for(int i=c.x-Range;i<c.x+Range;i++)
	{for(int j=LBorder;j<c.y+Range;j++)
	{for(int k=c.z-Range;k<c.z+Range;k++)
	{
		RenderCube(i, j, k,//画的位置
			1,		//方块的边长
			(*this)(i,j,k));//要画什么
			
	}}}
	glDisable(GL_TEXTURE_2D);
	return;
}


// Set the location of central Chunk,Data[5]
// 人在不停地走，但地图不可能永远加载下去，去加载无穷多个区块。因此只有一小块区域被加载到内存中。
// 这个函数的目的是让地图以(x,y,z)坐标为中心（重新）加载周围的地形数据。
bool Map::LoadAround(int x, int y, int z, bool SmartLoad)
{
	//将[16k,16k+16)中的全部整数向下掐到16k
	//由于要处理负数，因此%/*不能使用，最好的方法就是用floor
	x = (int)floor((float)x / 16) * 16;
	z = (int)floor((float)z / 16) * 16;
	int vNew[3];	//vNewOrigin
	vNew[0] = x - 16 * (a / 2);
	vNew[1] = 0;
	vNew[2] = z - 16 * (a / 2);
	//由x,y,z换算到新vOrigin的位置
	//注意x,y,z是指地图中心区块内的某位置，而vOrigin则是地图一角的位置
	if (SmartLoad)
	{
		if (!Data)
		{
			Data = new Chunk[a*a];
			for (int i = 0; i < a; i++)
				for (int j = 0; j < a; j++)
					Data[i*a + j] = LoadChunk(vNew[0] + j * 16, 0, vNew[2] + i * 16);
			memcpy(vOrigin, vNew, sizeof(int) * 3);
			return true;
		}
		else
		{
			if (vNew[0] == vOrigin[0] && vNew[2] == vOrigin[2])//不需要变化
				return true;
			else if (abs(vNew[0] - vOrigin[0]) > 16 || abs(vNew[2] - vOrigin[2]) > 16)
			{
				Save();
				for (int i = 0; i < a; i++)
					for (int j = 0; j < a; j++)
						Data[a*j + i] = LoadChunk(vNew[0] + 16 * j, 0, vNew[2] + 16 * i);
				memcpy(vOrigin, vNew, sizeof(int) * 3);
				return true;
			}
			else//地图向某个方向前滚了一个Chunk，则只需要加载新靠近的Chunks，保存远去的Chunks即可
			{
				if (vNew[0] == vOrigin[0] + 16)//x轴向正方向移动了一个区块
				{
					for (int i = 0; i < a; i++)
					{
						SaveChunk(vOrigin[0], 0, vOrigin[2] + i * 16, Data[i*a]);
						for (int j = 0; j < a - 1; j++)
							Data[i*a + j] = Data[i*a + (j + 1)];
						Data[i*a + a - 1] = LoadChunk(vNew[0] + 16 * (a - 1), 0, vNew[2] + i * 16);
					}
				}
				else if (vNew[0] == vOrigin[0] - 16)//x轴向负方向移动了一个区块
				{
					for (int i = 0; i < a; i++)
					{
						SaveChunk(vOrigin[0] + 16 * (a - 1), 0, vOrigin[2] + 16 * i, Data[i*a + a - 1]);;
						for (int j = a - 1; j >= 0; j--)
							Data[i*a + j] = Data[i*a + (j - 1)];
						Data[i*a] = LoadChunk(vNew[0], 0, vNew[2] + i * 16);
					}
				}
				else if (vNew[2] == vOrigin[2] + 16)//z轴向正方向移动了一个区块
				{
					for (int j = 0; j < a; j++)
					{
						SaveChunk(vOrigin[0] + j * 16, 0, vOrigin[2], Data[j]);
						for (int i = 0; i < a - 1; i++)
							Data[i*a + j] = Data[(i + 1)*a + j];
						Data[a*(a - 1) + j] = LoadChunk(vNew[0] + 16 * j, 0, vNew[2] + (a - 1) * 16);
					}
				}
				else if (vNew[2] == vOrigin[2] - 16)//z轴向负方向移动了一个区块
				{
					for (int j = 0; j < a; j++)
					{
						SaveChunk(vOrigin[0] + j * 16, 0, vOrigin[2] + (a - 1) * 16, Data[a*(a - 1) + j]);
						for (int i = a - 1; i >= 0; i--)
							Data[i*a + j] = Data[(i - 1)*a + j];
						Data[j] = LoadChunk(vNew[0] + 16 * j, 0, vNew[2]);
					}
				}

				memcpy(vOrigin, vNew, sizeof(int) * 3);
				return true;
			}
		}
	}
	else//没有SmartLoad的情况
	{
		if (!Data)
		{
			return false;
		}
		else
		{
			Save();
			for (int i = 0; i < a; i++)
				for (int j = 0; j < a; j++)
					Data[a*j + i] = LoadChunk(vNew[0] + 16 * j, 0, vNew[2] + 16 * i);
			memcpy(vOrigin, vNew, sizeof(int) * 3);
			return true;
		}
	}

	
	
}


// 将一部分地图数据转化为八分树，八分树写到out中，用以减少数据冗余度
void Map::CodeOctTree(ostream& out, Chunk c, int x0, int y0, int z0, int a)
{
	int u = c[x0][y0][z0];
	bool flag = true;//这一部分的数据是否完全一样 true代表一样，只要用一个结点就可以表示
	//false代表不完全一样，需要建子树
	for (int dx = 0; dx < a&&flag; dx++)
		for (int dy = 0; dy < a&&flag; dy++)
			for (int dz = 0; dz < a&&flag; dz++)
				if (c[x0 + dx][y0 + dy][z0 + dz] != u)
					flag = false;
	if (flag)
		out << u << " ";//为了读入方便，两个结点数据之间加一个空格
	else
	{
		out << -1 << " ";//-1是结点标志，代表还没到叶子
		CodeOctTree(out, c, x0 + a / 2, y0 + a / 2, z0 + a / 2, a / 2);
		CodeOctTree(out, c, x0 + a / 2, y0 + a / 2, z0          , a / 2);
		CodeOctTree(out, c, x0 + a / 2, y0          , z0 + a / 2, a / 2);
		CodeOctTree(out, c, x0 + a / 2, y0          , z0          , a / 2);
		CodeOctTree(out, c, x0          , y0 + a / 2, z0 + a / 2, a / 2);
		CodeOctTree(out, c, x0          , y0 + a / 2, z0          , a / 2);
		CodeOctTree(out, c, x0          , y0          , z0 + a / 2, a / 2);
		CodeOctTree(out, c, x0          , y0          , z0          , a / 2);
		//递归调用
	}
	return;
}


// 与CodeOctTree相对应，用于解析一棵八分树，从in中读取，写到c中
void Map::DecodeOctTree(istream& in, Chunk c, int x0, int y0, int z0, int a)
{
	int u;
	in >> u;
	if (u == -1)//遇到非叶子节点
	{
		DecodeOctTree(in, c, x0 + a / 2, y0 + a / 2, z0 + a / 2, a / 2);
		DecodeOctTree(in, c, x0 + a / 2, y0 + a / 2, z0          , a / 2);
		DecodeOctTree(in, c, x0 + a / 2, y0          , z0 + a / 2, a / 2);
		DecodeOctTree(in, c, x0 + a / 2, y0          , z0          , a / 2);
		DecodeOctTree(in, c, x0          , y0 + a / 2, z0 + a / 2, a / 2);
		DecodeOctTree(in, c, x0          , y0 + a / 2, z0          , a / 2);
		DecodeOctTree(in, c, x0          , y0          , z0 + a / 2, a / 2);
		DecodeOctTree(in, c, x0          , y0          , z0          , a / 2);
		//递归调用
	}
	else
		for (int dx = 0; dx < a; dx++)
			for (int dy = 0; dy < a; dy++)
				for (int dz = 0; dz < a; dz++)
					c[x0 + dx][y0 + dy][z0 + dz] = u;
	//将这一区域填满u
	return;
}


// 保存所有现在加载的区块
void Map::Save()
{
	for (int i = 0; i < a; i++)
		for (int j = 0; j < a; j++)
			SaveChunk(vOrigin[0] + j * 16, 0, vOrigin[2] + i * 16, Data[i*a + j]);
}
