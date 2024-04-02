// ToolbarManager.mm
#import "ToolbarManager.h"

@implementation ToolbarManager

- (void)setupToolbarForWindow:(NSWindow *)window {
    NSToolbar *toolbar = [[NSToolbar alloc] initWithIdentifier:@"com.yourcompany.toolbarIdentifier"];
    toolbar.delegate = self;
    [window setToolbar:toolbar];
}

// Implement NSToolbarDelegate methods here
- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSToolbarItemIdentifier)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag {
    NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier];

    if ([itemIdentifier isEqualToString:@"yourToolbarItemIdentifier"]) {
        // Configure the toolbar item
        item.label = @"Item Label";
        item.paletteLabel = @"Item Label";
        item.toolTip = @"Tooltip";
        item.target = self;
        // Set the action to a selector implemented in this class
        item.action = @selector(toolbarItemAction:);
        // Optionally set the image for the item
        item.image = [NSImage imageNamed:NSImageNameInfo];
    }

    return item;
}

- (NSArray<NSToolbarItemIdentifier> *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar {
    return @[@"yourToolbarItemIdentifier"];
}

- (NSArray<NSToolbarItemIdentifier> *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar {
    return @[@"yourToolbarItemIdentifier"];
}

// Example action method for toolbar items
- (void)toolbarItemAction:(id)sender {
    // Handle the action
    NSLog(@"Toolbar item clicked!");
}

@end
