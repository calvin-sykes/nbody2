#include "Config.h"
#include "Error.h"
#include "Sim.h"
#include "StartState.h"

#ifdef OS_WINDOWS
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
	_putenv_s("OMP_WAIT_POLICY", "PASSIVE");
	
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
		MessageBox(NULL, charToWstring(e.what()).data(), NULL, MB_ICONERROR);
#else
		std::cout << e.what();
#endif
		return 1;
	}
	/*catch (std::exception e)
	{
#ifdef OS_WINDOWS
		MessageBox(NULL, L"UNCAUGHT ERROR!", NULL, MB_ICONERROR);
		MessageBox(NULL, charToWstring(e.what()).data(), NULL, MB_ICONERROR);
#else
		std::cout << "UNCAUGHT ERROR!" << std::endl;
#endif
	}*/
	return 0;
}
