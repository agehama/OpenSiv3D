#include "WindowMac.h"
#include <AppCore/Monitor.h>
#include "AppMac.h"
#import "Cocoa/Cocoa.h"
#import "ViewController.h"

namespace ultralight {

WindowMac::WindowMac(Monitor* monitor, uint32_t width, uint32_t height,
  bool fullscreen, unsigned int window_flags) : monitor_(monitor), is_fullscreen_(fullscreen) {
  NSRect frame = NSMakeRect(0, 0, width, height);
  NSWindowStyleMask style = 0;
  if (window_flags & kWindowFlags_Borderless)
    style |= NSWindowStyleMaskBorderless;
  else
    style |= NSWindowStyleMaskClosable;
  if (window_flags & kWindowFlags_Titled)
    style |= NSWindowStyleMaskTitled;
  if (window_flags & kWindowFlags_Resizable)
    style |= NSWindowStyleMaskResizable;
  if (window_flags & kWindowFlags_Maximizable)
    style |= NSWindowStyleMaskMiniaturizable;

  window_  = [[NSWindow alloc] initWithContentRect:frame
                                         styleMask:style
                                             backing:NSBackingStoreBuffered
                                               defer:NO];
  [window_ center];
  [window_ setBackgroundColor:[NSColor whiteColor]];
  [window_ makeKeyAndOrderFront:NSApp];
  [window_ setOrderedIndex:0];
  [NSApp activateIgnoringOtherApps:YES];
  
  controller_ = [[ViewController alloc] init];
  [controller_ initWithWindow:this frame:frame];
  [window_ setContentViewController:controller_];
    
  delegate_ = [[WindowDelegate alloc] init];
  [delegate_ initWithWindow:this];
  [window_ setDelegate:delegate_];
    
  // Move app to foreground
  ProcessSerialNumber psn = {0, kCurrentProcess};
  TransformProcessType(&psn, kProcessTransformToForegroundApplication);
}

WindowMac::~WindowMac() {
}

uint32_t WindowMac::width() const {
  return PixelsToDevice((int)controller_.metalView.metalLayer.drawableSize.width);
}

uint32_t WindowMac::height() const {
  return PixelsToDevice((int)controller_.metalView.metalLayer.drawableSize.height);
}

double WindowMac::scale() const {
  return monitor_->scale();
}

void WindowMac::SetTitle(const char* title) {
  window_.title = [NSString stringWithUTF8String:title];
}

void WindowMac::SetCursor(ultralight::Cursor cursor) {
}

void WindowMac::Close() {
  [window_ close];
}

void WindowMac::OnClose() {
  if (listener_)
    listener_->OnClose();
  if (app_listener_)
    app_listener_->OnClose();
}

void WindowMac::OnResize(uint32_t width, uint32_t height) {
  if (app_listener_)
    app_listener_->OnResize(width, height);
  if (listener_)
    listener_->OnResize(width, height);
}
    
void WindowMac::SetNeedsDisplay() {;
  controller_.metalView.needsDisplay = true;
}
    
CAMetalLayer* WindowMac::layer() {
  return controller_.metalView.metalLayer;
}

Ref<Window> Window::Create(Monitor* monitor, uint32_t width, uint32_t height,
  bool fullscreen, unsigned int window_flags) {
  return AdoptRef(*new WindowMac(monitor, width, height, fullscreen, window_flags));
}

Window::~Window() {}

}  // namespace ultralight

