#pragma once

#include "desktop.impl.hpp"

#import <Cocoa/Cocoa.h>

@class PickerDelegate;

namespace saucer::modules
{
    template <picker::type>
    struct panel
    {
        static NSOpenPanel *open();
    };

    template <>
    struct panel<picker::type::save>
    {
        static NSSavePanel *open();
    };

    fs::path convert(const NSURL *);
    std::vector<fs::path> convert(const NSArray<NSURL *> *);
} // namespace saucer::modules

@interface PickerDelegate : NSObject <NSOpenSavePanelDelegate>
{
  @public
    saucer::modules::picker::options options;
}
- (instancetype)initWithOptions:(saucer::modules::picker::options)opts;
- (BOOL)panel:(id)sender shouldEnableURL:(NSURL *)url;
@end
