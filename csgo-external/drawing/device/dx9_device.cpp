#include "dx9_device.hpp"
#include <d3d9.h>
#include <d3dx9.h>
#include <base/rect.hpp>

namespace drawing
{
	dx9_device_t::dx9_device_t(HWND owner):
		dpp_(),
		d3d_(nullptr),
		device_(nullptr),
		font_(nullptr),
		sprite_(nullptr)
	{
		base::rect_t rc;
		::GetWindowRect(owner, &rc);

		d3d_ = ::Direct3DCreate9(D3D_SDK_VERSION);
		::ZeroMemory(&dpp_, sizeof dpp_);

		dpp_.Windowed = 1;
		dpp_.SwapEffect = D3DSWAPEFFECT_DISCARD;
		dpp_.hDeviceWindow = owner;
		dpp_.BackBufferFormat = D3DFMT_A8R8G8B8;
		dpp_.BackBufferWidth = rc.width();
		dpp_.BackBufferHeight = rc.height();
		dpp_.EnableAutoDepthStencil = 1;
		dpp_.AutoDepthStencilFormat = D3DFMT_D16;
		dpp_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		if (FAILED(d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dpp_.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dpp_, &device_)))
			throw std::runtime_error("Could not create device.");

		create_device_objects();
	}

	dx9_device_t::~dx9_device_t()
	{
		dx9_device_t::release();
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

		if (FAILED(device_->Reset(&dpp_)))
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
		if (FAILED(::D3DXCreateFont(device_, 13, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &font_)))
			return false;

		if (FAILED(::D3DXCreateSprite(device_, &sprite_)))
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