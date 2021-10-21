#include "wii_sound.h"

#include <asndlib.h>
#include <mp3player.h>

FILE *wii_sound::fileHandle = nullptr;

void wii_sound::Initialize()
{
    ASND_Init();
    MP3Player_Init();
}

bool wii_sound::PlayMusic(std::string fileName)
{
    if (MP3Player_IsPlaying())
        return true;

    fileHandle = fopen(fileName.c_str(), "rb");
    if (!fileHandle)
        return false;

    MP3Player_PlayFile(fileHandle, reader_callback, 0);
    return true;
}

void wii_sound::StopMusic()
{
    if (!MP3Player_IsPlaying())
        return;

    MP3Player_Stop();
    fclose(fileHandle);
    fileHandle = nullptr;
}

int32_t wii_sound::reader_callback(void *fp, void *dat, int32_t size)
{
    return fread(dat, size, 1, fileHandle);
}
