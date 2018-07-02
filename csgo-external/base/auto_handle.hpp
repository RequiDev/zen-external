#pragma once

#include <windef.h>
#include <memory>
#include <handleapi.h>

namespace base
{
	// Credits to DarthTon for move semantics and customization per template parameter
	template<typename T, typename F, F CloseFn, T ZeroHandle>
	class auto_handle_t
	{
	public:
		explicit auto_handle_t(T handle = ZeroHandle) noexcept:
			handle_(handle)
		{
		}

		auto_handle_t(auto_handle_t&& rhs) noexcept
			: handle_(rhs.handle_)
		{
			rhs.handle_ = ZeroHandle;
		}

		~auto_handle_t()
		{
			if (handle_ != ZeroHandle)
				CloseFn(handle_);
		}

		auto_handle_t(const auto_handle_t&) = delete;
		auto_handle_t& operator =(const auto_handle_t&) = delete;

		T get() const noexcept
		{
			return handle_;
		}

		auto_handle_t& operator =(auto_handle_t&& rhs) noexcept
		{
			if (std::addressof(rhs) == this)
				return *this;

			reset(rhs.handle_);
			rhs.handle_ = ZeroHandle;

			return *this;
		}

		auto_handle_t& operator =(T handle) noexcept
		{
			reset(handle);
			return *this;
		}

		void reset(T handle = ZeroHandle) noexcept
		{
			if (handle == handle_)
				return;

			if (handle_ != ZeroHandle)
				CloseFn(handle_);

			handle_ = handle;
		}

		T release() noexcept
		{
			auto tmp = handle_;
			handle_ = ZeroHandle;
			return tmp;
		}

		operator T() const noexcept
		{
			return handle_;
		}

		explicit operator bool() const noexcept
		{
			return handle_ != ZeroHandle;
		}

		bool operator ==(const auto_handle_t& rhs) noexcept
		{
			return rhs.handle_ == handle_;
		}

		T* operator&() noexcept
		{
			return &handle_;
		}
	private:
		T handle_;
	};

	using handle_t = auto_handle_t<HANDLE, decltype(&CloseHandle), &CloseHandle, nullptr>;
	using file_handle_t = auto_handle_t<HANDLE, decltype(&CloseHandle), &CloseHandle, INVALID_HANDLE_VALUE>;
	using snap_handle_t = file_handle_t;
} // namespace base