#pragma once

#include <3ds.h>

class n3ds_input
{
public:
    static void Initialize();
    static void ScanPads();

    static uint32_t GetButtonsDown();
    static uint32_t GetButtonsUp();
    static uint32_t GetButtonsHeld();
};