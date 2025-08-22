#include "cocoa.desktop.impl.hpp"

#include "instantiate.hpp"

#include <saucer/cocoa.utils.hpp>

namespace saucer::modules
{
    using impl = desktop::impl;

    position impl::mouse_position() const // NOLINT(*-static)
    {
        const auto guard  = utils::autorelease_guard{};
        const auto [x, y] = NSEvent.mouseLocation;

        return {.x = static_cast<int>(x), .y = static_cast<int>(y)};
    }

    template <picker::type T>
    std::optional<picker::result_t<T>> impl::pick(picker::options opts)
    {
        const auto guard   = utils::autorelease_guard{};
        auto *const dialog = panel<T>::open();

        if (opts.initial)
        {
            [dialog setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:opts.initial->c_str()]]];
        }

        if constexpr (T == picker::type::files)
        {
            [dialog setAllowsMultipleSelection:YES];
        }
        else if constexpr (T == picker::type::folder)
        {
            [dialog setCanChooseFiles:NO];
            [dialog setCanChooseDirectories:YES];
        }

        auto *delegate = [[[PickerDelegate alloc] initWithOptions:std::move(opts)] autorelease];
        [dialog setDelegate:delegate];

        if ([dialog runModal] != NSModalResponseOK)
        {
            return std::nullopt;
        }

        if constexpr (T == picker::type::files)
        {
            return convert(dialog.URLs);
        }
        else
        {
            return convert(dialog.URL);
        }
    }

    void impl::open(const std::string &uri) // NOLINT(*-static)
    {
        const auto guard      = utils::autorelease_guard{};
        auto *const workspace = [NSWorkspace sharedWorkspace];
        auto *const str       = [NSString stringWithUTF8String:uri.c_str()];
        auto *const url       = fs::exists(uri) ? [NSURL fileURLWithPath:str] : [NSURL URLWithString:str];

        [workspace openURL:url];
    }

    SAUCER_INSTANTIATE_DESKTOP_PICKERS(SAUCER_INSTANTIATE_DESKTOP_IMPL_PICKER)
} // namespace saucer::modules
