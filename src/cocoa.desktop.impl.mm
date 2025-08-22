#include "cocoa.desktop.impl.hpp"

#include <saucer/cocoa.utils.hpp>

#include <ranges>
#include <algorithm>

#include <fnmatch.h>

namespace saucer
{
    template <modules::picker::type T>
    NSOpenPanel *modules::panel<T>::open()
    {
        return [NSOpenPanel openPanel];
    };

    NSSavePanel *modules::panel<modules::picker::type::save>::open()
    {
        return [NSSavePanel savePanel];
    }

    fs::path modules::convert(const NSURL *file)
    {
        const utils::autorelease_guard guard{};
        return file.absoluteString.UTF8String;
    }

    std::vector<fs::path> modules::convert(const NSArray<NSURL *> *files)
    {
        const auto guard = utils::autorelease_guard{};
        auto rtn         = std::vector<fs::path>{};

        rtn.reserve(files.count);

        for (NSUInteger i = 0; files.count > i; ++i)
        {
            auto *const file = reinterpret_cast<NSURL *>([files objectAtIndex:i]);
            rtn.emplace_back(convert(file));
        }

        return rtn;
    }
} // namespace saucer

@implementation PickerDelegate
- (instancetype)initWithOptions:(saucer::modules::picker::options)opts
{
    self          = [super init];
    self->options = std::move(opts);

    return self;
}

- (BOOL)panel:(id)sender shouldEnableURL:(NSURL *)url
{
    const auto *name = url.lastPathComponent.UTF8String;

    if (url.hasDirectoryPath)
    {
        return YES;
    }

    auto match = [name]<typename T>(T &&filter)
    {
        return fnmatch(std::forward<T>(filter).c_str(), name, FNM_PATHNAME) == 0;
    };

    return static_cast<BOOL>(std::ranges::any_of(options.filters, match));
}
@end
