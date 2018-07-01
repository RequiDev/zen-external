#pragma once
#include <windows.h>
#include <string>
#include <d3d9.h>

namespace base
{
	class point_t;
} // namespace base

struct ID3DXFont;
struct ID3DXSprite;

namespace drawing
{
	class renderer_t
	{
		struct fps_data_t
		{
			int fps;
			int last_fps;
			float last_tick_count;
			float tick_count;
		};
	public:
		renderer_t() noexcept;
		~renderer_t();

		bool create(HWND owner);
		void draw_text(const std::string& text, const base::point_t& point) const;
		void begin_rendering() const;
		void end_rendering();

		int get_frame_rate() const;
		bool reset_device();
	private:
		bool create_device_objects();
		void destroy_device_objects();

		D3DPRESENT_PARAMETERS dpp_;
		IDirect3D9* d3d_;
		IDirect3DDevice9* device_;
		ID3DXFont* font_;
		ID3DXSprite* sprite_;
		fps_data_t fps_data_;
		int frame_rate_;
	};
} // namespace drawing