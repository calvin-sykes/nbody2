#include "Config.h"
#include "Error.h"
#include "Sim.h"
#include "StartState.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#endif

#include <iostream>

// comment bunny //
/*	   ()()		 */
/*	   ('.')	 */
/*	   (()()	 */
/*	  *(_()()	 */
///////////////////

std::wstring charToWstring(const char* str)
{
	const auto size = strlen(str);
	std::wstring wstr;
	if (size > 0) {
		wstr.resize(size);
		mbstowcs(&wstr[0], str, size);
	}
	return wstr;
}

using namespace nbody;

int main(int argc, char** argv)
{
#ifdef OS_WINDOWS
	_putenv_s("OMP_WAIT_POLICY", "PASSIVE");
#endif
	try
	{
		Sim sim;

		sim.pushState(new StartState(&sim));
		sim.simLoop();

		return 0;
	}
	catch (Error e)
	{
#ifdef OS_WINDOWS
		MessageBox(nullptr, charToWstring(e.what()).data(), nullptr, MB_ICONERROR);
#else
		std::cout << e.what();
#endif
		return 1;
	}
	catch (std::exception e)
	{
#ifdef OS_WINDOWS
		MessageBox(nullptr, L"UNCAUGHT ERROR!", nullptr, MB_ICONERROR);
		MessageBox(nullptr, charToWstring(e.what()).data(), nullptr, MB_ICONERROR);
#else
		std::cout << "UNCAUGHT ERROR! " << e.what() << std::endl;
#endif
	}
	catch(...)
	{
#ifdef OS_WINDOWS
		MessageBox(nullptr, L"UNCAUGHT ERROR!", nullptr, MB_ICONERROR);
#endif
		return 1;
	}
	return 0;
}
