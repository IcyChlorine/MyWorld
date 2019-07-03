#include"stdafx.h"
#include"MyWorld.h"
#include"WidgetClasses.h"
#include"game\GameClasses.h"

GLWindow* pWnd{ nullptr };
Game* pGame{ nullptr };

void Render()
{}


int main()
{
	GLWindow theWnd{ "MyWorld",1440,900 };
	pWnd = &theWnd;

	gltSetProjectionMode(GLT_2D, pWnd->GetWidth(), pWnd->GetHeight());
	gltShowLoadingPage();

	GWelcomePage welcome_page;
	if (welcome_page.Run() != string{ "quit" })
	{
		gltShowLoadingPage();
		gltSetProjectionMode(GLT_3D, pWnd->GetWidth(), pWnd->GetHeight());

		Game theGame;
		pGame = &theGame;

		theGame.Run();
	}
	return 0;
}



