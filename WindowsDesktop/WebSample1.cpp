#include <Siv3D.hpp> // OpenSiv3D v0.4.2
#include "BrowserRect.hpp"

void Main()
{
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	BrowserRect browser(Scene::Rect().stretched(-50));
	
	browser.view()->LoadURL("https://qiita.com/advent-calendar/2019/siv3d");

	while (System::Update())
	{
		browser.update();
		browser.drawRounded(10.0);
	}
}
