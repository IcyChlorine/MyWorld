#include "Map.h"
#include "GameClasses.h"

Map::Map(int loading_radius) :loading_radius{ loading_radius }
{
	r = vec<int>{ 0,0,0 };//特地强调
	if (loading_radius <= 0) loading_radius = 1;
	LoadAround(vec<float>{ 0,0,0 });
}

Map::~Map(void)
{	
	for (int i = 0; i < data.size(); i++)
		for (int j = 0; j < data[i].size(); j++)
			delete data[i][j];
}

int& Map::operator()(int x,int y,int z)
{
	static int rx,rz;
	static int WTF = 0;
	//relative coordinate，相对于地图原点的坐标
	//地图原点在地图一角，不是中心
	//地图原点：r
	rx=x-r.x;
	rz=z-r.z;
	if (rx < 0 || rz < 0 || y >= 256 || y < 0 || rx >= chunk_size*2*loading_radius || rz>=chunk_size * 2 * loading_radius)
		//MessageBox(NULL,"Map Access Violation.","",MB_OK);
		return WTF;//返回一个可以随意更改的变量
	return (data[rz / 16][rx / 16]->rl_at(rx % 16, y, rz % 16));
}
int& Map::operator()(float x, float y, float z)
{
	return (*this)((int)floor(x), (int)floor(y), (int)floor(z));
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
void Map::RenderAround(vec<float> c,int Range)
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
	vec<int> rNew;	//vNewOrigin
	rNew[0] = x - 16 * loading_radius;
	rNew[1] = 0;
	rNew[2] = z - 16 * loading_radius;
	//由x,y,z换算到新vOrigin的位置
	//注意x,y,z是指地图中心区块内的某位置，而vOrigin则是地图一角的位置

	if (data.empty())
	{
		data.resize(2 * loading_radius + 1);
		for (int i = 0; i <= 2 * loading_radius; i++)
		{
			data[i].resize(2 * loading_radius + 1);
			for (int j = 0; j <= 2 * loading_radius; j++)
				//data[i][j] = LoadChunk(vNew[0] + j * 16, 0, vNew[2] + i * 16);
				data[i][j] = new Chunk(rNew + chunk_size*vec<int>(i, 0, j));
		}
		r = rNew;
		return true;
	}
	else
	{
		if (r==rNew)//不需要变化
			return true;
		else if (abs(r[0] - rNew[0]) > chunk_size || abs(r[2] - rNew[2]) > chunk_size)//前后离得太远，还是重新加载所有区块吧
		{
			for (int i = 0; i < data.size(); i++)
			{
				for (int j = 0; j < data[i].size(); j++)
				{
					delete data[i][j];//chunk类的析构函数中包含了保存的过程了
					data[i][j] = new Chunk(rNew + chunk_size*vec<int>(i, 0, j));
				}
			}
		}
		//以下是我辛苦钻研的滚动时加载技术
		else//地图向某个方向前滚了一个Chunk，则只需要加载新靠近的Chunks，保存远去的Chunks即可
		{
			if (rNew[0] == r[0] + chunk_size)//x轴向正方向移动了一个区块
			{
				for (int j = 0; j < data.size(); j++)
				{
					delete data[0][j];
				}
				data.push_back(data.front());
				data.pop_front();//滚动式加载；类似链环旋转
				for (int j = 0; j < data.size(); j++)
					data[2 * loading_radius][j] = new Chunk(rNew + chunk_size*vec<int>(2 * loading_radius, 0, j));
			}
			else if (rNew[0] == r[0] - chunk_size)//x轴向正方向移动了一个区块
			{
				for (int j = 0; j < data.size(); j++)
				{
					delete data[2*loading_radius][j];
				}
				data.push_front(data.back());
				data.pop_back();//滚动式加载；类似链环旋转
				for (int j = 0; j < data.size(); j++)
					data[0][j] = new Chunk(rNew + chunk_size*vec<int>(0, 0, j));
			}
			else if (rNew[2] == r[2] + chunk_size)//z轴向正方向移动了一个区块
			{
				for (int i = 0; i < data.size();i++)
				{
					delete data[i][0];
					data[i].pop_front();
					data[i].push_back(new Chunk(rNew + chunk_size*vec<int>(i, 0, 2 * loading_radius)));
				}
			}
			else if (rNew[2] == r[2] - chunk_size)//z轴向正方向移动了一个区块
			{
				for (int i = 0; i < data.size(); i++)
				{
					delete data[i].back();
					data[i].pop_back();
					data[i].push_front(new Chunk(rNew + chunk_size*vec<int>(i, 0, 0)));
				}
			}
			r = rNew;
			return true;
		}
	}	
}
