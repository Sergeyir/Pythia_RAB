#include <iostream>
#include <string>
#include <fstream>

void PrintError()
{
	printf("\033[1m\033[31mError: ");
	printf("\033[0m");
	exit(1);
}


void PrintErrorWithoutExit()
{
	printf("\033[1m\033[31mError: ");
	printf("\033[0m");
}

void PrintError(std::string message)
{
	printf("\033[1m\033[31mError: ");
	printf("\033[0m");
	std::cout << message << std::endl;
	exit(1);
}

void PrintWarning()
{
	printf("\033[1m\033[33mWarning: ");
	printf("\033[0m");
}

void PrintWarning(std::string message)
{
	printf("\033[1m\033[33mWarning: ");
	printf("\033[0m");
	std::cout << message << std::endl;
}

void CheckInputFile(std::string name)
{
	std::ifstream file(name.c_str());

	if(!file.is_open())
	{
		PrintErrorWithoutExit();
		std::cout << "File " << name << " not found" << std::endl;
		exit(1);
	}
}

void CheckOutputFile(std::string name)
{
	std::ofstream file(name.c_str());

	if(!file.is_open())
	{
		PrintErrorWithoutExit();
		std::cout << "File " << name << " cannot be created" << std::endl;
		exit(1);
	}
}
