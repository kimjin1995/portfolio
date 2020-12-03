#include "Manager.h"

int main(int argc, char* argv[]) // main function
{
	const char* commandFilepath = "command.txt"; // get filepath
	if (argc > 1) // change filepath
		commandFilepath = argv[1];

	Manager m;
	m.Run(commandFilepath); // get filepath, run function
	return 0;
}
