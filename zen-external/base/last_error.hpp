#pragma once
#include <string>
#include <base/no_copy.hpp>

typedef long HRESULT;
typedef long NTSTATUS;

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

	class ntstatus_t
	{
	public:
		ntstatus_t() = default;
		ntstatus_t(NTSTATUS status);
		~ntstatus_t();

		operator NTSTATUS() const;
	private:
		NTSTATUS status_;
	};

	extern std::string get_error_message(int error);
	extern std::string get_nt_message(NTSTATUS status);
	extern int get_last_error();
	extern NTSTATUS get_last_ntstatus();
	extern void set_last_ntstatus(NTSTATUS status);
	extern HRESULT get_last_hresult();
	extern void set_last_hresult(HRESULT result);
} // namespace base
