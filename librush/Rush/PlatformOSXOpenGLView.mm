#include "Rush.h"

#ifdef RUSH_PLATFORM_MACOS

#import "PlatformOSXOpenGLView.h"

#include <OpenGL/gl.h>

#ifndef __clang__
#define __bridge
#endif //__clang__

void RushPlatformOSX_SetConfigPtr(RushAppConfig* cfg);

static RushAppConfig* s_appConfig = NULL; 
void RushPlatformOSX_SetConfigPtr(RushAppConfig* cfg)
{
    s_appConfig = cfg;
}

@implementation RushOpenGLView

- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
    @autoreleasepool
    {        
        RushPlatform_Update(gameContext);

        NSOpenGLContext    *currentContext = [self openGLContext];
        [currentContext makeCurrentContext];
        
        // must lock GL context because display link is threaded
        CGLLockContext((CGLContextObj)[currentContext CGLContextObj]);
        
        // Add your drawing codes here

        RushPlatform_Draw(gameContext);        

        glFlush();
        
        [currentContext flushBuffer];
        
        CGLUnlockContext((CGLContextObj)[currentContext CGLContextObj]);
    }
    return kCVReturnSuccess;
}

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    CVReturn result = [(__bridge RushOpenGLView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (void) prepareOpenGL
{    
    s_appConfig->width  = [self frame].size.width;
    s_appConfig->height = [self frame].size.height;    
    
    gameContext = RushPlatform_StartUp(s_appConfig);
        
    // Init display link
    
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    // Create a display link capable of being used with all active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    // Set the renderer output callback function
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, (__bridge void*)self);
    
    // Set the display link for the current renderer        
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    // Activate the display link
    CVDisplayLinkStart(displayLink);    
}

- (void)dealloc
{
    // Release the display link
    CVDisplayLinkRelease(displayLink);    
    RushPlatform_ShutDown(gameContext);
    //[super dealloc];
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)becomeFirstResponder
{
    return  YES;
}

- (BOOL)resignFirstResponder
{
    return YES;
}

- (void)keyDown:(NSEvent *)theEvent
{
    NSString *characters = [theEvent characters];
    if ([characters length]) 
    {
        unichar character = [characters characterAtIndex:0];
		RushPlatform_KeyDown(gameContext, (unsigned char)character);
	}
}

- (void)keyUp:(NSEvent *)theEvent
{
    NSString *characters = [theEvent characters];
    if ([characters length]) 
    {
        unichar character = [characters characterAtIndex:0];
		RushPlatform_KeyUp(gameContext, (unsigned char)character);
	}
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	float wheelDelta = [theEvent deltaX] +[theEvent deltaY] + [theEvent deltaZ];
	if (wheelDelta)
	{
        // TODO
	}
}

- (void)mouseDown:(NSEvent *)theEvent
{
    NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    RushPlatform_MouseDown(gameContext, 0, (int)location.x, int([self frame].size.height-location.y));    
}

- (void)mouseUp:(NSEvent *)theEvent
{
    NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    RushPlatform_MouseUp(gameContext, 0, (int)location.x, int([self frame].size.height-location.y));
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    RushPlatform_MouseMove(gameContext, (int)location.x, int([self frame].size.height-location.y));
}

- (void)mouseDragged:(NSEvent *)theEvent
{    
    NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    RushPlatform_MouseMove(gameContext, (int)location.x, int([self frame].size.height-location.y));
}

- (void)mouseEntered:(NSEvent *)theEvent
{
    // TODO
}

- (void) mouseExited:(NSEvent *)theEvent
{
    // TODO
}

@end

#endif //RUSH_PLATFORM_OSX

