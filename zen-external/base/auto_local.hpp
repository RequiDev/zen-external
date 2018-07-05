#pragma once
#include <windows.h>

namespace base
{
	template<typename T>
	class auto_local_t
	{
	public:
		auto_local_t() = default;

		explicit auto_local_t(size_t size, uint32_t flags = 0)
		{
			mem_ = reinterpret_cast<T>(LocalAlloc(flags, size));
		}

		~auto_local_t()
		{
			free();
		}

		void realloc(size_t size, uint32_t flags = 0)
		{
			LocalReAlloc(mem_, size, flags);
		}

		void free()
		{
			LocalFree(mem_);
		}

		void lock()
		{
			LocalLock(mem_);
		}

		void unlock()
		{
			LocalUnlock(mem_);
		}

		size_t size()
		{
			return LocalSize(mem_);
		}

		uint32_t flags()
		{
			return LocalFlags(mem_);
		}

		T get() const
		{
			return mem_;
		}

		operator T() const
		{
			return mem_;
		}
	private:
		T mem_;
	};
}
