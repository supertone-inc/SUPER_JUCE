// ToolbarManager.h
#import <Cocoa/Cocoa.h>

@interface ToolbarManager : NSObject <NSToolbarDelegate>

- (void)setupToolbarForWindow:(NSWindow *)window;

@end
