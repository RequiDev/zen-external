#pragma once
#include "device.hpp"
#include <windows.h>
#include <d3d9types.h>
#include <string>

struct IDirect3D9;
struct IDirect3DDevice9;
struct ID3DXFont;
struct ID3DXSprite;

namespace drawing
{
	class dx9_device_t : public device_t
	{
	public:
		explicit dx9_device_t(HWND owner);
		virtual ~dx9_device_t();

		virtual void release();
		virtual void begin_scene();
		virtual void end_scene();
		virtual bool reset();

		virtual void draw_text(const std::string& text, const base::point_t& pt);
	private:
		bool create_device_objects();
		void destroy_device_objects();

		D3DPRESENT_PARAMETERS dpp_;
		IDirect3D9* d3d_;
		IDirect3DDevice9* device_;
		ID3DXFont* font_;
		ID3DXSprite* sprite_;
	};
} // namespace drawing