#ifndef CONFIG_H
#define CONFIG_H

// Windows
#ifdef _WIN32
#define OS_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif

// Visual Studio
#ifdef _MSC_VER
#define SAFE_STRFN
#define SSE_ACCESS(x,type,i) x.type[i]
#endif

// GCC
#ifdef __GNUG__
#define SSE_ACCESS(x,type,i) x[i]
#endif

#endif
