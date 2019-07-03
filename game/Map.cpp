//#include "Map.h"
#include "GameClasses.h"
//#include "render.h"

Map::Map(int _a)
{
	if (!(_a % 2))_a++;
	//Ϊ�˷����������ͼ��a*a��������ɵĻ���a���������
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
	//relative coordinate������ڵ�ͼԭ�������
	//��ͼԭ���ڵ�ͼһ�ǣ���������
	//��ͼԭ�㣺vOrigin
	rx=x-vOrigin[0];
	rz=z-vOrigin[2];
	if (rx < 0 || rz < 0 || y >= 256 || y < 0 || rx >= 16 * a || rz >= 16 * a)
		//MessageBox(NULL,"Map Access Violation.","",MB_OK);
		return tmp;//����һ������������ĵı���
	return (Data[(rz/16)*a+(rx/16)])[rx%16][y][rz%16];
}
int& Map::operator()(float x, float y, float z)
{
	return (*this)((int)floor(x), (int)floor(y), (int)floor(z));
}

// ���ļ��м������飬���������򴴽�֮
Chunk Map::LoadChunk(int x, int y, int z)
{
	char str[99];
	sprintf(str, "map\\%d-%d-%d.chk", x, y, z);
	ifstream fin(str);
	Chunk c = new int[16][256][16];
	if (!fin)//���鲻���ڣ��򴴽�֮
	{
		Chunk c = new int[16][256][16];
		InitChunk(c);
		return c;
	}
	else
	{
		fin >> x >> y >> z;
		//ÿ������洢��16�ð˷����У�ÿ�����洢��[16][16][16]������
		for (int i = 0; i < 16; i++)
		{
			fin >> i;
			DecodeOctTree(fin, c, 0, 16 * i, 0, 16);
			//��ȡ����
		}
		fin.close();
		return c;
	}
}

//��ʼ��һ������
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

// ����һ������
bool Map::SaveChunk(int x, int y, int z, Chunk c)
{
	char str[99];
	sprintf(str, "map\\%d-%d-%d.chk", x, y, z);
	ofstream fout(str);

	fout << x << " " << y << " " << z << " "<<endl;
	//ÿ������洢��16�ð˷����У�ÿ�����洢��[16][16][16]������
	for (int i = 0; i < 16; i++)
	{
		fout << i << " "<<endl;
		CodeOctTree(fout, c, 0, 16 * i, 0, 16);
		fout << endl;
		//��ȡ����
	}
	fout.close();
	return true;
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
void Map::RenderAround(vec c,int Range)
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
	int vNew[3];	//vNewOrigin
	vNew[0] = x - 16 * (a / 2);
	vNew[1] = 0;
	vNew[2] = z - 16 * (a / 2);
	//��x,y,z���㵽��vOrigin��λ��
	//ע��x,y,z��ָ��ͼ���������ڵ�ĳλ�ã���vOrigin���ǵ�ͼһ�ǵ�λ��
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
			if (vNew[0] == vOrigin[0] && vNew[2] == vOrigin[2])//����Ҫ�仯
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
			else//��ͼ��ĳ������ǰ����һ��Chunk����ֻ��Ҫ�����¿�����Chunks������Զȥ��Chunks����
			{
				if (vNew[0] == vOrigin[0] + 16)//x�����������ƶ���һ������
				{
					for (int i = 0; i < a; i++)
					{
						SaveChunk(vOrigin[0], 0, vOrigin[2] + i * 16, Data[i*a]);
						for (int j = 0; j < a - 1; j++)
							Data[i*a + j] = Data[i*a + (j + 1)];
						Data[i*a + a - 1] = LoadChunk(vNew[0] + 16 * (a - 1), 0, vNew[2] + i * 16);
					}
				}
				else if (vNew[0] == vOrigin[0] - 16)//x���򸺷����ƶ���һ������
				{
					for (int i = 0; i < a; i++)
					{
						SaveChunk(vOrigin[0] + 16 * (a - 1), 0, vOrigin[2] + 16 * i, Data[i*a + a - 1]);;
						for (int j = a - 1; j >= 0; j--)
							Data[i*a + j] = Data[i*a + (j - 1)];
						Data[i*a] = LoadChunk(vNew[0], 0, vNew[2] + i * 16);
					}
				}
				else if (vNew[2] == vOrigin[2] + 16)//z�����������ƶ���һ������
				{
					for (int j = 0; j < a; j++)
					{
						SaveChunk(vOrigin[0] + j * 16, 0, vOrigin[2], Data[j]);
						for (int i = 0; i < a - 1; i++)
							Data[i*a + j] = Data[(i + 1)*a + j];
						Data[a*(a - 1) + j] = LoadChunk(vNew[0] + 16 * j, 0, vNew[2] + (a - 1) * 16);
					}
				}
				else if (vNew[2] == vOrigin[2] - 16)//z���򸺷����ƶ���һ������
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
	else//û��SmartLoad�����
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


// ��һ���ֵ�ͼ����ת��Ϊ�˷������˷���д��out�У����Լ������������
void Map::CodeOctTree(ostream& out, Chunk c, int x0, int y0, int z0, int a)
{
	int u = c[x0][y0][z0];
	bool flag = true;//��һ���ֵ������Ƿ���ȫһ�� true����һ����ֻҪ��һ�����Ϳ��Ա�ʾ
	//false������ȫһ������Ҫ������
	for (int dx = 0; dx < a&&flag; dx++)
		for (int dy = 0; dy < a&&flag; dy++)
			for (int dz = 0; dz < a&&flag; dz++)
				if (c[x0 + dx][y0 + dy][z0 + dz] != u)
					flag = false;
	if (flag)
		out << u << " ";//Ϊ�˶��뷽�㣬�����������֮���һ���ո�
	else
	{
		out << -1 << " ";//-1�ǽ���־������û��Ҷ��
		CodeOctTree(out, c, x0 + a / 2, y0 + a / 2, z0 + a / 2, a / 2);
		CodeOctTree(out, c, x0 + a / 2, y0 + a / 2, z0          , a / 2);
		CodeOctTree(out, c, x0 + a / 2, y0          , z0 + a / 2, a / 2);
		CodeOctTree(out, c, x0 + a / 2, y0          , z0          , a / 2);
		CodeOctTree(out, c, x0          , y0 + a / 2, z0 + a / 2, a / 2);
		CodeOctTree(out, c, x0          , y0 + a / 2, z0          , a / 2);
		CodeOctTree(out, c, x0          , y0          , z0 + a / 2, a / 2);
		CodeOctTree(out, c, x0          , y0          , z0          , a / 2);
		//�ݹ����
	}
	return;
}


// ��CodeOctTree���Ӧ�����ڽ���һ�ð˷�������in�ж�ȡ��д��c��
void Map::DecodeOctTree(istream& in, Chunk c, int x0, int y0, int z0, int a)
{
	int u;
	in >> u;
	if (u == -1)//������Ҷ�ӽڵ�
	{
		DecodeOctTree(in, c, x0 + a / 2, y0 + a / 2, z0 + a / 2, a / 2);
		DecodeOctTree(in, c, x0 + a / 2, y0 + a / 2, z0          , a / 2);
		DecodeOctTree(in, c, x0 + a / 2, y0          , z0 + a / 2, a / 2);
		DecodeOctTree(in, c, x0 + a / 2, y0          , z0          , a / 2);
		DecodeOctTree(in, c, x0          , y0 + a / 2, z0 + a / 2, a / 2);
		DecodeOctTree(in, c, x0          , y0 + a / 2, z0          , a / 2);
		DecodeOctTree(in, c, x0          , y0          , z0 + a / 2, a / 2);
		DecodeOctTree(in, c, x0          , y0          , z0          , a / 2);
		//�ݹ����
	}
	else
		for (int dx = 0; dx < a; dx++)
			for (int dy = 0; dy < a; dy++)
				for (int dz = 0; dz < a; dz++)
					c[x0 + dx][y0 + dy][z0 + dz] = u;
	//����һ��������u
	return;
}


// �����������ڼ��ص�����
void Map::Save()
{
	for (int i = 0; i < a; i++)
		for (int j = 0; j < a; j++)
			SaveChunk(vOrigin[0] + j * 16, 0, vOrigin[2] + i * 16, Data[i*a + j]);
}
