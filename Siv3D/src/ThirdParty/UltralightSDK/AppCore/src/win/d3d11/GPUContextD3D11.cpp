#if defined(DRIVER_D3D11)
#include "GPUContextD3D11.h"
#include <cassert>
#include <Siv3DEngine.hpp>
#include <Graphics/IGraphics.hpp>
#include <Siv3D.hpp>

namespace ultralight {

GPUContextD3D11::GPUContextD3D11() {}

GPUContextD3D11::~GPUContextD3D11() {}

// Inherited from GPUContext
void GPUContextD3D11::BeginDrawing() {
  back_buffer_view_ = s3d::Siv3DEngine::Get<s3d::ISiv3DGraphics>()->getRenderTargetView();
}
void GPUContextD3D11::EndDrawing() {}
void GPUContextD3D11::PresentFrame() {}

void GPUContextD3D11::Resize(int width, int height) {
  set_screen_size(width, height);
}

ID3D11Device* GPUContextD3D11::device() { return s3d::Siv3DEngine::Get<s3d::ISiv3DGraphics>()->getDevice(); }
ID3D11DeviceContext* GPUContextD3D11::immediate_context() { return s3d::Siv3DEngine::Get<s3d::ISiv3DGraphics>()->getContext(); }
IDXGISwapChain* GPUContextD3D11::swap_chain() { return s3d::Siv3DEngine::Get<s3d::ISiv3DGraphics>()->getSwapChain(); }
ID3D11RenderTargetView* GPUContextD3D11::render_target_view() { return back_buffer_view_; }

void GPUContextD3D11::EnableBlend() {
  immediate_context()->OMSetBlendState(blend_state_.Get(), NULL, 0xffffffff);
}

void GPUContextD3D11::DisableBlend() {
  immediate_context()->OMSetBlendState(disabled_blend_state_.Get(), NULL, 0xffffffff);
}

void GPUContextD3D11::EnableScissor() {
  immediate_context()->RSSetState(scissored_rasterizer_state_.Get());
}

void GPUContextD3D11::DisableScissor() {
  immediate_context()->RSSetState(rasterizer_state_.Get());
}

// Scale is calculated from monitor DPI, see Application::SetScale
void GPUContextD3D11::set_scale(double scale) { scale_ = scale; }
double GPUContextD3D11::scale() const { return scale_; }

// This is in units, not actual pixels.
void GPUContextD3D11::set_screen_size(uint32_t width, uint32_t height) { width_ = width; height_ = height; }
void GPUContextD3D11::screen_size(uint32_t& width, uint32_t& height) { width = width_; height = height_; }

bool GPUContextD3D11::Initialize(HWND hWnd, int screen_width, int screen_height, double screen_scale, bool fullscreen, bool enable_vsync, bool sRGB, int samples) {
  samples_ = samples;
#if ENABLE_MSAA
  samples_ = 4;
#endif
  enable_vsync_ = enable_vsync;
  set_screen_size(screen_width, screen_height);
  set_scale(screen_scale);

  HRESULT hr = S_OK;

  hwnd_ = hWnd;

  // Get the actual device width/height (may be different than screen size)
  RECT rc;
  ::GetClientRect(hWnd, &rc);
  UINT width = rc.right - rc.left;
  UINT height = rc.bottom - rc.top;

  // Create Enabled Blend State

  D3D11_RENDER_TARGET_BLEND_DESC rt_blend_desc;
  ZeroMemory(&rt_blend_desc, sizeof(rt_blend_desc));
  rt_blend_desc.BlendEnable = true;
  rt_blend_desc.SrcBlend = D3D11_BLEND_ONE;
  rt_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  rt_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
  rt_blend_desc.SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
  rt_blend_desc.DestBlendAlpha = D3D11_BLEND_ONE;
  rt_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
  rt_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  D3D11_BLEND_DESC blend_desc;
  ZeroMemory(&blend_desc, sizeof(blend_desc));
  blend_desc.AlphaToCoverageEnable = false;
  blend_desc.IndependentBlendEnable = false;
  blend_desc.RenderTarget[0] = rt_blend_desc;

  device()->CreateBlendState(&blend_desc, blend_state_.GetAddressOf());

  // Create Disabled Blend State

  ZeroMemory(&rt_blend_desc, sizeof(rt_blend_desc));
  rt_blend_desc.BlendEnable = false;
  rt_blend_desc.SrcBlend = D3D11_BLEND_ONE;
  rt_blend_desc.DestBlend = D3D11_BLEND_ZERO;
  rt_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
  rt_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
  rt_blend_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
  rt_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
  rt_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  ZeroMemory(&blend_desc, sizeof(blend_desc));
  blend_desc.AlphaToCoverageEnable = false;
  blend_desc.IndependentBlendEnable = false;
  blend_desc.RenderTarget[0] = rt_blend_desc;

  device()->CreateBlendState(&blend_desc, disabled_blend_state_.GetAddressOf());

  D3D11_RASTERIZER_DESC rasterizer_desc;
  ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
  rasterizer_desc.FillMode = D3D11_FILL_SOLID;
  rasterizer_desc.CullMode = D3D11_CULL_NONE;
  rasterizer_desc.FrontCounterClockwise = false;
  rasterizer_desc.DepthBias = 0;
  rasterizer_desc.SlopeScaledDepthBias = 0.0f;
  rasterizer_desc.DepthBiasClamp = 0.0f;
  rasterizer_desc.DepthClipEnable = false;
  rasterizer_desc.ScissorEnable = false;
#if ENABLE_MSAA
  rasterizer_desc.MultisampleEnable = true;
  rasterizer_desc.AntialiasedLineEnable = true;
#else
  rasterizer_desc.MultisampleEnable = false;
  rasterizer_desc.AntialiasedLineEnable = false;
#endif

  device()->CreateRasterizerState(&rasterizer_desc, rasterizer_state_.GetAddressOf());

  D3D11_RASTERIZER_DESC scissor_rasterizer_desc;
  ZeroMemory(&scissor_rasterizer_desc, sizeof(scissor_rasterizer_desc));
  scissor_rasterizer_desc.FillMode = D3D11_FILL_SOLID;
  scissor_rasterizer_desc.CullMode = D3D11_CULL_NONE;
  scissor_rasterizer_desc.FrontCounterClockwise = false;
  scissor_rasterizer_desc.DepthBias = 0;
  scissor_rasterizer_desc.SlopeScaledDepthBias = 0.0f;
  scissor_rasterizer_desc.DepthBiasClamp = 0.0f;
  scissor_rasterizer_desc.DepthClipEnable = false;
  scissor_rasterizer_desc.ScissorEnable = true;
#if ENABLE_MSAA
  scissor_rasterizer_desc.MultisampleEnable = true;
  scissor_rasterizer_desc.AntialiasedLineEnable = true;
#else
  scissor_rasterizer_desc.MultisampleEnable = false;
  scissor_rasterizer_desc.AntialiasedLineEnable = false;
#endif

  device()->CreateRasterizerState(&scissor_rasterizer_desc, scissored_rasterizer_state_.GetAddressOf());

  back_buffer_width_ = width;
  back_buffer_height_ = height;

  return true;
}

UINT GPUContextD3D11::back_buffer_width() { return back_buffer_width_; }
UINT GPUContextD3D11::back_buffer_height() { return back_buffer_height_; }

}  // namespace ultralight

#endif
