#include "Platform.h"
#include "UtilLog.h"
#include "Window.h"
#include "GfxApi.h"
#include "GfxDevice.h"

#include <ctype.h>

using namespace Rush;

struct RushPlatformContext
{
    MouseState      mouse;
    KeyboardState   keybd;
    RushAppConfig   cfg;
    Window*         window;
    RenderDevice*   render_device;
    void*           user_data;
};

static void DummyCallback(RushPlatformContext*) {};

void RushAppConfig_Init(RushAppConfig* cfg)
{
    RushAppConfig_SetResolution(cfg, RushAppResolution_iPhone_Landscape);

    cfg->max_height     = 0;
    cfg->max_width      = 0;
    cfg->full_screen    = false;
    cfg->nvperfhud      = false;

    cfg->name        = "Window";
    cfg->vsync       = 1;
    cfg->resizable   = 0;
    cfg->argc        = 0;
    cfg->argv        = NULL;

    cfg->on_startup  = DummyCallback;
    cfg->on_update   = DummyCallback;    
    cfg->on_draw     = DummyCallback;
    cfg->on_shutdown = DummyCallback;    
}

void RushAppConfig_SetResolution(RushAppConfig* cfg, RushAppResolution res)
{
    switch(res)
    {
    case RushAppResolution_HDTV_720p:
        cfg->width	= 1280;
        cfg->height	= 720;
        break;

    case RushAppResolution_HDTV_1080p:
        cfg->width	= 1920;
        cfg->height	= 1080;
        break;

    case RushAppResolution_iPad_Landscape:
        cfg->width	= 1024;
        cfg->height	= 768;
        break;

    case RushAppResolution_iPad_Portrait:
        cfg->width	= 768;
        cfg->height	= 1024;
        break;

    case RushAppResolution_iPhone_Landscape:
        cfg->width	= 480;
        cfg->height	= 320;
        break;

    case RushAppResolution_iPhone_Portrait:
        cfg->width	= 320;
        cfg->height	= 480;
        break;

    case RushAppResolution_iPhone4_Landscape:
        cfg->width	= 960;
        cfg->height	= 640;
        break;

    case RushAppResolution_iPhone4_Portrait:
        cfg->width	= 640;
        cfg->height	= 960;
        break;
    }
}

RushPlatformContext* RushPlatform_StartUp(RushAppConfig* cfg)
{
    RushPlatformContext* context = new RushPlatformContext;

    if( cfg )
    {
        context->cfg = *cfg;
    }
    else
    {
        RushAppConfig_Init(&context->cfg);
    }

#if defined(RUSH_PLATFORM_WINDOWS)
    context->window = RushPlatform_CreateWindow(context);
#else
    context->window = NULL;
#endif

    RenderDeviceConfig rdcfg;

    rdcfg.back_buffer_width = context->cfg.max_width ? context->cfg.max_width : context->cfg.width;
    rdcfg.back_buffer_height = context->cfg.max_height ? context->cfg.max_height : context->cfg.height;
    rdcfg.use_full_screen = context->cfg.full_screen;
    rdcfg.use_vertical_sync = context->cfg.vsync!=0;
    rdcfg.use_nvperfhud = false;

#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE
    context->render_device = Gfx_CreateDevice(context->window, rdcfg);
#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE

    context->cfg.on_startup(context);

    return context;
}

void RushPlatform_ShutDown(RushPlatformContext* context)
{
    context->cfg.on_shutdown(context);

#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE
    Gfx_DestroyDevice(context->render_device);
#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE

    delete context;
}

void RushPlatform_Update(RushPlatformContext* context)
{
    context->cfg.on_update(context);
}

void RushPlatform_Draw(RushPlatformContext* context)
{
#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE    
    Gfx_FrameBegin(context->render_device);

    context->cfg.on_draw(context);

    Gfx_FrameEnd(context->render_device);
    Gfx_Present(context->render_device);
#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE
}

void* RushPlatform_GetUserData(RushPlatformContext* context)
{
    return context->user_data;
}

void RushPlatform_SetUserData(RushPlatformContext* context, void* data)
{
    context->user_data = data;
}

void RushPlatform_KeyDown(RushPlatformContext* context, unsigned char code)
{
    code = (unsigned char)toupper(code);
    context->keybd.keys[code] = true;
}

void RushPlatform_KeyUp(RushPlatformContext* context, unsigned char code)
{
    code = (unsigned char)toupper(code);    
    context->keybd.keys[code] = false;
}

void RushPlatform_MouseDown(RushPlatformContext* context, int button, float x, float y)
{
    Vector2 mouse_pos(x, y);

    context->mouse.pos = mouse_pos;
    context->mouse.pos_pixels = mouse_pos;
    context->mouse.buttons[button] = true;
}

void RushPlatform_MouseUp(RushPlatformContext* context, int button, float x, float y)
{
    Vector2 mouse_pos(x, y);

    context->mouse.pos = mouse_pos;
    context->mouse.pos_pixels = mouse_pos;
    context->mouse.buttons[button] = false;
}

void RushPlatform_MouseMove(RushPlatformContext* context, float x, float y)
{
    Vector2 mouse_pos(x, y);

    context->mouse.pos = mouse_pos;
    context->mouse.pos_pixels = mouse_pos;
}

void RushPlatform_MouseWheel( RushPlatformContext* context, int h, int v )
{
    context->mouse.wheel_h = h;    
    context->mouse.wheel_v = v;
}

bool RushPlatform_GetKeyState(RushPlatformContext* context, unsigned char code)
{
    return context->keybd.is_key_down(code);
}

void RushPlatform_GetMousePos(RushPlatformContext* context, float* x, float* y)
{
    if(x) *x = context->mouse.pos.x;
    if(y) *y = context->mouse.pos.y;
}

bool RushPlatform_GetMouseButtonState(RushPlatformContext* context, unsigned char button)
{
    int max_buttons = sizeof(context->mouse.buttons)/sizeof(context->mouse.buttons[0]);
    if( button < max_buttons )
    {
        return context->mouse.buttons[button];
    }
    else
    {
        return false;
    }
}

const RushAppConfig* RushPlatform_GetConfig(RushPlatformContext* context)
{
    return &context->cfg;
}

RenderDevice*  RushPlatform_GetRenderDevice(RushPlatformContext* context)
{
    return context->render_device;
}

RenderContext* RushPlatform_GetRenderContext(RushPlatformContext* context)
{
#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE
    return Gfx_Immediate_Context(context->render_device);
#else //RUSH_RENDER_API!=RUSH_RENDER_API_NONE
    return NULL;
#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE
}

Rush::Window* RushPlatform_GetWindow(RushPlatformContext* context)
{
    return context->window;
}

const Rush::MouseState* RushPlatform_GetMouseState(RushPlatformContext* context)
{
    return &context->mouse;
}

const Rush::KeyboardState* RushPlatform_GetKeyboardState(RushPlatformContext* context)
{
    return &context->keybd;
}

// Platform-specific code

#ifdef RUSH_PLATFORM_WINDOWS
#include "WindowWin32.h"
static void fix_working_directory()
{
    // Set working directory to the location of the EXE

    char curr_filename[1024];	
    char fixed_directory[1024];

    GetModuleFileNameA(NULL, curr_filename, sizeof(curr_filename));

    size_t filename_len = Rush::rush_strlen(curr_filename);
    size_t slash_idx = (size_t)-1;
    for( size_t i=filename_len-1; i>0; --i )
    {
        if( curr_filename[i] == '/' || curr_filename[i] == '\\' )
        {
            slash_idx = i;
            break;
        }
    }
    if( slash_idx!=-1 )
    {
        memcpy(fixed_directory, curr_filename, slash_idx);
        fixed_directory[slash_idx] = 0;
        SetCurrentDirectoryA(fixed_directory);
    }
}

Rush::Window* RushPlatform_CreateWindow(RushPlatformContext* context)
{
    const RushAppConfig* cfg = &context->cfg;
    return new Rush::Win32Window(cfg->width, cfg->height, cfg->resizable, cfg->name, context);
}

int RushPlatform_Run(RushAppConfig* cfg)
{
    fix_working_directory();

    RushPlatformContext* context = RushPlatform_StartUp(cfg);

    while( context->window->is_closed()==false )
    {
		RushPlatform_Update(context);
		RushPlatform_Draw(context);

        MSG msg;
        while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    RushPlatform_ShutDown(context);

    return 0;
}

#endif //Windows






