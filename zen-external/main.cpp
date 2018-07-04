#include <iostream>
#include <cheat/cheat.hpp>
#include <winbase.h>

std::string error_codes[] =
{
	"Could not find game window.",
	"Could not create overlay.",
	"Could not attach to game process."
};

std::string get_last_error(int* last_error = nullptr)
{
	int error_message_id(::GetLastError());
	if (error_message_id == 0)
		return "";

	if (last_error)
		*last_error = error_message_id;

	char* message_buffer = nullptr;
	::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, error_message_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<char*>(&message_buffer), 0, nullptr);

	std::string message(message_buffer);

	::LocalFree(message_buffer);

	return message;
};

int main(int argc, char* argv[])
{
	cheat::cheat_t cheat("Counter-Strike: Global Offensive", "Valve001");
	int return_code(cheat.mainloop());
	if (return_code != EXIT_SUCCESS)
	{
		int last_error;
		std::string message(get_last_error(&last_error));
		std::cout << "Something went wrong: " << error_codes[return_code - 1000] << (message.empty() ? "" : "\n(" + std::to_string(last_error) + ") ") << message << std::endl;
		std::cin.get();
	}
	return return_code;
}
