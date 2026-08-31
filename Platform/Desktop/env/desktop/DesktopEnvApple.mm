#import <AppKit/AppKit.h>

#include "env/Env.h"
#include "nsLib/log.h"

void nsEnv::MessagePopup(const char *title, const char *message) {
    @autoreleasepool {
        auto *alert = [[NSAlert alloc] init];
        alert.messageText = [NSString stringWithUTF8String:title ? title : ""];
        alert.informativeText = [NSString stringWithUTF8String:message ? message : ""];
        [alert addButtonWithTitle:@"OK"];
        [alert runModal];
#if !__has_feature(objc_arc)
        [alert release];
#endif
    }
}

void nsEnv::OpenUrl(const char *url) {
    if (!url || !url[0]) {
        Log::Warning("Can't open empty URL");
        return;
    }

    @autoreleasepool {
        auto *string = [NSString stringWithUTF8String:url];
        auto *target = [NSURL URLWithString:string];
        if (!target || ![[NSWorkspace sharedWorkspace] openURL:target]) {
            Log::Warning("Can't open URL: %s", url);
        }
    }
}
