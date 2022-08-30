#ifndef DLOG

#if TELNETLOG
// Make sure to also add RemoteDebug to your `lib_deps` in `platformio.ini`
// when using TELNETLOG
#include <RemoteDebug.h>
#define DLOG(msg, ...) if(Debug.isActive(Debug.DEBUG)){Debug.printf(msg, ##__VA_ARGS__);}
#define VLOG(msg, ...) if(Debug.isActive(Debug.VERBOSE)){Debug.printf(msg, ##__VA_ARGS__);}

// This must be called during `setup`
#define DLOGBegin(hostname) do { \
    Debug.begin(hostname); \
    Debug.setResetCmdEnabled(true); \
} while (0)

// This must be called periodically, usually at the same time you call `yield`
#define DLOGHandle() do { \
    Debug.handle(); \
} while (0)
RemoteDebug Debug;
#elif SERIALLOG
#define DLOG(msg, ...) log_d(msg, ##__VA_ARGS__);
#define DLOGBegin(hostname)
#define DLOGHandle()
#else
#define DLOG(msg, ...)
#define DLOGBegin(hostname)
#define DLOGHandle()
#endif

#endif
