#include "Sandbox.h"

#include <cstdlib>

int main( int argc, char* args[] )
{
	Sandbox* sb = new Sandbox();

	if (!sb->setup()) return EXIT_FAILURE;
	
	while (sb->loop());

	delete sb;

	return EXIT_SUCCESS;
}
