#include "Rush.h"

#ifdef RUSH_PLATFORM_MACOS

#import <AppKit/AppKit.h>
#import <CoreVideo/CVDisplayLink.h>

#include "Platform.h"

@interface RushOpenGLView : NSOpenGLView
{
    CVDisplayLinkRef displayLink;
    RushPlatformContext* gameContext;
}
@end

#endif //RUSH_PLATFORM_OSX
