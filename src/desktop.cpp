#include "desktop.impl.hpp"

#include "instantiate.hpp"
#include <saucer/invoke.hpp>

namespace saucer::modules
{
    desktop::desktop(saucer::application *parent) : m_impl(std::make_unique<impl>())
    {
        m_impl->parent = parent;
    }

    desktop::~desktop() = default;

    position desktop::mouse_position() const
    {
        return invoke<&impl::mouse_position>(m_impl.get());
    }

    template <picker::type T>
    std::optional<picker::result_t<T>> desktop::pick(picker::options opts)
    {
        if (opts.filters.empty())
        {
            opts.filters.emplace("*");
        }

        return invoke<&impl::pick<T>>(m_impl.get(), std::move(opts));
    }

    void desktop::open(const std::string &uri)
    {
        return invoke<&impl::open>(m_impl.get(), uri);
    }

    SAUCER_INSTANTIATE_DESKTOP_PICKERS(SAUCER_INSTANTIATE_DESKTOP_PICKER)
} // namespace saucer::modules
