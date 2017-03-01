#include <stdexcept>

#ifdef FLIGHT_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <cstring>

extern "C" int NSRunAlertPanel(CFStringRef strTitle, CFStringRef strMsg,
			       CFStringRef strButton1, CFStringRef strButton2, 
			       CFStringRef strButton3, ...);
#endif

void PromoteExceptionToOSDialogBox(const std::exception & ex) {
#ifdef FLIGHT_MAC
    id pool = reinterpret_cast<id>(objc_getClass("NSAutoreleasePool"));
    if (!pool) {
        return;
    }
    pool = objc_msgSend(pool, sel_registerName("alloc"));
    if (!pool) {
    	return;
    }
    pool = objc_msgSend(pool, sel_registerName("init"));
    CFStringRef str;
    char * bytes =
	reinterpret_cast<char *>(CFAllocatorAllocate(CFAllocatorGetDefault(), 1024, 0));
    strncpy(bytes, ex.what(), 1024);
    str = CFStringCreateWithCStringNoCopy(NULL, bytes,
					  kCFStringEncodingMacRoman, NULL);
    NSRunAlertPanel(CFSTR("Crash"), str, CFSTR("OK"), NULL, NULL);
    CFRelease(str);
    objc_msgSend(pool, sel_registerName("release"));
#endif
}
