#ifndef __RUSH_SYSTEM_PLATFORM_H__
#define __RUSH_SYSTEM_PLATFORM_H__
#ifdef __cplusplus

namespace Rush
{
    class RenderDevice;
    class RenderContext;
    class Window;
    class MouseState;
    class KeyboardState;
}

extern "C" {
#endif// __cplusplus
    
    typedef struct RushPlatformContext RushPlatformContext;

    typedef void(*RushPlatformCallback_Startup)(RushPlatformContext* context);
    typedef void(*RushPlatformCallback_Update)(RushPlatformContext* context);
    typedef void(*RushPlatformCallback_Draw)(RushPlatformContext* context);
    typedef void(*RushPlatformCallback_Shutdown)(RushPlatformContext* context);


    typedef struct RushAppConfig
    {
        const char* name;

        int		vsync;

        int		width;
        int		height;

        int     max_width;
        int     max_height;

        bool    full_screen;
        bool    resizable;
        bool    nvperfhud;

        int     argc;
        char**  argv;

        RushPlatformCallback_Startup    on_startup;
        RushPlatformCallback_Update     on_update;    
        RushPlatformCallback_Draw       on_draw;
        RushPlatformCallback_Shutdown   on_shutdown;
    } RushAppConfig;


    typedef enum RushAppResolution
    {
        RushAppResolution_HDTV_720p,
        RushAppResolution_HDTV_1080p,
        RushAppResolution_iPad_Landscape,
        RushAppResolution_iPad_Portrait,
        RushAppResolution_iPhone_Landscape,
        RushAppResolution_iPhone_Portrait,
        RushAppResolution_iPhone4_Landscape,
        RushAppResolution_iPhone4_Portrait
    } RushAppResolution;

    void RushAppConfig_Init(RushAppConfig* cfg);
    void RushAppConfig_SetResolution(RushAppConfig* cfg, RushAppResolution res);

    int RushPlatform_Run(RushAppConfig* cfg);

    RushPlatformContext* RushPlatform_StartUp(RushAppConfig* cfg);
    void RushPlatform_ShutDown(RushPlatformContext* context);
    void RushPlatform_Update(RushPlatformContext* context);
    void RushPlatform_Draw(RushPlatformContext* context);
    const RushAppConfig* RushPlatform_GetConfig(RushPlatformContext* context);    
    
    void* RushPlatform_GetUserData(RushPlatformContext* context);
    void  RushPlatform_SetUserData(RushPlatformContext* context, void* data);

    void RushPlatform_KeyDown(RushPlatformContext* context, unsigned char code);
    void RushPlatform_KeyUp(RushPlatformContext* context, unsigned char code);
    void RushPlatform_MouseDown(RushPlatformContext* context, int button, float x, float y);
    void RushPlatform_MouseUp(RushPlatformContext* context, int button, float x, float y);
    void RushPlatform_MouseMove(RushPlatformContext* context, float x, float y);
    void RushPlatform_MouseWheel(RushPlatformContext* context, int h, int v);
    bool RushPlatform_GetKeyState(RushPlatformContext* context, unsigned char code);
    void RushPlatform_GetMousePos(RushPlatformContext* context, float* x, float* y);
    bool RushPlatform_GetMouseButtonState(RushPlatformContext* context, unsigned char button);

#ifdef __cplusplus
    
    Rush::RenderDevice*     RushPlatform_GetRenderDevice(RushPlatformContext* context);
    Rush::RenderContext*    RushPlatform_GetRenderContext(RushPlatformContext* context);
    Rush::Window*           RushPlatform_CreateWindow(RushPlatformContext* context);
    Rush::Window*           RushPlatform_GetWindow(RushPlatformContext* context);
    
    const Rush::MouseState*     RushPlatform_GetMouseState(RushPlatformContext* context);
    const Rush::KeyboardState*  RushPlatform_GetKeyboardState(RushPlatformContext* context);
}
#endif// __cplusplus        
#endif // __RUSH_SYSTEM_PLATFORM_H__

