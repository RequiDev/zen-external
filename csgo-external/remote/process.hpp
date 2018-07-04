#pragma once
#include <base/auto_handle.hpp>

namespace remote
{
	/**
 * \brief A wrapper class for process and memory operations
 */
class process_t
{
public:
	/**
	 * \brief Opens a handle to a process using OpenProcess
	 * \param process_id Id to the process to open.
	 * \param desired_access What access the handle should have.
	 * \return Wether the handle could be opened or not.
	 */
	bool attach(uint32_t process_id, uint32_t desired_access = PROCESS_ALL_ACCESS);

	/**
	 * \brief Reads memory from an address into a buffer.
	 * \param address Address to read from.
	 * \param buffer Buffer in which the data gets stored in.
	 * \param size Size of memory that should be read.
	 * \return Wether the read operation succeeded or not. GetLastError() for more information.
	 */
	bool read_memory(uintptr_t address, LPVOID buffer, SIZE_T size) const;

	/**
	 * \brief Writes memory to an address from a buffer.
	 * \param address Address to write to.
	 * \param buffer Buffer which contains the data to write.
	 * \param size Size of memory that should be written.
	 * \return Wether the write operation succeeded or not. GetLastError() for more information
	 */
	bool write_memory(uintptr_t address, LPCVOID buffer, SIZE_T size) const;

	/**
	 * \brief Read memory from an address using a template value.
	 * \tparam T Any class/struct/type you want to write into memory.
	 * \param address Address to read from.
	 * \param value Value to read into.
	 * \return Wether the read operation succeeded or not. GetLastError() for more information.
	 */
	template<typename T>
	bool read(uintptr_t address, T& value)
	{
		return read_memory(address, &value, sizeof value);
	}

	/**
	 * \brief Writes memory to an address using a template value.
	 * \tparam T Any class/struct/type you want to write into memory.
	 * \param address Address to write to.
	 * \param value Value to write.
	 * \return Wether the write operation succeeded or not. GetLastError() for more information
	 */
	template<typename T>
	bool write(uintptr_t address, const T& value)
	{
		return write_memory(address, &value, sizeof value);
	}


private:
	base::handle_t handle_;
};

} // namespace remote