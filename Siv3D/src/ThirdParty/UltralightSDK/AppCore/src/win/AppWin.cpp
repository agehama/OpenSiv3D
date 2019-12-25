#include "AppWin.h"
#include <Windows.h>
#include "WindowWin.h"
#if defined(DRIVER_D3D11)
#include "d3d11/GPUContextD3D11.h"
#include "d3d11/GPUDriverD3D11.h"
#elif defined(DRIVER_D3D12)
#include "d3d12/GPUContextD3D12.h"
#include "d3d12/GPUDriverD3D12.h"
#endif
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <Shlwapi.h>
#include "WindowsUtil.h"
#include "MonitorWin.h"
#include <Siv3D.hpp>

namespace ultralight {

std::mutex sivMutex;
UINT sivWindowMessage;
WPARAM sivWindowWParam;
LPARAM sivWindowLParam;

AppWin::AppWin(Settings settings, Config config) : settings_(settings) {
  windows_util_.reset(new WindowsUtil());
  windows_util_->EnableDPIAwareness();

  main_monitor_.reset(new MonitorWin(windows_util_.get()));

  config.device_scale_hint = main_monitor_->scale();
  config.face_winding = kFaceWinding_Clockwise;
  Platform::instance().set_config(config);

  font_loader_.reset(new FontLoaderWin());
  Platform::instance().set_font_loader(font_loader_.get());

  HMODULE hModule = GetModuleHandleW(NULL);
  WCHAR path[MAX_PATH];
  GetModuleFileNameW(hModule, path, MAX_PATH);
  PathRemoveFileSpecW(path);

  PathAppendW(path, settings_.file_system_path.utf16().data());

  file_system_.reset(new FileSystemWin(path));
  Platform::instance().set_file_system(file_system_.get());

  renderer_ = Renderer::Create();
}

AppWin::~AppWin() {
  Platform::instance().set_gpu_driver(nullptr);
  gpu_driver_.reset();
  gpu_context_.reset();
}

void AppWin::OnClose() {
}

void AppWin::OnResize(uint32_t width, uint32_t height) {
  if (gpu_context_)
    gpu_context_->Resize((int)width, (int)height);
}

void AppWin::set_window(Ref<Window> window) {
  window_ = window;
#if defined(DRIVER_D3D11)
  gpu_context_.reset(new GPUContextD3D11());
  WindowWin* win = static_cast<WindowWin*>(window_.get());
  if (!gpu_context_->Initialize(win->hwnd(), win->width(),
    win->height(), win->scale(), win->is_fullscreen(), true, true, 1)) {
    MessageBoxW(NULL, (LPCWSTR)L"Failed to initialize D3D11 context", (LPCWSTR)L"Notification", MB_OK);
    exit(-1);
  }

  gpu_driver_.reset(new GPUDriverD3D11(gpu_context_.get()));
#elif defined(DRIVER_D3D12)
  gpu_context_.reset(new GPUContextD3D12());
  WindowWin* win = static_cast<WindowWin*>(window_.get());
  if (!gpu_context_->Initialize(win->hwnd(), win->width(),
	  win->height(), win->scale(), win->is_fullscreen(), true, true, 1)) {
	  MessageBoxW(NULL, (LPCWSTR)L"Failed to initialize D3D12 context", (LPCWSTR)L"Notification", MB_OK);
	  exit(-1);
  }

  gpu_driver_.reset(new GPUDriverD3D12(gpu_context_.get()));
#endif
  Platform::instance().set_gpu_driver(gpu_driver_.get());
  win->set_app_listener(this);
}

Monitor* AppWin::main_monitor() {
  return main_monitor_.get();
}

Ref<Renderer> AppWin::renderer() {
  return *renderer_.get();
}

LRESULT CALLBACK WndProc(WindowData& windowData, UINT message, WPARAM wParam, LPARAM lParam) {
	WindowWin* window = windowData.window;
	HWND hWnd = window->hwnd();

	const auto pos = s3d::Cursor::Pos();

	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		static_cast<AppWin*>(App::instance())->InvalidateWindow();
		static_cast<AppWin*>(App::instance())->OnPaint();
		EndPaint(hWnd, &ps);
		break;
	/*case WM_DESTROY:
		  PostQuitMessage(0);
		  break;*/
	case WM_ENTERSIZEMOVE:
		windowData.is_resizing_modal = true;
		break;
	/*case WM_SIZE:
		{
		  if (WINDOWDATA()) {
		    WINDOW()->OnResize(WINDOW()->width(), WINDOW()->height());
		    // This would normally be called when the message loop is idle
		    // but during resize the window consumes all messages so we need
		    // to force paints during the operation.
		    static_cast<AppWin*>(App::instance())->OnPaint();
		  }
		  break;
		}*/
	case WM_EXITSIZEMOVE:
		windowData.is_resizing_modal = false;
		window->OnResize(window->width(), window->height());
		static_cast<AppWin*>(App::instance())->OnPaint();
		break;
	case WM_KEYDOWN:
		window->FireKeyEvent(KeyEvent(KeyEvent::kType_RawKeyDown, (uintptr_t)wParam, (intptr_t)lParam, false));
		break;
	case WM_KEYUP:
		window->FireKeyEvent(KeyEvent(KeyEvent::kType_KeyUp, (uintptr_t)wParam, (intptr_t)lParam, false));
		break;
	case WM_CHAR:
		window->FireKeyEvent(KeyEvent(KeyEvent::kType_Char, (uintptr_t)wParam, (intptr_t)lParam, false));
		break;
	case WM_MOUSEMOVE:
		window->FireMouseEvent(
			{ MouseEvent::kType_MouseMoved,
			  pos.x,
			  pos.y,
			  windowData.cur_btn });
		break;
	case WM_LBUTTONDOWN:
		SetCapture(window->hwnd());
		windowData.cur_btn = MouseEvent::kButton_Left;
		window->FireMouseEvent(
			{ MouseEvent::kType_MouseDown,
			  pos.x,
			  pos.y,
			  windowData.cur_btn });
		break;
	case WM_MBUTTONDOWN:
		SetCapture(window->hwnd());
		windowData.cur_btn = MouseEvent::kButton_Middle;
		window->FireMouseEvent(
			{ MouseEvent::kType_MouseDown,
			  pos.x,
			  pos.y,
			  windowData.cur_btn });
		break;
	case WM_RBUTTONDOWN:
		SetCapture(window->hwnd());
		windowData.cur_btn = MouseEvent::kButton_Right;
		window->FireMouseEvent(
			{ MouseEvent::kType_MouseDown,
			  pos.x,
			  pos.y,
			  windowData.cur_btn });
		break;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
		window->FireMouseEvent(
			{ MouseEvent::kType_MouseUp,
			  pos.x,
			  pos.y,
			  windowData.cur_btn });
		windowData.cur_btn = MouseEvent::kButton_None;
		break;
	case WM_MOUSEWHEEL:
		window->FireScrollEvent(
			{ ScrollEvent::kType_ScrollByPixel, 0, window->PixelsToDevice(GET_WHEEL_DELTA_WPARAM(wParam)) });
		break;
	default:
		//return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

void AppWin::Run() {
	if (!window_) {
		Logger << U"Forgot to call App::set_window before App::Run";
		return;
	}

	{
		WindowWin* win = static_cast<WindowWin*>(window_.get());
		std::lock_guard lock(sivMutex);
		WndProc(win->windowData(), sivWindowMessage, sivWindowWParam, sivWindowLParam);
		sivWindowMessage = WM_NULL;
	}

	OnPaint();
}

void AppWin::Quit() {
  is_running_ = false;
}

void AppWin::OnPaint() {
  Update();
  if (!gpu_driver_)
    return;

  gpu_driver_->BeginSynchronize();
  renderer_->Render();
  gpu_driver_->EndSynchronize();

  if (gpu_driver_->HasCommandsPending()) {
    gpu_context_->BeginDrawing();
    gpu_driver_->DrawCommandList();
    if (window_)
	    static_cast<WindowWin*>(window_.get())->Draw();
    gpu_context_->PresentFrame();
    gpu_context_->EndDrawing();
    is_first_paint_ = false;
  }
  else if (window_needs_repaint_ && !is_first_paint_) {
    gpu_context_->BeginDrawing();
    if (window_)
      static_cast<WindowWin*>(window_.get())->Draw();
    gpu_context_->PresentFrame();
    gpu_context_->EndDrawing();
  }

  window_needs_repaint_ = false;
}

void AppWin::Update() {
  if (listener_)
    listener_->OnUpdate();
  
  renderer()->Update();
}

static App* g_app_instance = nullptr;

Ref<App> App::Create(Settings settings, Config config) {
  g_app_instance = new AppWin(settings, config);
  return AdoptRef(*g_app_instance);
}

App::~App() {
  g_app_instance = nullptr;
}

App* App::instance() {
  return g_app_instance;
}

}  // namespace ultralight
