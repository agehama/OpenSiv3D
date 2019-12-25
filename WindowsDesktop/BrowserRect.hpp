#pragma once
#include <Siv3D.hpp>

#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#include <AppCore/JSHelpers.h>

#pragma comment (lib, "UltralightSDK/Ultralight")
#pragma comment (lib, "UltralightSDK/UltralightCore")
#pragma comment (lib, "UltralightSDK/WebCore")
#pragma comment (lib, "d3dcompiler")

class BrowserRect
{
public:
	BrowserRect(const Rect& rect) :
		rect(rect),
		renderTexture(rect.size),
		gammaShader(U"gamma.hlsl", { { U"PSConstants2D", 0 } })
	{
		app = ultralight::App::Create();
		window = ultralight::Window::Create(app->main_monitor(), rect.w, rect.h, false, ultralight::kWindowFlags_Titled);
		app->set_window(*window.get());
		overlay = ultralight::Overlay::Create(*window.get(), window->width(), window->height(), 0, 0);
	}

	~BrowserRect()
	{
		window = nullptr;
		app = nullptr;
		overlay = nullptr;
	}

	ultralight::Ref<ultralight::View> view()
	{
		return overlay->view();
	}

	void update()
	{
		auto originalBlendState = Graphics2D::GetBlendState();
		auto originalRasterizerState = Graphics2D::GetRasterizerState();
		auto originalSamplerState = Graphics2D::GetSamplerState();
		auto originalViewport = Graphics2D::GetViewport();
		{
			//マウス位置を rect.pos 基準にずらす
			Transformer2D t(Mat3x2::Translate(rect.pos), true);
			ScopedRenderTarget2D renderTarget(renderTexture);
			Graphics2D::Flush();

			app->Run();
		}
		Graphics2D::Internal::SetBlendState(originalBlendState);
		Graphics2D::Internal::SetRasterizerState(originalRasterizerState);
		Graphics2D::Internal::SetSamplerState(originalSamplerState);
		Graphics2D::Internal::SetViewport(originalViewport);
	}

	void draw()const
	{
		ScopedCustomShader2D shader(gammaShader);
		renderTexture.draw(rect.pos);
	}

	void drawRounded(double radius)const
	{
		ScopedCustomShader2D shader(gammaShader);
		rect.rounded(radius)(renderTexture).draw();
	}

	const Rect& scope()const
	{
		return rect;
	}

private:
	Rect rect;
	ultralight::RefPtr<ultralight::App> app;
	ultralight::RefPtr<ultralight::Window> window;
	ultralight::RefPtr<ultralight::Overlay> overlay;
	RenderTexture renderTexture;
	PixelShader gammaShader;
};
