#include "wii_graphics.h"

#include <cstdio>
#include <string>
#include <cstring>
#include <malloc.h>

#include "vshader_shbin.h"

C3D_RenderTarget *wii_graphics::target = nullptr;

DVLB_s *wii_graphics::vshader_dvlb = nullptr;
shaderProgram_s wii_graphics::program = {};
int wii_graphics::uLoc_projection = 0;
int wii_graphics::uLoc_modelView = 0;
C3D_Mtx wii_graphics::projection = {};
void *wii_graphics::vbo_data = nullptr;

typedef struct
{
    float position[3];
    float texcoord[2];
} vertex;

static const vertex vertex_list[] =
{
    {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
};

#define vertex_list_count (sizeof(vertex_list) / sizeof(vertex_list[0]))

void wii_graphics::Initialize()
{
    // Initialize graphics

    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

    // Initialize the render target

    target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    // Load the vertex shader, create a shader program and bind it

    vshader_dvlb = DVLB_ParseFile((u32 *)vshader_shbin, vshader_shbin_size);
    shaderProgramInit(&program);
    shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
    C3D_BindProgram(&program);

    // Get the location of the uniforms

    uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
    uLoc_modelView = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");

    // Configure attributes for use with the vertex shader

    C3D_AttrInfo *attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord

    // Create the VBO (vertex buffer object)

    vbo_data = linearAlloc(sizeof(vertex_list));
    memcpy(vbo_data, vertex_list, sizeof(vertex_list));

    // Configure buffers

    C3D_BufInfo *bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, vbo_data, sizeof(vertex), 2, 0x10);

    // Configure the first fragment shading substage to just pass through the vertex color
    // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight

    C3D_TexEnv *env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvColor(env, 0xff00ffff);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_CONSTANT, GPU_CONSTANT);
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
}

void wii_graphics::Dispose()
{
    // Free the VBO

    linearFree(vbo_data);

    // Free the shader program

    shaderProgramFree(&program);
    DVLB_Free(vshader_dvlb);

    // Deinitialize graphics

    C3D_Fini();
    gfxExit();
}

void wii_graphics::BeginRender()
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_RenderTargetClear(target, C3D_CLEAR_ALL, 0x401020ff, 0);
    C3D_FrameDrawOn(target);
}

void wii_graphics::FinishRender()
{
    C3D_FrameEnd(0);
}

bool wii_graphics::IsMainLoopRunning()
{
    return aptMainLoop();
}

void wii_graphics::SetOrthoProjectionMatrix(float left, float right, float bottom, float top, float near, float far)
{
    Mtx_OrthoTilt(&projection, left, right, bottom, top, near, far, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
}

void wii_graphics::SetModelViewMatrix(float x, float y, float w, float h)
{
    C3D_Mtx modelView;
    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, x, y, 0.5f, false);
    Mtx_Scale(&modelView, w, h, 1.0f);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);
}

void wii_graphics::DrawQuad()
{
    C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, vertex_list_count);
}

// void wii_graphics::Load2DModelViewMatrix(uint32_t matrixIndex, float x, float y)
// {
//     Mtx modelMatrix;
//     guMtxIdentity(modelMatrix);
//     guMtxTransApply(modelMatrix, modelMatrix, x, y, -5.0f);

//     Mtx viewMatrix;
//     guVector cam = {0.0F, 0.0F, 0.0F};
//     guVector up = {0.0F, 1.0F, 0.0F};
//     guVector look = {0.0F, 0.0F, -1.0F};
//     guLookAt(viewMatrix, &cam, &up, &look);

//     Mtx modelViewMatrix;
//     guMtxConcat(viewMatrix, modelMatrix, modelViewMatrix);
//     GX_LoadPosMtxImm(modelViewMatrix, matrixIndex);

//     GX_SetCurrentMtx(matrixIndex);
// }

// uint32_t wii_graphics::Create2DQuadDisplayList(void *displayList, float top, float bottom, float left, float right, float uvTop, float uvBottom, float uvLeft, float uvRight)
// {
//     // Configure vertex formats

//     GX_ClearVtxDesc();
//     GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
//     //GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
//     GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

//     GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
//     //GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGB8, 0);
//     GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

//     memset(displayList, 0, MAX_DISPLAY_LIST_SIZE);

//     // Invalidate vertex cache

//     GX_InvVtxCache();

//     // It's necessary to flush the data cache of the display list
//     // just before filling it.

//     DCInvalidateRange(displayList, MAX_DISPLAY_LIST_SIZE);

//     // Start generating the display list

//     GX_BeginDispList(displayList, MAX_DISPLAY_LIST_SIZE);

//     GX_Begin(GX_TRIANGLESTRIP, GX_VTXFMT0, 4);
//     GX_Position3f32(left, top, 0.0f);
//     //GX_Color3f32(1.0f, 0.0f, 0.0f);
//     GX_TexCoord2f32(uvLeft, uvTop);

//     GX_Position3f32(right, top, 0.0f);
//     //GX_Color3f32(0.0f, 1.0f, 0.0f);
//     GX_TexCoord2f32(uvRight, uvTop);

//     GX_Position3f32(left, bottom, 0.0f);
//     //GX_Color3f32(0.0f, 0.0f, 1.0f);
//     GX_TexCoord2f32(uvLeft, uvBottom);

//     GX_Position3f32(right, bottom, 0.0f);
//     //GX_Color3f32(1.0f, 1.0f, 1.0f);
//     GX_TexCoord2f32(uvRight, uvBottom);

//     GX_End();

//     return GX_EndDispList();
// }

// void wii_graphics::CallDisplayList(void *displayList, uint32_t displayListSize)
// {
//     GX_CallDispList(displayList, displayListSize);
// }

void wii_graphics::CreateTextureObject(C3D_Tex *textureObject, uint16_t width, uint16_t height, GPU_TEXCOLOR format, GPU_TEXTURE_WRAP_PARAM wrap, GPU_TEXTURE_FILTER_PARAM filter)
{
    bool success = C3D_TexInit(textureObject, width, height, format);

    if (!success)
    {
        fopen("Error creating texture object.", "r");
        std::string message = "Error creating texture object.";
        svcOutputDebugString(message.c_str(), message.length());
    }

    //C3D_TexUpload(textureObject, textureData);
    C3D_TexSetFilter(textureObject, filter, filter);
    C3D_TexSetWrap(textureObject, wrap, wrap);
}

void wii_graphics::UploadTextureObject(C3D_Tex *textureObject, uint8_t *textureData)
{
    C3D_TexUpload(textureObject, textureData);
    //C3D_TexFlush(&textureObject);
}

void wii_graphics::BindTextureObject(C3D_Tex *textureObject, int32_t mapIndex)
{
    C3D_TexBind(mapIndex, textureObject);
}

uint32_t wii_graphics::GetTextureSize(uint16_t width, uint16_t height, GPU_TEXCOLOR format, int32_t maxLevel)
{
    uint32_t size = 0;

    switch (format)
    {
    case GPU_RGBA8:
        size = 32;
        break;
    case GPU_RGB8:
        size = 24;
        break;
    case GPU_RGBA5551:
    case GPU_RGB565:
    case GPU_RGBA4:
    case GPU_LA8:
    case GPU_HILO8:
        size = 16;
        break;
    case GPU_L8:
    case GPU_A8:
    case GPU_LA4:
    case GPU_ETC1A4:
        size = 8;
        break;
    case GPU_L4:
    case GPU_A4:
    case GPU_ETC1:
        size = 4;
        break;
    }

    size *= (uint32_t)width * height / 8;
    return C3D_TexCalcTotalSize(size, maxLevel);
}

// void wii_graphics::FlushDataCache(void *startAddress, uint32_t size)
// {
//     DCFlushRange(startAddress, size);
// }

// void wii_graphics::SwapBuffers()
// {
//     GX_CopyDisp(frameBuffer[currentFramebuffer], GX_TRUE);
//     GX_DrawDone();
//     VIDEO_SetNextFramebuffer(frameBuffer[currentFramebuffer]);
//     VIDEO_Flush();
//     VIDEO_WaitVSync();
//     currentFramebuffer ^= 1;
// }