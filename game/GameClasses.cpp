#include "GameClasses.h"

Camera::Camera()
{
}


Camera::~Camera()
{
}

Character::Character()
{
}


Character::~Character()
{
}

void GUI::Render()
{

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE);
	//glDisable(GL_FOG);

	extern Game* pGame;
	int wnd_height = pWnd->GetHeight();
	int wnd_width = pWnd->GetWidth();
	int selected_item = pGame->selected_item;
	HTEX(*texture)[3] = pGame->cube_tex;

	glBegin(GL_LINES);//��ʮ��׼��
	glVertex2f(wnd_width / 2 - 100, wnd_height / 2);
	glVertex2f(wnd_width / 2 + 100, wnd_height / 2);
	glVertex2f(wnd_width / 2, wnd_height / 2 - 100);
	glVertex2f(wnd_width / 2, wnd_height / 2 + 100);
	glEnd();

	const int ITEM_BOX_WIDTH = 80;

	glBegin(GL_QUADS);//����Ʒ��
	glColor3f(0.7f, 0.7f, 0.7f);

	glVertex2f(wnd_width / 2 - 5 * ITEM_BOX_WIDTH, wnd_height);
	glVertex2f(wnd_width / 2 + 5 * ITEM_BOX_WIDTH, wnd_height);
	glVertex2f(wnd_width / 2 + 5 * ITEM_BOX_WIDTH, wnd_height - ITEM_BOX_WIDTH);
	glVertex2f(wnd_width / 2 - 5 * ITEM_BOX_WIDTH, wnd_height - ITEM_BOX_WIDTH);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(wnd_width / 2 + (-6 + selected_item) * ITEM_BOX_WIDTH, wnd_height);
	glVertex2f(wnd_width / 2 + (-6 + selected_item + 1) * ITEM_BOX_WIDTH, wnd_height);
	glVertex2f(wnd_width / 2 + (-6 + selected_item + 1) * ITEM_BOX_WIDTH, wnd_height - ITEM_BOX_WIDTH);
	glVertex2f(wnd_width / 2 + (-6 + selected_item) * ITEM_BOX_WIDTH, wnd_height - ITEM_BOX_WIDTH);
	glEnd();


	glEnable(GL_TEXTURE_2D);
	for (int i = 1; i <= 7; i++)
	{
		HTEX cur_tex = texture[i][1] ? texture[i][1] : texture[i][0];
		glBindTexture(GL_TEXTURE_2D, cur_tex);	//ѡ������

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(wnd_width / 2 + (-6 + i) * ITEM_BOX_WIDTH + 16, wnd_height - 16);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(wnd_width / 2 + (-6 + i + 1) * ITEM_BOX_WIDTH - 16, wnd_height - 16);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(wnd_width / 2 + (-6 + i + 1) * ITEM_BOX_WIDTH - 16, wnd_height - ITEM_BOX_WIDTH + 16);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(wnd_width / 2 + (-6 + i) * ITEM_BOX_WIDTH + 16, wnd_height - ITEM_BOX_WIDTH + 16);
		glEnd();
		//-16��+16��Ϊ���÷���ȸ�������ȥһȦ
	}

	//���fps ��ʱ��ûʵ��
	//printer::glPrint("fps: %f | gfps: %f", fps, gfps);		// ������ֵ���Ļ

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE);
	return;
}