#include "Dialog.hpp"

#ifdef FLIGHT_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <cstring>

#elif FLIGHT_WINDOWS
#include <Windows.h>
#endif

void PromoteExceptionToOSDialogBox(const std::exception & ex) {
#ifdef FLIGHT_MAC
    id pool = reinterpret_cast<id>(objc_getClass("NSAutoreleasePool"));
    pool = objc_msgSend(pool, sel_registerName("alloc"));
    pool = objc_msgSend(pool, sel_registerName("init"));
    id alert = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("alloc"));
    alert = objc_msgSend(alert, sel_registerName("init"));
    id button = objc_msgSend(alert, sel_registerName("addButtonWithTitle:"), CFSTR("OK"));
    CFStringRef str;
    char * bytes =
    	reinterpret_cast<char *>(CFAllocatorAllocate(CFAllocatorGetDefault(), 1024, 0));
    strncpy(bytes, ex.what(), 1024);
    str = CFStringCreateWithCStringNoCopy(nullptr, bytes,
    					  kCFStringEncodingMacRoman, nullptr);
    id text = objc_msgSend(alert, sel_registerName("setMessageText:"), CFSTR("Crash"));
    id infoText = objc_msgSend(alert, sel_registerName("setInformativeText:"), str);
    alert = objc_msgSend(alert, sel_registerName("runModal"));
    objc_msgSend(pool, sel_registerName("drain"));
#elif FLIGHT_WINDOWS
    MessageBox(nullptr, ex.what(), MB_OK);
#endif
}
