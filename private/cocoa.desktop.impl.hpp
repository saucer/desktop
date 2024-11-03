#pragma once

#include "desktop.hpp"

#import <Cocoa/Cocoa.h>

@class PickerDelegate;

@interface PickerDelegate : NSObject <NSOpenSavePanelDelegate>
{
  @public
    const saucer::modules::picker::options *m_options;
}
- (instancetype)initWithOptions:(const saucer::modules::picker::options *)options;
- (BOOL)panel:(id)sender shouldEnableURL:(NSURL *)url;
@end
