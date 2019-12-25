#include <Siv3D.hpp> // OpenSiv3D v0.4.2
#include "BrowserRect.hpp"

class Text
{
public:
	Text(const String& str, const Vec2& pos):
		str(str), pos(pos)
	{}
	String str;
	Vec2 pos;
};

class Listener : public ultralight::LoadListener
{
public:
	void DropText(const ultralight::JSObject& thisObject, const ultralight::JSArgs& args)
	{
		char* str = static_cast<ultralight::String>(args[0].ToString()).utf8().data();
		texts.emplace_back(Unicode::FromUTF8(str), RandomVec2({ 0,Scene::Width() }, 0));
	}

	void OnDOMReady(ultralight::View* caller) override
	{
		ultralight::SetJSContext(caller->js_context());
		ultralight::JSObject global = ultralight::JSGlobalObject();

		using ultralight::JSCallback;
		global["DropText"] = BindJSCallback(&Listener::DropText);
	}

	Array<Text> texts;
};

void Main()
{
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));
	Font font(50, Typeface::Heavy);

	BrowserRect browser(Scene::Rect().stretched(-100));

	Listener listener;
	browser.view()->set_load_listener(&listener);

	browser.view()->LoadHTML(R"(
<html>
<style type="text/css">
body { font-family: sans-serif; text-align: center; }
</style>
<link rel="stylesheet" href="https://www.w3schools.com/w3css/4/w3.css">
<body>
<form class="w3-container w3-card-4" action="/action_page.php">
  <h2 class="w3-text-blue">OpenSiv3D Web Sample</h2>
  <p>input some characters</p>
  <p><input id="textBox" class="w3-input w3-border" name="text1" type="text" value=""></p>
  <p><button class="w3-btn w3-blue w3-xxxlarge" onclick="DropText(document.getElementById('textBox').value);">Go!</button></p>
</form>
</body>
</html>
)");

	while (System::Update())
	{
		browser.update();
		browser.draw();

		for (auto& text : listener.texts)
		{
			text.pos += Vec2(0, 0.5);
			font(text.str).draw(text.pos, RandomHSV({ 0,360 }, { 1,1 }, { 1,1 }));
		}
	}
}
