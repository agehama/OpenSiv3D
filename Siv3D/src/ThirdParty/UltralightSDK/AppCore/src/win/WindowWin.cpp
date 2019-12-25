#include "WindowWin.h"
#include <AppCore/Monitor.h>
#include <Windows.h>
#include <ShellScalingAPI.h>
#include "Windowsx.h"
#include <tchar.h>
#include "AppWin.h"
#include <Siv3D.hpp>

namespace ultralight {

void CenterHwndOnMainMonitor(HWND hwnd)
{
  RECT rect;
  GetWindowRect(hwnd, &rect);
  LPRECT prc = &rect;

  // Get main monitor
  HMONITOR hMonitor = MonitorFromPoint({ 1,1 }, MONITOR_DEFAULTTONEAREST);

  MONITORINFO mi;
  RECT        rc;
  int         w = prc->right - prc->left;
  int         h = prc->bottom - prc->top;

  mi.cbSize = sizeof(mi);
  GetMonitorInfo(hMonitor, &mi);

  rc = mi.rcMonitor;

  prc->left = rc.left + (rc.right - rc.left - w) / 2;
  prc->top = rc.top + (rc.bottom - rc.top - h) / 2;
  prc->right = prc->left + w;
  prc->bottom = prc->top + h;

  SetWindowPos(hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

WindowWin::WindowWin(Monitor* monitor, uint32_t width, uint32_t height,
  bool fullscreen, unsigned int window_flags) : monitor_(monitor), is_fullscreen_(fullscreen) {
  width_ = width;
  height_ = height;
  hwnd_ = static_cast<HWND>(Platform::Windows::Window::GetHWND());

  window_data_.window = this;
  window_data_.cur_btn = ultralight::MouseEvent::kButton_None;
  window_data_.is_resizing_modal = false;

  cursor_hand_ = ::LoadCursor(NULL, IDC_HAND);
  cursor_arrow_ = ::LoadCursor(NULL, IDC_ARROW);
  cursor_ibeam_ = ::LoadCursor(NULL, IDC_IBEAM);
  cur_cursor_ = ultralight::kCursor_Hand;
}

WindowWin::~WindowWin() {
  DestroyCursor(cursor_hand_);
  DestroyCursor(cursor_arrow_);
  DestroyCursor(cursor_ibeam_);
}

uint32_t WindowWin::width() const {
  return width_;
}

uint32_t WindowWin::height() const {
  return height_;
}

double WindowWin::scale() const {
  return monitor_->scale();
}

void WindowWin::SetTitle(const char* title) {
  SetWindowTextA(hwnd_, title);
}

void WindowWin::SetCursor(ultralight::Cursor cursor) {
  switch (cursor) {
  case ultralight::kCursor_Hand: {
    if (cur_cursor_ != ultralight::kCursor_Hand)
      ::SetCursor(cursor_hand_);
    break;
  }
  case ultralight::kCursor_Pointer: {
    if (cur_cursor_ != ultralight::kCursor_Pointer)
      ::SetCursor(cursor_arrow_);
    break;
  }
  case ultralight::kCursor_IBeam: {
    if (cur_cursor_ != ultralight::kCursor_IBeam)
      ::SetCursor(cursor_ibeam_);
    break;
  }
  };

  cur_cursor_ = cursor;
}

void WindowWin::Close() {
  DestroyWindow(hwnd_);
}

void WindowWin::OnClose() {
  if (listener_)
    listener_->OnClose();
  if (app_listener_)
    app_listener_->OnClose();
}

void WindowWin::OnResize(uint32_t width, uint32_t height) {
  if (listener_)
    listener_->OnResize(width, height);
  if (app_listener_)
    app_listener_->OnResize(width, height);
}

Ref<Window> Window::Create(Monitor* monitor, uint32_t width, uint32_t height,
  bool fullscreen, unsigned int window_flags) {
  return AdoptRef(*new WindowWin(monitor, width, height, fullscreen, window_flags));
}

Window::~Window() {}

}  // namespace ultralight
