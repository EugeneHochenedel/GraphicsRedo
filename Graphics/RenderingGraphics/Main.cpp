#include "RenderingGraphicsApp.h"

int main()
{
	Application* theApp = new Renderer();
	if (theApp->startup() == true)
	{
		while (theApp->update() == true)
		{
			theApp->draw();
		}
		theApp->shutdown();
	}

	delete theApp;
	system("pause");
	return 0;
}

