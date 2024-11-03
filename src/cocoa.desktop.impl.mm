#include "cocoa.desktop.impl.hpp"

#include <ranges>
#include <algorithm>

#include <fnmatch.h>

@implementation PickerDelegate
- (instancetype)initWithOptions:(const saucer::modules::picker::options *)options
{
    self            = [super init];
    self->m_options = options;

    return self;
}

- (BOOL)panel:(id)sender shouldEnableURL:(NSURL *)url
{
    const auto *name = url.lastPathComponent.UTF8String;

    if (url.hasDirectoryPath)
    {
        return YES;
    }

    return std::ranges::any_of(m_options->filters,
                               [name](auto &&filter) { return fnmatch(filter.c_str(), name, FNM_PATHNAME) == 0; });
}
@end
