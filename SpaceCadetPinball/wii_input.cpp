#include "wii_input.h"

void wii_input::Initialize()
{
}

void wii_input::ScanPads()
{
    hidScanInput();
}

uint32_t wii_input::GetButtonsDown()
{
    return hidKeysDown();
}

uint32_t wii_input::GetButtonsUp()
{
    return hidKeysUp();
}

uint32_t wii_input::GetButtonsHeld()
{
    return hidKeysHeld();
}
