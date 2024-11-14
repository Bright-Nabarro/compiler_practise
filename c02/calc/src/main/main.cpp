#include <print>
#include <string>
#include "driver.hpp"

auto main(int argc, char* argv[]) -> int
{
	int ret = 0;
	
	Driver driver;
	for (int i = 0; i < argc; i++)
	{
		if (argv[i] == std::string{"-p"})
		{
			driver.set_trace(true);
		}
		else if (argv[i] == std::string { "-s" })
		{
			driver.set_trace(true);
		}
		else if (!driver.parse(argv[i]))
		{
			std::println("{}",driver.get_result());
		}
	}

	return ret;
}

