#include "pch.h"
#include "winmain.h"

#include <malloc.h>

#include "control.h"
#include "midi.h"
#include "options.h"
#include "pb.h"
#include "pinball.h"
#include "render.h"
#include "Sound.h"
#include "n3ds_graphics.h"
#include "n3ds_input.h"

int winmain::bQuit = 0;
int winmain::activated;
int winmain::DispFrameRate = 0;
int winmain::DispGRhistory = 0;
int winmain::single_step = 0;
int winmain::last_mouse_x;
int winmain::last_mouse_y;
int winmain::mouse_down;
int winmain::no_time_loss;

std::string winmain::DatFileName;
bool winmain::ShowSpriteViewer = false;
bool winmain::LaunchBallEnabled = true;
bool winmain::HighScoresEnabled = true;
bool winmain::DemoActive = false;
char *winmain::BasePath;
std::string winmain::FpsDetails;
double winmain::UpdateToFrameRatio;
winmain::DurationMs winmain::TargetFrameTime;
optionsStruct &winmain::Options = options::Options;

int winmain::WinMain(LPCSTR lpCmdLine)
{
	std::set_new_handler(memalloc_failure);

	BasePath = (char *)"SpaceCadetPinball/";

	pinball::quickFlag = 0; // strstr(lpCmdLine, "-quick") != nullptr;
	DatFileName = options::get_string("Pinball Data", pinball::get_rc_string(168, 0));

	// Check for full tilt .dat file and switch to it automatically

	auto cadetFilePath = pinball::make_path_name("CADET.DAT");
	auto cadetDat = fopen(cadetFilePath.c_str(), "r");
	if (cadetDat)
	{
		fclose(cadetDat);
		DatFileName = "CADET.DAT";
		pb::FullTiltMode = true;
	}

	// PB init from message handler

	{
		options::init();
		if (!Sound::Init(Options.SoundChannels, Options.Sounds))
			Options.Sounds = false;

		if (!pinball::quickFlag && !midi::music_init())
			Options.Music = false;

		if (pb::init())
		{
			fprintf(stderr, "Could not load game data: The .dat file is missing");
			exit(EXIT_FAILURE);
		}
	}

	// Initialize graphics

	n3ds_graphics::Initialize();

	// Texture data and create texture object

	uint16_t texWidth = 1024;
	uint16_t texHeight = 512;
	uint32_t textureSize = n3ds_graphics::GetTextureSize(texWidth, texHeight, GPU_RGBA8, 0);
	uint8_t *textureData = (uint8_t *)linearAlloc(textureSize);
	memset(textureData, 0, textureSize);
	C3D_Tex textureObject;
	n3ds_graphics::CreateTextureObject(&textureObject, texWidth, texHeight, GPU_RGBA8, GPU_CLAMP_TO_EDGE, GPU_LINEAR);
	n3ds_graphics::BindTextureObject(&textureObject, 0);

	// Set the projection matrix according to screen texture resolution

	n3ds_graphics::SetOrthoProjectionMatrix(0, render::vscreen->Width, 0, render::vscreen->Height, 0.1f, 1.0f);

	// Initialize input

	n3ds_input::Initialize();

	// Initialize game

	pb::reset_table();
	pb::firsttime_setup();
	pb::replay_level(0);

	// Begin main loop

	bQuit = false;

	while (n3ds_graphics::IsMainLoopRunning() && !bQuit)
	{
		// Input

		n3ds_input::ScanPads();

		uint32_t buttonsDown = n3ds_input::GetButtonsDown();
		uint32_t buttonsUp = n3ds_input::GetButtonsUp();

		if (buttonsDown & KEY_Y)
			break;

		if (buttonsDown & KEY_START)
			pause();

		if (buttonsDown & KEY_X)
			new_game();

		pb::keydown(buttonsDown);
		pb::keyup(buttonsUp);

		if (!single_step)
		{
			// Update game when not paused

			pb::frame(1000.0f / 60.0f);

			// Copy game screen buffer to texture

			// for (int32_t y = 0; y < render::vscreen->Height; y++)
			// {
			// 	for (int32_t x = 0; x < render::vscreen->Width; x++)
			// 	{
			// 		render::vscreen->BmpBufPtr1[y * render::vscreen->Width + x].rgba.Alpha = 0xFF;
			// 		render::vscreen->BmpBufPtr1[y * render::vscreen->Width + x].rgba.Blue = 0x00;
			// 		render::vscreen->BmpBufPtr1[y * render::vscreen->Width + x].rgba.Green = y & 0xff;
			// 		render::vscreen->BmpBufPtr1[y * render::vscreen->Width + x].rgba.Red = x & 0xff;
			// 	}
			// }

			uint32_t dstOffset = 0;
			uint32_t widthCount = 0;
			constexpr uint32_t tileSize = 8;
			constexpr uint32_t subTileSize1 = 4;
			constexpr uint32_t subTileSize2 = 2;
			uint32_t widthBytes = render::vscreen->Width * tileSize * 4;

			for (int32_t y = 0; y < render::vscreen->Height; y += tileSize)
			{
				for (int32_t x = 0; x < render::vscreen->Width; x += tileSize)
				{
					for (uint8_t ty = 0; ty < tileSize; ty += subTileSize1)
					{
						for (uint8_t tx = 0; tx < tileSize; tx += subTileSize1)
						{
							for (uint8_t sty1 = 0; sty1 < subTileSize1; sty1 += subTileSize2)
							{
								for (uint8_t stx1 = 0; stx1 < subTileSize1; stx1 += subTileSize2)
								{
									uint32_t index = (y + ty + sty1 + 0) * render::vscreen->Width + (x + tx + stx1);
									Rgba color = render::vscreen->BmpBufPtr1[index + 0].rgba;
									textureData[dstOffset + 0] = color.Alpha;
									textureData[dstOffset + 1] = color.Blue;
									textureData[dstOffset + 2] = color.Green;
									textureData[dstOffset + 3] = color.Red;

									color = render::vscreen->BmpBufPtr1[index + 1].rgba;
									textureData[dstOffset + 4] = color.Alpha;
									textureData[dstOffset + 5] = color.Blue;
									textureData[dstOffset + 6] = color.Green;
									textureData[dstOffset + 7] = color.Red;

									index = (y + ty + sty1 + 1) * render::vscreen->Width + (x + tx + stx1);
									color = render::vscreen->BmpBufPtr1[index + 0].rgba;
									textureData[dstOffset + 8] = color.Alpha;
									textureData[dstOffset + 9] = color.Blue;
									textureData[dstOffset + 10] = color.Green;
									textureData[dstOffset + 11] = color.Red;

									color = render::vscreen->BmpBufPtr1[index + 1].rgba;
									textureData[dstOffset + 12] = color.Alpha;
									textureData[dstOffset + 13] = color.Blue;
									textureData[dstOffset + 14] = color.Green;
									textureData[dstOffset + 15] = color.Red;

									dstOffset += 16;
									widthCount += 16;
									if (widthCount == widthBytes)
									{
										dstOffset += (texWidth - render::vscreen->Width) << 5;
										widthCount = 0;
									}
								}
							}
						}
					}
				}
			}

			n3ds_graphics::UploadTextureObject(&textureObject, textureData);
		}

		// Render fullscreen quads

		n3ds_graphics::BeginRender();

		constexpr float separationX = 375;
		float tableWidthCoefficient = separationX / texWidth;
		float y = render::vscreen->Height - texHeight;
		n3ds_graphics::DrawQuad(render::get_offset_x(), y - render::get_offset_y(), separationX, texHeight, 0.0, 0.0, tableWidthCoefficient, 1.0);
		n3ds_graphics::DrawQuad(separationX, y, render::vscreen->Width - separationX, texHeight, tableWidthCoefficient, 0.0, (render::vscreen->Width - separationX) / texWidth, 1.0);

		n3ds_graphics::FinishRender();
	}

	printf("Uninitializing...");

	end_pause();

	options::uninit();
	midi::music_shutdown();
	pb::uninit();
	Sound::Close();
	n3ds_graphics::Dispose();

	C3D_TexDelete(&textureObject);
	linearFree(textureData);
	n3ds_graphics::Dispose();

	printf("Finished uninitializing.");

	return 0;
}

void winmain::memalloc_failure()
{
	midi::music_stop();
	Sound::Close();
	char *caption = pinball::get_rc_string(170, 0);
	char *text = pinball::get_rc_string(179, 0);
	fprintf(stderr, "%s %s\n", caption, text);
	exit(EXIT_FAILURE);
}

void winmain::end_pause()
{
	if (single_step)
	{
		pb::pause_continue();
		no_time_loss = 1;
	}
}

void winmain::new_game()
{
	end_pause();
	pb::replay_level(0);
}

void winmain::pause()
{
	pb::pause_continue();
	no_time_loss = 1;
}

void winmain::UpdateFrameRate()
{
	// UPS >= FPS
	auto fps = Options.FramesPerSecond, ups = Options.UpdatesPerSecond;
	UpdateToFrameRatio = static_cast<double>(ups) / fps;
	TargetFrameTime = DurationMs(1000.0 / ups);
}
