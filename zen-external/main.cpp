#include <iostream>
#include <base/last_error.hpp>
#include <cheat/cheat.hpp>

std::string error_codes[] =
{
	"Could not find game window.",
	"Could not attach to game process.",
	"Could not create overlay."
};

int main(int argc, char* argv[])
{
	cheat::cheat_t cheat("Counter-Strike: Global Offensive", "Valve001");
	int return_code(cheat.mainloop());
	if (return_code != EXIT_SUCCESS)
	{
		std::string error_message;

		int last_error(base::get_last_error());
		HRESULT last_hresult(base::get_last_hresult());
		NTSTATUS last_ntstatus(base::get_last_ntstatus());
		if (last_error)
			error_message = base::get_error_message(last_error);
		else if (last_hresult)
			error_message = base::get_error_message(last_hresult);
		else if (last_ntstatus)
			error_message = base::get_nt_message(last_ntstatus);

		std::cout << "Something went wrong: " << error_codes[return_code - 1000] << (error_message.empty() ? "" : "\n(" + std::to_string(last_error) + ") ") << error_message << std::endl;
		std::cin.get();
	}
	return return_code;
}
