#pragma once
#include <base/no_copy.hpp>
#include <base/no_move.hpp>

namespace base
{
	template<typename T>
	class auto_alloc_t : no_copy_t, no_move_t
	{
	public:
		auto_alloc_t():
			buffer_(nullptr),
			size_(0)
		{
		}

		explicit auto_alloc_t(size_t size):
			size_(size)
		{
			buffer_ = new T[size_];
		}

		void reset(size_t new_size)
		{
			if (buffer_)
				delete[] buffer_;

			size_ = new_size;
			buffer_ = new T[size_];
		}

		~auto_alloc_t()
		{
			delete[] buffer_;
		}

		T& operator[](int i)
		{
			return buffer_[i];
		}

		T operator[](int i) const
		{
			return buffer_[i];
		}

		size_t size() const
		{
			return size_ * sizeof T;
		}

		T* get()
		{
			return buffer_;
		}

		operator T*()
		{
			return buffer_;
		}
	private:
		T* buffer_;
		size_t size_;
	};
} // namespace base