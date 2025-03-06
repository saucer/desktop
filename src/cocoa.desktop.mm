#include "cocoa.desktop.impl.hpp"

#include "cocoa.utils.hpp"
#include "instantiate.hpp"

#include <ranges>

#import <Cocoa/Cocoa.h>

namespace saucer::modules
{
    void desktop::open(const std::string &uri)
    {
        const utils::autorelease_guard guard{};

        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this, uri] { return open(uri); });
        }

        auto *const workspace = [NSWorkspace sharedWorkspace];
        auto *const str       = [NSString stringWithUTF8String:uri.c_str()];
        auto *const url       = fs::exists(uri) ? [NSURL fileURLWithPath:str] : [NSURL URLWithString:str];

        [workspace openURL:url];
    }

    template <picker::type Type>
    auto *make_panel()
    {
        if constexpr (Type == picker::type::save)
        {
            return [NSSavePanel savePanel];
        }
        else
        {
            return [NSOpenPanel openPanel];
        }
    }

    fs::path convert(const NSURL *file)
    {
        return file.absoluteString.UTF8String;
    }

    std::vector<fs::path> convert(const NSArray<NSURL *> *files)
    {
        const utils::autorelease_guard guard{};

        std::vector<fs::path> rtn;
        rtn.reserve(files.count);

        for (const NSURL *file : files)
        {
            rtn.emplace_back(convert(file));
        }

        return rtn;
    }

    template <picker::type Type>
    picker::result_t<Type> desktop::pick(const picker::options &opts)
    {
        const utils::autorelease_guard guard{};

        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this, opts] { return pick<Type>(opts); });
        }

        auto *const panel = make_panel<Type>();

        if (opts.initial)
        {
            [panel setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:opts.initial->c_str()]]];
        }

        if constexpr (Type == picker::type::files)
        {
            [panel setAllowsMultipleSelection:YES];
        }
        else if constexpr (Type == picker::type::folder)
        {
            [panel setCanChooseFiles:NO];
            [panel setCanChooseDirectories:YES];
        }

        auto *delegate = [[[PickerDelegate alloc] initWithOptions:&opts] autorelease];
        [panel setDelegate:delegate];

        if ([panel runModal] != NSModalResponseOK)
        {
            return std::nullopt;
        }

        if constexpr (Type == picker::type::files)
        {
            return convert(panel.URLs);
        }
        else
        {
            return convert(panel.URL);
        }
    }

    std::vector<screen> desktop::screens() const
    {
        const utils::autorelease_guard guard{};

        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this] { return screens(); });
        }

        auto *const screens = [NSScreen screens];

        std::vector<screen> rtn{};
        rtn.reserve(screens.count);

        for (const NSScreen *entry : screens)
        {
            rtn.emplace_back(screen{
                .id       = entry.localizedName.UTF8String,
                .size     = {entry.frame.size.width, entry.frame.size.height},
                .position = {entry.frame.origin.x, entry.frame.origin.y},
            });
        }

        return rtn;
    }

    std::pair<int, int> desktop::mouse_position() const
    {
        const utils::autorelease_guard guard{};

        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this] { return mouse_position(); });
        }

        const auto [x, y] = NSEvent.mouseLocation;
        return {x, y};
    }

    INSTANTIATE_PICKER();
} // namespace saucer::modules
