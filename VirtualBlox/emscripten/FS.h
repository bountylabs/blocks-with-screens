#ifndef __FS_H__
#define __FS_H__

#include <stdlib.h>
#include <stdint.h>

namespace fs {

class File {
public:
    File();

    size_t write(const uint8_t *buf, size_t size);
    void close();
    void println(const char* str);
    operator bool() const;
};

class FS
{
public:
    FS();
    ~FS();

    File open(const char* path, const char* mode);
    //File open(const String& path, const char* mode);
    bool exists(const char* path);
    void begin();
    void format();

protected:
};

} // namespace fs

extern fs::FS SPIFFS;

#endif