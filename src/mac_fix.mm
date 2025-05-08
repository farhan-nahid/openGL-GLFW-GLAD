#ifdef __APPLE__
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

// Make this function externally visible from C++
extern "C" {
void macOSVersionCheck() {
    // Get macOS version
    NSOperatingSystemVersion version = [[NSProcessInfo processInfo] operatingSystemVersion];
    
    if (version.majorVersion >= 13) { // macOS 13 (Ventura) or later
        // Set startup OpenGL working directory
        // This helps with some modern macOS OpenGL compatibility issues
        NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
        [[NSFileManager defaultManager] changeCurrentDirectoryPath:resourcePath];
        
        // Also print warning about deprecated OpenGL
        NSLog(@"Warning: This application uses OpenGL which is deprecated in macOS 10.14 and later.");
    }
}
}
#endif