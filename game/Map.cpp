#include "Map.h"
#include "GameClasses.h"

Map::Map(int loading_radius) :loading_radius{ loading_radius }
{
	r = vec<int>{ 0,0,0 };//�ص�ǿ��
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
	//relative coordinate������ڵ�ͼԭ�������
	//��ͼԭ���ڵ�ͼһ�ǣ���������
	//��ͼԭ�㣺r
	rx=x-r.x;
	rz=z-r.z;
	if (rx < 0 || rz < 0 || y >= 256 || y < 0 || rx >= chunk_size*2*loading_radius || rz>=chunk_size * 2 * loading_radius)
		//MessageBox(NULL,"Map Access Violation.","",MB_OK);
		return WTF;//����һ������������ĵı���
	return (data[rz / 16][rx / 16]->rl_at(rx % 16, y, rz % 16));
}
int& Map::operator()(float x, float y, float z)
{
	return (*this)((int)floor(x), (int)floor(y), (int)floor(z));
}


//ÿ�������������������[0]���ϱ�������[1]�ǲ�������[2]�ǵ�������
//���У�[1] [2]Ϊ��������/����������ϱ�������һ��
void Map::RenderCube(float x, float y, float z, float a, int id)
{
	/*if (id == 7)
	{
		glEnable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}*///���ڲ��Ե�͸��Ч��


	if (!id) return;
	glBindTexture(GL_TEXTURE_2D, pGame->cube_tex[id][0]);	//ѡ������
	glBegin(GL_QUADS);
	// ����
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + a, y + a, z + a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + a, y + a, z + 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 0, y + a, z + 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 0, y + a, z + a);

	// ����
	if (pGame->cube_tex[id][2])
	{
		glEnd();
		glBindTexture(GL_TEXTURE_2D, pGame->cube_tex[id][2]);	//ѡ������
		glBegin(GL_QUADS);
	}

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + a, y + 0, z + a);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 0, y + 0, z + a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 0, y + 0, z + 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + a, y + 0, z + 0);

	// ����
	if (pGame->cube_tex[id][1] || pGame->cube_tex[id][2])
	{
		glEnd();
		glBindTexture(GL_TEXTURE_2D, pGame->cube_tex[id][1]);	//ѡ������
		glBegin(GL_QUADS);
	}

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + a, y + a, z + 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + a, y + 0, z + 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 0, y + 0, z + 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 0, y + a, z + 0);
	// ǰ��
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + a, y + a, z + a);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 0, y + a, z + a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 0, y + 0, z + a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + a, y + 0, z + a);
	// ����
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 0, y + a, z + a);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 0, y + a, z + 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 0, y + 0, z + 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 0, y + 0, z + a);
	// ����
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
	}*///���ڲ��Ե�͸��Ч��

	return;
}
// ����
void Map::RenderAround(vec<float> c,int Range)
{
	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, texture);				// ѡ������

	static int LBorder=(c.z-16>=0)?(c.z-16):0;
	for(int i=c.x-Range;i<c.x+Range;i++)
	{for(int j=LBorder;j<c.y+Range;j++)
	{for(int k=c.z-Range;k<c.z+Range;k++)
	{
		RenderCube(i, j, k,//����λ��
			1,		//����ı߳�
			(*this)(i,j,k));//Ҫ��ʲô
			
	}}}
	glDisable(GL_TEXTURE_2D);
	return;
}


// Set the location of central Chunk,Data[5]
// ���ڲ�ͣ���ߣ�����ͼ��������Զ������ȥ��ȥ�������������顣���ֻ��һС�����򱻼��ص��ڴ��С�
// ���������Ŀ�����õ�ͼ��(x,y,z)����Ϊ���ģ����£�������Χ�ĵ������ݡ�
bool Map::LoadAround(int x, int y, int z, bool SmartLoad)
{
	//��[16k,16k+16)�е�ȫ��������������16k
	//����Ҫ�����������%/*����ʹ�ã���õķ���������floor
	x = (int)floor((float)x / 16) * 16;
	z = (int)floor((float)z / 16) * 16;
	vec<int> rNew;	//vNewOrigin
	rNew[0] = x - 16 * loading_radius;
	rNew[1] = 0;
	rNew[2] = z - 16 * loading_radius;
	//��x,y,z���㵽��vOrigin��λ��
	//ע��x,y,z��ָ��ͼ���������ڵ�ĳλ�ã���vOrigin���ǵ�ͼһ�ǵ�λ��

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
		if (r==rNew)//����Ҫ�仯
			return true;
		else if (abs(r[0] - rNew[0]) > chunk_size || abs(r[2] - rNew[2]) > chunk_size)//ǰ�����̫Զ���������¼������������
		{
			for (int i = 0; i < data.size(); i++)
			{
				for (int j = 0; j < data[i].size(); j++)
				{
					delete data[i][j];//chunk������������а����˱���Ĺ�����
					data[i][j] = new Chunk(rNew + chunk_size*vec<int>(i, 0, j));
				}
			}
		}
		//���������������еĹ���ʱ���ؼ���
		else//��ͼ��ĳ������ǰ����һ��Chunk����ֻ��Ҫ�����¿�����Chunks������Զȥ��Chunks����
		{
			if (rNew[0] == r[0] + chunk_size)//x�����������ƶ���һ������
			{
				for (int j = 0; j < data.size(); j++)
				{
					delete data[0][j];
				}
				data.push_back(data.front());
				data.pop_front();//����ʽ���أ�����������ת
				for (int j = 0; j < data.size(); j++)
					data[2 * loading_radius][j] = new Chunk(rNew + chunk_size*vec<int>(2 * loading_radius, 0, j));
			}
			else if (rNew[0] == r[0] - chunk_size)//x�����������ƶ���һ������
			{
				for (int j = 0; j < data.size(); j++)
				{
					delete data[2*loading_radius][j];
				}
				data.push_front(data.back());
				data.pop_back();//����ʽ���أ�����������ת
				for (int j = 0; j < data.size(); j++)
					data[0][j] = new Chunk(rNew + chunk_size*vec<int>(0, 0, j));
			}
			else if (rNew[2] == r[2] + chunk_size)//z�����������ƶ���һ������
			{
				for (int i = 0; i < data.size();i++)
				{
					delete data[i][0];
					data[i].pop_front();
					data[i].push_back(new Chunk(rNew + chunk_size*vec<int>(i, 0, 2 * loading_radius)));
				}
			}
			else if (rNew[2] == r[2] - chunk_size)//z�����������ƶ���һ������
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
