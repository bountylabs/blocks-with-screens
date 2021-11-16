#include "FS.h"

using fs::FS;
using fs::File;

fs::FS SPIFFS;

File::File()
{

}

size_t File::write(const uint8_t *buf, size_t size)
{
    return 0;
}

void File::close()
{

}

void File::println(const char* str)
{

}

File::operator bool() const
{
    return true;
}


FS::FS()
{

}
FS::~FS()
{

}

File FS::open(const char* path, const char* mode)
{
    return File();
}
//File open(const String& path, const char* mode);
bool FS::exists(const char* path)
{
    return false;
}
void FS::begin()
{

}
void FS::format()
{

}