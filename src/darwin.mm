#include <nanogui/nanogui.h>
#import <Cocoa/Cocoa.h>

NAMESPACE_BEGIN(nanogui)

std::string file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes, bool save) {
    std::string path = "";
    if (save) {
        NSSavePanel *saveDlg = [NSSavePanel savePanel];

        NSMutableArray *types = [NSMutableArray new];
        for (size_t idx = 0; idx < filetypes.size(); ++idx)
            [types addObject: [NSString stringWithUTF8String: filetypes[idx].first.c_str()]];

        [saveDlg setAllowedFileTypes: types];

        if ([saveDlg runModal] == NSModalResponseOK)
            path = [[[saveDlg URL] path] UTF8String];
    } else {
        NSOpenPanel *openDlg = [NSOpenPanel openPanel];

        [openDlg setCanChooseFiles:YES];
        [openDlg setCanChooseDirectories:NO];
        [openDlg setAllowsMultipleSelection:NO];
        NSMutableArray *types = [NSMutableArray new];
        for (size_t idx = 0; idx < filetypes.size(); ++idx)
            [types addObject: [NSString stringWithUTF8String: filetypes[idx].first.c_str()]];

        [openDlg setAllowedFileTypes: types];

        if ([openDlg runModal] == NSModalResponseOK) {
            for (NSURL* url in [openDlg URLs]) {
                path = std::string((char*) [[url path] UTF8String]);
                break;
            }
        }
    }
    return path;
}

void chdir_to_bundle_parent() {
    NSString *path = [[[NSBundle mainBundle] bundlePath] stringByDeletingLastPathComponent];
    chdir([path fileSystemRepresentation]);
}

void disable_saved_application_state_osx() {
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSQuitAlwaysKeepsWindows"];
}

NAMESPACE_END(nanogui)
