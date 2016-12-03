#include "Sim.h"
#include "StartState.h"
#include "Error.h"

#ifdef NBOS_WINDOWS
#include <Windows.h>
#endif

// comment bunny //
/*	   ()()		 */
/*	   ('.')	 */
/*	   (()()	 */
/*	  *(_()()	 */
///////////////////

std::wstring charToWstring(const char* str)
{
	const size_t size = strlen(str);
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
	try
	{
		Sim sim;

		sim.pushState(new StartState(&sim));
		sim.simLoop();

		return 0;
	}
	catch (Error e)
	{
#ifdef NBOS_WINDOWS
		MessageBox(NULL, charToWstring(e.what()).data(), NULL, MB_ICONERROR);
#else
		std::cout << e.what();
#endif
		return 1;
	}
	catch (...)
	{
		std::cout << "Something happened...";
		return 1;
	}
	return 0;
}
