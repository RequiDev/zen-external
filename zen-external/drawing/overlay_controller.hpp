#pragma once

namespace drawing
{
	class renderer_t;

	class overlay_controller_t
	{
	public:
		virtual ~overlay_controller_t() {}

		virtual void set_renderer(renderer_t* renderer) = 0;
	};
} // namespace drawing
