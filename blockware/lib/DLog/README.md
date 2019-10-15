# DLog

Simple header library to handle serial / telnet debug logging.

To add a log, simply use `DLOG` like you would `printf`

    DLOG("%d bottles of beer on the wall\n", numBottles);

To save ram, you might consider using `PSTR` to store strings in flash.

    DLOG(PSTR("%d bottles of beer on the wall\n"), numBottles);

By default log lines become no-ops in your program. If you'd like them to go somewhere you can define either `SERIALLOG` or `TELNETLOG` before `#include <DLog.h>` at the start of your program.

    #define SERIALLOG 1
    #include <DLog.h>

When using `TELNETLOG`, the [RemoteDebug](https://github.com/JoaoLopesF/RemoteDebug) library is used to make logs available in realtime over the network. You can simply use `telnet hostname.local` OR `nc hostname.local 23` OR `telnet 192.168.1.85` to connect to the device (of course replace hostname or the IP with your device's hostname or IP).
