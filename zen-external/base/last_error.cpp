#include "last_error.hpp" 
#include "auto_local.hpp"
#include <native/peb_native.hpp>

namespace base
{
	hresult_t::hresult_t(HRESULT result):
		result_(result)
	{
	}

	hresult_t::~hresult_t()
	{
		if (FAILED(result_))
			set_last_hresult(result_);
	}

	hresult_t::operator HRESULT() const
	{
		return result_;
	}

	ntstatus_t::ntstatus_t(NTSTATUS status):
		status_(status)
	{
	}

	ntstatus_t::~ntstatus_t()
	{
		if (!NT_SUCCESS(status_))
			set_last_ntstatus(status_);
	}

	ntstatus_t::operator NTSTATUS() const
	{
		return status_;
	}

	std::string get_error_message(int error)
	{
		auto_local_t<char*> message_buffer;
		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<char*>(&message_buffer), 0, nullptr);

		return message_buffer.get();
	}

	std::string get_nt_message(NTSTATUS status)
	{
		auto_local_t<char*> message_buffer;
		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
			LoadLibrary("ntdll.dll"), status, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<char*>(&message_buffer), 0, nullptr);

		return message_buffer.get();
	}

	int get_last_error()
	{
		return ::GetLastError();
	}

	NTSTATUS last_status = 0;
	long get_last_ntstatus()
	{
		NTSTATUS tmp = last_status;
		last_status = 0;
		return tmp;
	}

	void set_last_ntstatus(NTSTATUS status)
	{
		last_status = status;
	}


	HRESULT last_result = 0;
	HRESULT get_last_hresult()
	{
		HRESULT tmp = last_result;
		last_result = 0;
		return tmp;
	}

	void set_last_hresult(HRESULT result)
	{
		last_result = result;
	}
}
