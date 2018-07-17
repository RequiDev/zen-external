#pragma once
#include <base/auto_handle.hpp>
#include <remote/peb.hpp>
#include <base/last_error.hpp>
#include <libloaderapi.h>

namespace remote
{
/**
 * \brief A wrapper class for process and memory operations
 */
class process_t : base::no_copy_t
{
public:
	process_t();
	/**
	 * \brief Constructs the process class with an already existing handle.
	 * \param handle 
	 */
	explicit process_t(HANDLE handle);
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

	/**
	 * \brief Query information of the process.
	 * \tparam T Type of info to store the data in.
	 * \param info Reference to object where the info gets stored.
	 * \param info_class Which type of info should be pulled from the process.
	 * \return Wether the query succeeded or not.
	 */
	template<class T>
	bool query_information(T& info, PROCESSINFOCLASS info_class) const
	{
		using nqip_t = NTSTATUS(NTAPI*)(HANDLE, PROCESSINFOCLASS, PVOID, SIZE_T, PULONG);
		nqip_t nt_query_information_process = reinterpret_cast<nqip_t>(
			::GetProcAddress(::GetModuleHandle("ntdll.dll"), "NtQueryInformationProcess"));
		if (!nt_query_information_process)
			return false;

		base::ntstatus_t status;
		status = nt_query_information_process(handle_, info_class, &info, sizeof T, nullptr);

		return NT_SUCCESS(status);
	}

	/**
	 * \brief Query object of the process handle.
	 * \tparam T Type of info to store the data in.
	 * \param info Reference to object where the info gets stored.
	 * \param info_class Which type of info should be pulled from the process handle.
	 * \return Wether the query succeeded or not.
	 */
	template<class T>
	bool query_object(T& info, OBJECT_INFORMATION_CLASS info_class) const
	{
		using ntqo_t = NTSTATUS(NTAPI*)(HANDLE, OBJECT_INFORMATION_CLASS, PVOID, ULONG, PULONG);
		ntqo_t nt_query_object = reinterpret_cast<ntqo_t>(
			::GetProcAddress(::GetModuleHandle("ntdll.dll"), "NtQueryObject"));
		if (!nt_query_object)
			return false;

		base::ntstatus_t status;
		status = nt_query_object(handle_, info_class, &info, sizeof info, nullptr);

		return NT_SUCCESS(status);
	}

	/**
	 * \brief Reads memory from a UNICODE_STRING
	 * \param unicode The UNICODE_STRING that's in the remote process.
	 * \return The UNICODE_STRING read and returned as std::string.
	 */
	std::string read_unicode_string(const UNICODE_STRING& unicode) const;

	/**
	 * \brief Get granted access of the process handle.
	 * \return Granted access.
	 */
	uint32_t granted_access() const;

	/**
	 * \brief Gets module handle of the remote process.
	 * \param str Name of the module to be found.
	 * \param module_out Reference to module_t, where the module gets copied into.
	 * \return Wether the module was found or not.
	 */
	bool get_module(const char* str, module_t& module_out);

private:
	base::handle_t handle_;
	peb_t peb_;
};

} // namespace remote