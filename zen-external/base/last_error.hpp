#pragma once
#include <string>

typedef long HRESULT;

namespace base
{
	class hresult_t
	{
	public:
		hresult_t() = default;
		hresult_t(HRESULT result);
		~hresult_t();

		operator HRESULT() const;
	private:
		HRESULT result_;
	};

	extern std::string get_error_message(int error);
	extern int get_last_error();
	extern HRESULT get_last_hresult();
	extern void set_last_hresult(HRESULT result);
} // namespace base
