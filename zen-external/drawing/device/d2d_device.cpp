#pragma warning(disable:4005)

#include "d2d_device.hpp"
#include <base/rect.hpp>
#include <base/point.hpp>
#include <base/last_error.hpp>
#include <dwrite.h>
#include <d2d1.h>
#include <locale>
#include <codecvt>

namespace drawing
{
	d2d_device_t::d2d_device_t():
		owner_(nullptr),
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
		owner_ = owner;
		base::rect_t rect;
		if (!::GetWindowRect(owner_, &rect))
			return false;

		base::hresult_t hr;
		hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr, reinterpret_cast<void**>(&factory_));
		if (FAILED(hr))
			return false;

		if (FAILED(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&write_factory_))))
			return false;

		const D2D1_PIXEL_FORMAT pixel_format(D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
		const D2D1_RENDER_TARGET_PROPERTIES render_target_properties(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE, pixel_format));
		const D2D1_SIZE_U size(D2D1::SizeU(rect.width(), rect.height()));
		const D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_render_target_properties(D2D1::HwndRenderTargetProperties(owner_, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY));

		hr = factory_->CreateHwndRenderTarget(render_target_properties, hwnd_render_target_properties, &render_target_);
		if (FAILED(hr))
			return false;

		create_device_objects();

		return true;
	}

	void d2d_device_t::release()
	{
		if (write_factory_)
			write_factory_->Release();
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
		base::rect_t rect;
		if (!::GetWindowRect(owner_, &rect))
			return false;

		base::hresult_t hr;
		hr = render_target_->Resize(D2D1::SizeU(rect.width(), rect.height()));

		if (FAILED(hr))
			return false;

		return true;
	}

	void d2d_device_t::draw_text(const std::string& text, const base::point_t& pt)
	{
		const D2D_RECT_F font_rect(D2D1::RectF(float(pt.x), float(pt.y), pt.x + float(text.length()) * (13.f * 0.5f), 13.f));
		std::wstring wstr(text.begin(), text.end());

		render_target_->DrawText(wstr.data(), uint32_t(wstr.length()), font_, &font_rect, color_brush_);
	}

	bool d2d_device_t::create_device_objects()
	{
		base::hresult_t hr;

		hr = render_target_->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFFFF), &color_brush_);
		if (FAILED(hr))
			return false;

		hr = write_factory_->CreateTextFormat(L"Tahoma", nullptr, DWRITE_FONT_WEIGHT(13), DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 13, L"de-de", &font_);
		if (FAILED(hr))
			return false;

		return true;
	}

	void d2d_device_t::destroy_device_objects()
	{
		// FIXME: Crashing? nullptr on IUnknown
		if (color_brush_)
			color_brush_->Release();
		if (font_)
			font_->Release();
	}
} // namespace drawing