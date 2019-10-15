#ifndef DLOG

#if TELNETLOG
#include <RemoteDebug.h>
#define DLOG(msg, ...) if(Debug.isActive(Debug.DEBUG)){Debug.printf(msg, ##__VA_ARGS__);}
#define VLOG(msg, ...) if(Debug.isActive(Debug.VERBOSE)){Debug.printf(msg, ##__VA_ARGS__);}
RemoteDebug Debug;
#elif SERIALLOG
#define DLOG(msg, ...) Serial.printf(msg, ##__VA_ARGS__);}
#else
#define DLOG(msg, ...)
#endif

#endif
