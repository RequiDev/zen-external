#include "dx9_device.hpp"
#include <d3d9.h>
#include <d3dx9.h>
#include <base/rect.hpp>
#include <base/last_error.hpp>

namespace drawing
{
	dx9_device_t::dx9_device_t():
		dpp_(),
		d3d_(nullptr),
		device_(nullptr),
		font_(nullptr),
		sprite_(nullptr)
	{
	}

	dx9_device_t::~dx9_device_t()
	{
		dx9_device_t::release();
	}

	bool dx9_device_t::create(HWND owner)
	{
		base::rect_t rc;
		if (!::GetWindowRect(owner, &rc))
			return false;

		d3d_ = ::Direct3DCreate9(D3D_SDK_VERSION);
		dpp_ = { 0 };

		dpp_.Windowed = 1;
		dpp_.SwapEffect = D3DSWAPEFFECT_DISCARD;
		dpp_.hDeviceWindow = owner;
		dpp_.BackBufferFormat = D3DFMT_A8R8G8B8;
		dpp_.BackBufferWidth = rc.width();
		dpp_.BackBufferHeight = rc.height();
		dpp_.EnableAutoDepthStencil = 1;
		dpp_.AutoDepthStencilFormat = D3DFMT_D16;
		dpp_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		base::hresult_t hr;
		hr = d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dpp_.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dpp_, &device_);
		if (FAILED(hr))
			return false;

		if (!create_device_objects())
			return false;

		return true;
	}

	void dx9_device_t::release()
	{
		destroy_device_objects();
		if (device_)
			device_->Release();
		if (d3d_)
			d3d_->Release();
	}

	void dx9_device_t::begin_scene()
	{
		device_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		device_->BeginScene();
		sprite_->Begin(D3DXSPRITE_ALPHABLEND);
	}

	void dx9_device_t::end_scene()
	{
		sprite_->End();
		device_->EndScene();
		device_->Present(nullptr, nullptr, nullptr, nullptr);
	}

	bool dx9_device_t::reset()
	{
		base::rect_t rc;
		if (!::GetWindowRect(dpp_.hDeviceWindow, &rc))
			return false;

		destroy_device_objects();

		dpp_.BackBufferWidth = rc.width();
		dpp_.BackBufferHeight = rc.height();

		base::hresult_t hr;
		hr = device_->Reset(&dpp_);
		if (FAILED(hr))
			return false;

		if (!create_device_objects())
			return false;

		return true;
	}

	void dx9_device_t::draw_text(const std::string& text, const base::point_t& pt)
	{
		base::rect_t rc(pt);
		// TODO: Add different type of flags
		int format(DT_NOCLIP);
		//format |= DT_CENTER;

		font_->DrawText(sprite_, text.data(), -1, &rc, format, 0xFFFFFFFF);
	}

	bool dx9_device_t::create_device_objects()
	{
		base::hresult_t hr;
		hr = ::D3DXCreateFont(device_, 13, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &font_);
		if (FAILED(hr))
			return false;

		hr = ::D3DXCreateSprite(device_, &sprite_);
		if (FAILED(hr))
			return false;

		return true;
	}

	void dx9_device_t::destroy_device_objects()
	{
		if (font_)
			font_->Release();
		font_ = nullptr;

		if (sprite_)
			sprite_->Release();
		sprite_ = nullptr;
	}
} // namespace drawing