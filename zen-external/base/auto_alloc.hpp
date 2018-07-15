#pragma once
namespace base
{
	template<typename T>
	class auto_alloc_t
	{
	public:
		explicit auto_alloc_t(size_t size):
			size_(size)
		{
			buffer_ = new T[size];
		}

		auto_alloc_t(const auto_alloc_t&) = delete; // non construction-copyable
		auto_alloc_t& operator=(const auto_alloc_t&) = delete; // non copyable

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
}
