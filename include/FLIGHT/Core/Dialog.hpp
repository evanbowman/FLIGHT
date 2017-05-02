#pragma once

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <cstring>
#include <objc/objc-runtime.h>
#include <objc/objc.h>
#elif defined(_WIN32) or defined(_WIN64)
#include <Windows.h>
#elif __LINUX__
#include <iostream>
#endif

void CreateDialogBox(const char * message);
