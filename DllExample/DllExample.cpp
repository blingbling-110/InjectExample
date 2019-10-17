#include "pch.h"
#include "DllExample.h"
#include <Windows.h>

void expFun()
{
	MessageBoxA(NULL, "动态链接库调用成功", "调用提示", MB_OK);
}