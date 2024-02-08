#include "wii_input.h"

uint32_t wii_input::gcButtonsDown = 0;
uint32_t wii_input::gcButtonsUp = 0;
#ifdef HW_RVL
uint32_t wii_input::wiiButtonsDown = 0;
uint32_t wii_input::wiiButtonsUp = 0;
#endif

void wii_input::Initialize()
{
    PAD_Init();

#ifdef HW_RVL
    WPAD_Init();
#endif
}

void wii_input::ScanPads()
{
    PAD_ScanPads();
    gcButtonsDown = PAD_ButtonsDown(0);
    gcButtonsUp = PAD_ButtonsUp(0);

#ifdef HW_RVL
    WPAD_ScanPads();
    wiiButtonsDown = WPAD_ButtonsDown(0);
    wiiButtonsUp = WPAD_ButtonsUp(0);
#endif
}

bool wii_input::Exit()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_BUTTON_HOME) || (gcButtonsDown & PAD_TRIGGER_Z);
#else
    return (gcButtonsDown & PAD_TRIGGER_Z);
#endif
}

bool wii_input::Pause()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_BUTTON_PLUS) || (gcButtonsDown & PAD_BUTTON_START);
#else
    return (gcButtonsDown & PAD_BUTTON_START);
#endif
}

bool wii_input::NewGame()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_BUTTON_1) || (gcButtonsDown & PAD_BUTTON_Y);
#else
    return (gcButtonsDown & PAD_BUTTON_Y);
#endif
}

bool wii_input::LaunchBallDown()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_BUTTON_A) || (gcButtonsDown & PAD_BUTTON_A);
#else
    return (gcButtonsDown & PAD_BUTTON_A);
#endif
}

bool wii_input::LaunchBallUp()
{
#ifdef HW_RVL
    return (wiiButtonsUp & WPAD_BUTTON_A) || (gcButtonsUp & PAD_BUTTON_A);
#else
    return (gcButtonsUp & PAD_BUTTON_A);
#endif
}

bool wii_input::MoveLeftPaddleDown()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_NUNCHUK_BUTTON_Z) || (gcButtonsDown & PAD_TRIGGER_L);
#else
    return (gcButtonsDown & PAD_TRIGGER_L);
#endif
}

bool wii_input::MoveLeftPaddleUp()
{
#ifdef HW_RVL
    return (wiiButtonsUp & WPAD_NUNCHUK_BUTTON_Z) || (gcButtonsUp & PAD_TRIGGER_L);
#else
    return (gcButtonsUp & PAD_TRIGGER_L);
#endif
}

bool wii_input::MoveRightPaddleDown()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_BUTTON_B) || (gcButtonsDown & PAD_TRIGGER_R);
#else
    return (gcButtonsDown & PAD_TRIGGER_R);
#endif
}

bool wii_input::MoveRightPaddleUp()
{
#ifdef HW_RVL
    return (wiiButtonsUp & WPAD_BUTTON_B) || (gcButtonsUp & PAD_TRIGGER_R);
#else
    return (gcButtonsUp & PAD_TRIGGER_R);
#endif
}

bool wii_input::NudgeLeftDown()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_BUTTON_LEFT) || (gcButtonsDown & PAD_BUTTON_LEFT);
#else
    return (gcButtonsDown & PAD_BUTTON_LEFT);
#endif
}

bool wii_input::NudgeLeftUp()
{
#ifdef HW_RVL
    return (wiiButtonsUp & WPAD_BUTTON_LEFT) || (gcButtonsUp & PAD_BUTTON_LEFT);
#else
    return (gcButtonsUp & PAD_BUTTON_LEFT);
#endif
}

bool wii_input::NudgeRightDown()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_BUTTON_RIGHT) || (gcButtonsDown & PAD_BUTTON_RIGHT);
#else
    return (gcButtonsDown & PAD_BUTTON_RIGHT);
#endif
}

bool wii_input::NudgeRightUp()
{
#ifdef HW_RVL
    return (wiiButtonsUp & WPAD_BUTTON_RIGHT) || (gcButtonsUp & PAD_BUTTON_RIGHT);
#else
    return (gcButtonsUp & PAD_BUTTON_RIGHT);
#endif
}

bool wii_input::NudgeUpDown()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_BUTTON_UP) || (gcButtonsDown & PAD_BUTTON_UP);
#else
    return (gcButtonsDown & PAD_BUTTON_UP);
#endif
}

bool wii_input::NudgeUpUp()
{
#ifdef HW_RVL
    return (wiiButtonsUp & WPAD_BUTTON_UP) || (gcButtonsUp & PAD_BUTTON_UP);
#else
    return (gcButtonsUp & PAD_BUTTON_UP);
#endif
}

bool wii_input::SkipError()
{
#ifdef HW_RVL
    return (wiiButtonsDown & WPAD_BUTTON_A) || (gcButtonsDown & PAD_BUTTON_A);
#else
    return (gcButtonsDown & PAD_BUTTON_A);
#endif
}