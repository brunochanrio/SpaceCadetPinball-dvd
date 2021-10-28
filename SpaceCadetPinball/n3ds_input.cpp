#include "n3ds_input.h"

void n3ds_input::Initialize()
{
}

void n3ds_input::ScanPads()
{
    hidScanInput();
}

uint32_t n3ds_input::GetButtonsDown()
{
    return hidKeysDown();
}

uint32_t n3ds_input::GetButtonsUp()
{
    return hidKeysUp();
}

uint32_t n3ds_input::GetButtonsHeld()
{
    return hidKeysHeld();
}
