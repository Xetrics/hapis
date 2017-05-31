#include <Windows.h>
#include <iostream>
#include <inttypes.h>

#include "rrapi.h"

int main()
{
	try
	{
		RustNetAPI::Init();

		// TODO: Proxy & hacks
		return 0;
	}
	catch (std::exception e)
	{
		printf("ERROR: %s\n", e.what());
		return 1;
	}
}