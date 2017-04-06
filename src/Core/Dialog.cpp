#include <FLIGHT/Core/Dialog.hpp>

#ifdef FLIGHT_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <cstring>
#include <objc/objc-runtime.h>
#include <objc/objc.h>

#elif FLIGHT_WINDOWS
#include <Windows.h>

#elif FLIGHT_LINUX
#include <iostream>
#endif

void PromoteExceptionToOSDialogBox(const std::exception & ex) {
#ifdef FLIGHT_MAC
    id pool = reinterpret_cast<id>(objc_getClass("NSAutoreleasePool"));
    pool = objc_msgSend(pool, sel_registerName("alloc"));
    pool = objc_msgSend(pool, sel_registerName("init"));
    id alert =
        objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("alloc"));
    alert = objc_msgSend(alert, sel_registerName("init"));
    id button = objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
                             CFSTR("OK"));
    CFStringRef str;
    static const size_t bufferSize = 1024;
    char * bytes = reinterpret_cast<char *>(
        CFAllocatorAllocate(CFAllocatorGetDefault(), bufferSize, 0));
    strncpy(bytes, ex.what(), bufferSize);
    str = CFStringCreateWithCStringNoCopy(nullptr, bytes,
                                          kCFStringEncodingMacRoman, nullptr);
    id text = objc_msgSend(alert, sel_registerName("setMessageText:"),
                           CFSTR("Crash"));
    id infoText =
        objc_msgSend(alert, sel_registerName("setInformativeText:"), str);
    alert = objc_msgSend(alert, sel_registerName("runModal"));
    objc_msgSend(pool, sel_registerName("drain"));
#elif FLIGHT_WINDOWS
    MessageBox(nullptr, "Crash", ex.what(), MB_OK);
#elif FLIGHT_LINUX
    // TODO: Is it worth linking all of gtk for this? Linux users should be ok
    // with running things in a terminal anyway.
    std::cerr << ex.what() << std::endl;
#endif
}
