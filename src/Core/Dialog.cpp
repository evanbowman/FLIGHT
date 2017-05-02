#include <FLIGHT/Core/Dialog.hpp>

void CreateDialogBox(const char * message) {
#ifdef __APPLE__
    id pool = reinterpret_cast<id>(objc_getClass("NSAutoreleasePool"));
    pool = objc_msgSend(pool, sel_registerName("alloc"));
    pool = objc_msgSend(pool, sel_registerName("init"));
    id alert =
        objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("alloc"));
    alert = objc_msgSend(alert, sel_registerName("init"));
    objc_msgSend(alert, sel_registerName("addButtonWithTitle:"), CFSTR("OK"));
    CFStringRef str;
    static const size_t bufferSize = 1024;
    char * bytes = reinterpret_cast<char *>(
        CFAllocatorAllocate(CFAllocatorGetDefault(), bufferSize, 0));
    strncpy(bytes, message, bufferSize);
    str = CFStringCreateWithCStringNoCopy(nullptr, bytes,
                                          kCFStringEncodingMacRoman, nullptr);
    objc_msgSend(alert, sel_registerName("setMessageText:"), CFSTR("Crash"));
    objc_msgSend(alert, sel_registerName("setInformativeText:"), str);
    alert = objc_msgSend(alert, sel_registerName("runModal"));
    objc_msgSend(pool, sel_registerName("drain"));
#elif defined(_WIN32) or defined(_WIN64)
    MessageBox(nullptr, "Crash", message, MB_OK);
#elif __LINUX__
    // TODO: Is it worth linking all of gtk for this? Linux users should be ok
    // with running things in a terminal anyway.
    std::cerr << message << std::endl;
#endif
}
