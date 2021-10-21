#pragma once

#include <string>

class wii_sound
{
private:
    static FILE *fileHandle;

public:
    static void Initialize();
    static bool PlayMusic(std::string fileName);
    static void StopMusic();

private:
    static int32_t reader_callback(void *fp, void *dat, int32_t size);
};