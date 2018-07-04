#include "d2d_device.hpp"
#include <base/rect.hpp>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <base/point.hpp>

namespace drawing
{
	d2d_device_t::d2d_device_t():
		factory_(nullptr),
		render_target_(nullptr),
		write_factory_(nullptr),
		font_(nullptr),
		color_brush_(nullptr)
	{
	}

	d2d_device_t::~d2d_device_t()
	{
		d2d_device_t::release();
	}

	bool d2d_device_t::create(HWND owner)
	{
		base::rect_t rc;
		if (!::GetWindowRect(owner, &rc))
			return false;

		if (FAILED(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr, reinterpret_cast<void**>(&factory_))))
			return false;

		{
			const D2D1_PIXEL_FORMAT pixel_format(D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
			const D2D1_RENDER_TARGET_PROPERTIES render_target_properties(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE, pixel_format));
			const D2D1_SIZE_U size(D2D1::SizeU(rc.width(), rc.height()));
			const D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_render_target_properties(D2D1::HwndRenderTargetProperties(owner, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY));

			if (FAILED(factory_->CreateHwndRenderTarget(render_target_properties, hwnd_render_target_properties, &render_target_))) // wtf d2d?????
				return false;
		}

		if (FAILED(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&write_factory_))))
			return false;

		HRESULT hr = write_factory_->CreateTextFormat(L"Tahoma", nullptr, DWRITE_FONT_WEIGHT(13), DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 13, L"de-de", &font_);
		if (FAILED(hr))
		{
			char buffer[MAX_PATH];
			sprintf_s(buffer, MAX_PATH, "0x%X", hr);
			MessageBox(owner, buffer, "", 0);
			return false;
		}

		if (FAILED(render_target_->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFFFF), &color_brush_)))
			return false;

		return true;
	}

	void d2d_device_t::release()
	{
		if (color_brush_)
			color_brush_->Release();
		if (write_factory_)
			write_factory_->Release();
		if (render_target_)
			render_target_->Release();
		if (factory_)
			factory_->Release();
	}

	void d2d_device_t::begin_scene()
	{
		render_target_->BeginDraw();
		render_target_->SetTransform(D2D1::Matrix3x2F::Identity());
		render_target_->Clear();
	}

	void d2d_device_t::end_scene()
	{
		render_target_->EndDraw();
	}

	bool d2d_device_t::reset()
	{
		return true;
	}

	void d2d_device_t::draw_text(const std::string& text, const base::point_t& pt)
	{
		const D2D_RECT_F font_rect = D2D1::RectF(pt.x, pt.y, pt.x + float(text.length()) * float(13 / 2), 13);
		std::wstring wstr(text.begin(), text.end());

		render_target_->DrawText(wstr.data(), uint32_t(wstr.length()), font_, &font_rect, color_brush_);
	}
} // namespace drawing