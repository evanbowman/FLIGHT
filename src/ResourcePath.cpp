#include "ResourcePath.hpp"

#ifdef FLIGHT_WINDOWS
std::string ResourcePath() {
    HMODULE hModule = GetModuleHandleW(nullptr);
    char buffer[MAX_PATH];
    GetModuleFileName(hModule, buffer, MAX_PATH);
    const std::string path(buffer);
    const std::size_t lastFwdSlash = path.find_last_of('\\');
    std::string pathWithoutBinary = path.substr(0, lastFwdSlash + 1);
    return pathWithoutBinary + "..\\res\\";
}

#elif FLIGHT_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc.h>
#include <objc/objc-runtime.h>

std::string ResourcePath() {
    id pool = reinterpret_cast<id>(objc_getClass("NSAutoreleasePool"));
    std::string rpath;
    if (!pool) {
	return rpath;
    }
    pool = objc_msgSend(pool, sel_registerName("alloc"));
    if (!pool) {
	return rpath;
    }
    pool = objc_msgSend(pool, sel_registerName("init"));
    id bundle =
	objc_msgSend(reinterpret_cast<id>(objc_getClass("NSBundle")),
		     sel_registerName("mainBundle"));
    if (bundle) {
	id path = objc_msgSend(bundle, sel_registerName("resourcePath"));
	rpath =
	    reinterpret_cast<const char *>(objc_msgSend(path, sel_registerName("UTF8String"))) +
	    std::string("/");
    }
    objc_msgSend(pool, sel_registerName("drain"));
    return rpath;
}

#elif FLIGHT_LINUX
std::string ResourcePath() {
    char buffer[PATH_MAX];
    [[gnu::unused]] const std::size_t bytesRead =
        readlink("/proc/self/exe", buffer, sizeof(buffer));
    const std::string path(buffer);
    const std::size_t lastFwdSlash = path.find_last_of("/");
    std::string pathWithoutBinary = path.substr(0, lastFwdSlash + 1);
    return pathWithoutBinary + "../res/";
}
#endif
