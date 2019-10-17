#pragma once

#ifdef DLLEXAMPLE_EXPORTS
#define DLLEXAMPLE_API __declspec(dllexport)
#else
#define DLLEXAMPLE_API __declspec(dllimport)
#endif // DLLEXAMPLE_EXPORTS

extern "C" DLLEXAMPLE_API void expFun();