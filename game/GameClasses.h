#pragma once
#ifndef _GAME_CLASSES_H

#include"..\stdafx.h"
#include"..\WidgetClasses.h"

#include"GameMath.h"
#include"Map.h"

struct Camera
{
public:
	vec r;//位矢
	float theta{ 0.0f }, phi{ 0.0f };
	//theta 向左逆时针旋转的度数
	//phi “抬头”的度数，-90-90
public:
	Camera();
	~Camera();

	void ApplyTransform()
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-phi, 1.0f, 0.0f, 0.0f);//注意符号
		glRotatef(-theta, 0.0f, 1.0f, 0.0f);	//glrotate以逆时针旋转为正
		glTranslatef(-r.x, -r.y, -r.z);
	}
	void ApplyInvTransform()//逆变换
	{
		glLoadIdentity();
		glRotatef(phi, 1.0f, 0.0f, 0.0f);//注意符号
		glRotatef(theta, 0.0f, 1.0f, 0.0f);	//glrotate以逆时针旋转为正
		glTranslatef(r.x, r.y, r.z);
	}
	vec ApplyTransform(vec v)//rotate only
	{
		vec v1, v2;

		//模拟矩阵乘法
		v1[1] = v[1];
		v1[2] = cos(rad(-theta))*v[2]
			- sin(rad(-theta))*v[0];
		v1[0] = sin(rad(-theta))*v[2]
			+ cos(rad(-theta))*v[0];

		v2[0] = v1[0];
		v2[1] = cos(rad(-phi))*v1[1]
			- sin(rad(-phi))*v1[2];
		v2[2] = sin(-rad(phi))*v1[1]
			+ cos(rad(-phi))*v1[2];

		return v2;
	}
	vec ApplyInvTransform(vec v)
	{
		throw runtime_error{ "func not implemented" };
	}
	void Rotate(float dtheta, float dphi)
	{
		theta += dtheta;
		phi += dphi;
		if (phi > 90.0f) phi = 90.0f;
		if (phi < -90.0f)	phi = -90.0f;
	}
	void Move(float dx,float dy,float dz)
	{
		r.x += dx; r.y += dy; r.z += dz;
	}
	void Move(vec dr) { r = r + dr; }
	//在本征坐标系中的移动
	void MoveForward(float d)
	{
		r.x -= d*sin(rad(theta));
		r.z -= d*cos(rad(theta));
	}
	void MoveRight(float d)
	{
		//出于某种未知的原因，这里没法用宏，不然就会算错，即使宏中加了括号
		r.x -= d*sin(rad(theta - 90.0f));
		r.z -= d*cos(rad(theta - 90.0f));
	}
	void MoveUp(float d)
	{
		r.y += d;
	}

};

class Character
{
private:
	vec r;//位矢
	float theta{ 0.0f }, phi{ 0.0f };
	//theta 向左逆时针旋转的度数
	//phi “抬头”的度数，-90-90
	
public:
	Camera camera;

	Character();
	~Character();

	const vec& pos{ r };
	void LookFrom() { camera.ApplyTransform(); }
	void Rotate(float dtheta, float dphi)
	{
		theta += dtheta;
		phi += dphi;
		if (phi > 90.0f) phi = 90.0f;
		if (phi < -90.0f)	phi = -90.0f;
		camera.Rotate(dtheta, dphi);
	}
	void Move(float dx, float dy, float dz)
	{
		r.x += dx; r.y += dy; r.z += dz;
		camera.Move(vec(dx, dy, dz));
	}
	void Move(vec dr) { r = r + dr; camera.Move(dr); }
	//在本征坐标系中的移动
	void MoveForward(float d)
	{
		r.x -= d*sin(rad(theta));
		r.z -= d*cos(rad(theta));
		camera.MoveForward(d);
	}
	void MoveRight(float d)
	{
		r.x -= d*sin(rad(theta - 90.0f));
		r.z -= d*cos(rad(theta - 90.0f));
		camera.MoveRight(d);
	}
	void MoveUp(float d)
	{
		r.y += d;
		camera.MoveUp(d);
	}
	//一些矢量代数的Get函数
	vec GetPos()
	{
		return r;
	}
	vec GetForward()
	{
		vec v;
		v.x = -sin(rad(theta));
		v.z = -cos(rad(theta));
		v.x *= cos(rad(phi));
		v.z *= cos(rad(phi));
		v.y = sin(rad(phi));
		//三角函数的基本运用，画画图就知道了
		//大一补记：其实就是球坐标
		return v;
	}
};



class GUI :public GContainer
{
public:
	virtual ~GUI() {}
	virtual void Render();
};
class Game:public GContainer
{
private:
	static void _JudgeUnique()
	{
		static bool first_flag{ true };
		if (!first_flag)
			throw runtime_error{ "only one Game object should be created!" };
		else
			first_flag = true;
	}
	float LightAmbient[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; 				// 环境光参数
	float LightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };					// 漫射光参数
	float LightPosition[4] = { 1.0f, 2.0f, -3.0f, 0.0f };				// 光源位置
public:
	HTEX cube_tex[10][3];
	unsigned int selected_item{ 1 };

	Character player;
	Map map;
	GUI gui;

	bool cur_free{ false };
public:
	Game()
	{
		_JudgeUnique();
		//加载纹理
		memset(cube_tex, 0, sizeof(HTEX) * 30);
		if (!(cube_tex[1][0] = gltLoadTexture("texture\\dirt.bmp")))							// 调用纹理载入子例程
			throw runtime_error("Init Error: Fail to load texture");						// 如果未能载入，返回FALSE
		if (!(cube_tex[2][0] = gltLoadTexture("texture\\grass_top.bmp")))
			throw runtime_error("Init Error: Fail to load texture");
		if (!(cube_tex[2][1] = gltLoadTexture("texture\\grass_side.bmp")))
			throw runtime_error("Init Error: Fail to load texture");
		cube_tex[2][2] = cube_tex[1][0];//草方块底部和泥土一样
		if (!(cube_tex[3][0] = gltLoadTexture("texture\\stone.bmp")))
			throw runtime_error("Init Error: Fail to load texture");
		if (!(cube_tex[4][0] = gltLoadTexture("texture\\bedrock.bmp")))
			throw runtime_error("Init Error: Fail to load texture");
		if (!(cube_tex[5][0] = gltLoadTexture("texture\\planks_spruce.bmp")))
			throw runtime_error("Init Error: Fail to load texture");
		if (!(cube_tex[6][0] = gltLoadTexture("texture\\log_spruce_top.bmp")))
			throw runtime_error("Init Error: Fail to load texture");
		if (!(cube_tex[6][1] = gltLoadTexture("texture\\log_spruce_side.bmp")))
			throw runtime_error("Init Error: Fail to load texture");
		/*if (!(cube_tex[7][0] = gltLoadTexture("texture\\leaves_spruce.png")))
			throw runtime_error("Init Error: Fail to load texture");*///正在测试的透明效果，然而并不成功
		if (!(cube_tex[7][0] = gltLoadTexture("texture\\leaves_spruce.bmp")))
			throw runtime_error("Init Error: Fail to load texture");


		float LightScale[] = { 0.6f,0.6f,0.6f,1.0f };
		glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);				// 设置环境光
		glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);				// 设置漫射光
		glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);			// 设置光源位置
		glEnable(GL_LIGHT1);							// 启用一号光源
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, LightScale);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, LightScale);

		pWnd->ShowCursor(false);
		pWnd->SetCursorPos(200, 200);
		player.MoveUp(3.0f);
	}
	Game(const Game&) = delete;

	virtual void Update()
	{
		//获得控制/输入信息
		bool* keys = pWnd->GetConsole().keys;
		Mouse& mouse = pWnd->GetConsole().mouse;

		//人物移动
		float dx, dy, dz;
		//这里的x,z是指MC，不是WC(MC和WC的y方向一致)
		dx = (keys['D'] ? 0.025f : 0.0f) - (keys['A'] ? 0.025f : 0.0f);
		dy = (keys[VK_SPACE] ? 0.025f : 0.0f) - (keys[VK_CAPITAL] ? 0.025f : 0.0f);
		dz = (keys['W'] ? 0.025f : 0.0f) - (keys['S'] ? 0.025f : 0.0f);
		player.MoveRight(dx);
		player.MoveForward(dz);
		player.MoveUp(dy);

		//切换模式
		if (keys[VK_ESCAPE])
		{
			cur_free = !cur_free;
			pWnd->ShowCursor(cur_free);
			keys[VK_ESCAPE] = false;
		}
		
		//鼠标旋转视角和放置破坏方块
		if (!cur_free)
		{
			//视角旋转
			player.Rotate((200.0f - mouse.x)*0.03f, (200.0f - mouse.y)*0.03);
			//注意屏幕像素坐标和游戏坐标的y轴方向是反的
			//Character中，向左转theta为正，向上转phi为正
			pWnd->SetCursorPos(200, 200);

			//放置/破坏方块
			if (mouse.LButton)
			{
				vec vFwd = player.GetForward();
				vec vPos = player.GetPos();
				vec vTgt;
				for (float d = 0.0f; d < 5.0f; d += 0.05f)
				{
					vTgt = vPos + d*vFwd;
					if (map(vTgt.x, vTgt.y, vTgt.z))
					{
						map.at(vTgt) = 0;
						mouse.LButton = false;
						break;
					}
				}
			}
			if (mouse.RButton)
			{
				vec vFwd = player.GetForward();
				vec vPos = player.GetPos();
				vec vTgt;
				for (float d = 0.0f; d < 5.0f; d += 0.05f)
				{
					vTgt = vPos + d*vFwd;
					if (map(vTgt.x, vTgt.y, vTgt.z))
					{
						vTgt -= 0.05f*vFwd;
						map(vTgt.x, vTgt.y, vTgt.z) = selected_item;
						mouse.RButton = false;
						break;
					}
				}

			}
		}

		//修正光源位置
		//由于光源位置不受模型观察矩阵作用 故需要人为作用以修正
		vec v1;
		for (int i = 0; i < 3; i++)
			v1[i] = LightPosition[i];
		vec v2 = player.camera.ApplyTransform(v1);
		float LightPositionNew[4];
		for (int i = 0; i < 3; i++)
			LightPositionNew[i] = v2[i];
		LightPositionNew[3] = 0.0f;
		gltSetProjectionMode(GLT_3D, pWnd->GetWidth(), pWnd->GetHeight());
		glLightfv(GL_LIGHT1, GL_POSITION, LightPositionNew);

		//切换选中的方块类型
		for (int i = 1; i <= 10; i++)
		{
			if (!i) i = 10;
			if (keys[i + '0'])
				selected_item = i;
		}
		
	}
	virtual void Render()
	{
		gltBeginRender();

		gltSetProjectionMode(GLT_3D, pWnd->GetWidth(), pWnd->GetHeight());
		player.LookFrom();
		map.RenderAround(player.pos);

		gltSetProjectionMode(GLT_2D, pWnd->GetWidth(), pWnd->GetHeight());
		gui.Render();
	
		gltEndRender();
	}
	virtual string Run()
	{
		while (!(pWnd->Quit()))//经包装，一种简洁的消息循环
		{
			pWnd->Update();
			Update();
			Render();
		}
		return "quit";
	}
};
extern Game* pGame;//全局唯一的Game对象的指针，实际定义在myworld.cpp


#endif // !_GAME_CLASSES_H