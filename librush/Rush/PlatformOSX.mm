#include "Rush.h"
#include "Platform.h"

#ifdef RUSH_PLATFORM_MACOS

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

extern void RushPlatformOSX_SetConfigPtr(RushAppConfig* cfg);

int RushPlatform_Run(RushAppConfig* cfg)
{
    RushPlatformOSX_SetConfigPtr(cfg);
    return NSApplicationMain(cfg->argc, (const char**)cfg->argv);
}

#endif //RUSH_PLATFORM_MACOS


