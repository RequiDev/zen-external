#pragma once
#include "device.hpp"
#include <windef.h>
#include <d3d9types.h>
#include <string>

struct IDirect3D9;
struct IDirect3DDevice9;
struct ID3DXFont;
struct ID3DXSprite;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;

namespace drawing
{
	class dx9_device_t:
		public device_t
	{
		struct custom_vertex_t
		{
			float pos[3];
			D3DCOLOR col;
			float uv[2];
		};
		#define D3DFVF_CUSTOMVERTEX = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	public:
		explicit dx9_device_t();
		virtual ~dx9_device_t();

		virtual bool create(HWND owner);

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
		IDirect3DVertexBuffer9* vertex_buffer_;
		IDirect3DIndexBuffer9* index_buffer_;
		ID3DXFont* font_;
		ID3DXSprite* sprite_;
	};
} // namespace drawing