#include "renderer.hpp"
#include <d3d9.h>
#include <d3dx9.h>
#include <ctime>
#include <base/rect.hpp>

namespace drawing
{
	renderer_t::renderer_t() noexcept:
		dpp_(),
		d3d_(nullptr),
		device_(nullptr),
		font_(nullptr),
		sprite_(nullptr),
		fps_data_(),
		frame_rate_(0)
	{
	}

	renderer_t::~renderer_t()
	{
		sprite_->Release();
		font_->Release();
		device_->Release();
		d3d_->Release();
	}

	bool renderer_t::create(HWND owner)
	{
		base::rect_t rc;
		if (!::GetWindowRect(owner, &rc))
			return false;

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
			return false;

		if (!create_device_objects())
			return false;

		return true;
	}

	void renderer_t::draw_text(const std::string& text, const base::point_t& point) const
	{
		base::rect_t rc(point);
		// TODO: Add different type of flags
		int format(DT_NOCLIP);
		//format |= DT_CENTER;

		font_->DrawText(sprite_, text.data(), -1, &rc, format, 0xFFFFFFFF);
	}

	void renderer_t::begin_rendering() const
	{
		device_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		device_->BeginScene();
		sprite_->Begin(D3DXSPRITE_ALPHABLEND);
	}

	void renderer_t::end_rendering()
	{
		++fps_data_.frames;
		float tick_count(clock() * 0.001f);
		if (tick_count - fps_data_.tick_count >= 1.0f)
		{
			fps_data_.tick_count = tick_count;
			fps_data_.fps = fps_data_.frames;
			fps_data_.frames = 0;
		}

		sprite_->End();
		device_->EndScene();
		device_->Present(nullptr, nullptr, nullptr, nullptr);
	}

	int renderer_t::get_frame_rate() const
	{
		return fps_data_.fps;
	}

	bool renderer_t::reset_device()
	{
		// In case we get WM_SIZE but the renderer isn't even created yet
		if (!device_)
			return true;

		base::rect_t rc;
		if (!::GetWindowRect(dpp_.hDeviceWindow, &rc))
			return false;

		destroy_device_objects();

		dpp_.BackBufferWidth = rc.width();
		dpp_.BackBufferHeight = rc.height();

		HRESULT hr(device_->Reset(&dpp_));
		if (FAILED(hr))
			return false;

		if (!create_device_objects())
			return false;

		return true;
	}

	bool renderer_t::create_device_objects()
	{
		if (FAILED(::D3DXCreateFont(device_, 13, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &font_)))
			return false;

		if (FAILED(::D3DXCreateSprite(device_, &sprite_)))
			return false;

		return true;
	}

	void renderer_t::destroy_device_objects()
	{
		if (font_)
			font_->Release();
		font_ = nullptr;

		if (sprite_)
			sprite_->Release();
		sprite_ = nullptr;
	}
} // namespace drawing