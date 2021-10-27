#pragma once

#include <3ds.h>
#include <citro3d.h>

#define DISPLAY_TRANSFER_FLAGS                                                                     \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) |               \
     GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

#define CLEAR_COLOR 0x68B0D8FF

class wii_graphics
{
private:
    static C3D_RenderTarget *target;

    static DVLB_s *vshader_dvlb;
    static shaderProgram_s program;
    static int uLoc_projection;
    static int uLoc_modelView;
    static C3D_Mtx projection;
    static void *vbo_data;

    static C3D_Tex textureObject;
    static uint8_t *textureData;

public:
    static void Initialize();
    static void Dispose();
    static void Render();
    static bool IsMainLoop();
    // static void LoadOrthoProjectionMatrix(float top, float bottom, float left, float right, float near, float far);
    // static void Load2DModelViewMatrix(uint32_t matrixIndex, float x, float y);
    // static uint32_t Create2DQuadDisplayList(void *displayList, float top, float bottom, float left, float right, float uvTop, float uvBottom, float uvLeft, float uvRight);
    // static void CallDisplayList(void *displayList, uint32_t displayListSize);
    static void CreateTextureObject(C3D_Tex *textureObject, uint8_t *textureData, uint16_t width, uint16_t height, GPU_TEXCOLOR format, GPU_TEXTURE_WRAP_PARAM wrap, GPU_TEXTURE_FILTER_PARAM filter);
    static void LoadTextureObject(C3D_Tex *textureObject, int32_t mapIndex);
    static uint32_t GetTextureSize(uint16_t width, uint16_t height, GPU_TEXCOLOR format, int32_t maxLevel);
    // static void FlushDataCache(void *startAddress, uint32_t size);
    // static void SwapBuffers();
};