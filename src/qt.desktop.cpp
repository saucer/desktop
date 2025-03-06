#include "desktop.hpp"

#include "instantiate.hpp"

#include "qt.app.impl.hpp"
#include "qt.window.impl.hpp"

#include <tuple>
#include <ranges>

#include <QUrl>
#include <QScreen>
#include <QFileDialog>
#include <QDesktopServices>

namespace saucer::modules
{
    static constexpr auto modes = std::make_tuple( //
        QFileDialog::ExistingFile,                 //
        QFileDialog::ExistingFiles,                //
        QFileDialog::Directory,                    //
        QFileDialog::AnyFile                       //
    );

    void desktop::open(const std::string &uri)
    {
        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this, uri] { return open(uri); });
        }

        QDesktopServices::openUrl(QString::fromStdString(uri));
    }

    template <picker::type Type>
    picker::result_t<Type> desktop::pick(const picker::options &opts)
    {
        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this, opts] { return pick<Type>(opts); });
        }

        QFileDialog dialog;

        static constexpr auto mode = std::get<std::to_underlying(Type)>(modes);
        dialog.setFileMode(mode);

        if (opts.initial)
        {
            dialog.setDirectory(QString::fromStdString(opts.initial.value()));
        }

        const auto filters = opts.filters | std::ranges::to<QStringList>();
        dialog.setNameFilters(filters);

        dialog.exec();

        auto result = dialog.selectedFiles() |                                              //
                      std::views::transform([](auto &&str) { return str.toStdString(); }) | //
                      std::ranges::to<std::vector<fs::path>>();

        if (result.empty())
        {
            return std::nullopt;
        }

        if constexpr (Type == picker::type::files)
        {
            return result;
        }
        else
        {
            return result.front();
        }
    }

    screen convert(QScreen *screen)
    {
        const auto geometry = screen->geometry();

        return {
            .id       = screen->name().toStdString(),
            .size     = {geometry.width(), geometry.height()},
            .position = {geometry.x(), geometry.y()},
        };
    }

    std::vector<screen> desktop::screens() const
    {
        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this] { return screens(); });
        }

        const auto &app    = m_parent->native<false>()->application;
        const auto screens = app->screens();

        std::vector<screen> rtn{};
        rtn.reserve(screens.size());

        for (const auto &entry : screens)
        {
            rtn.emplace_back(convert(entry));
        }

        return rtn;
    }

    std::optional<screen> desktop::screen_at(const window &window) const
    {
        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this, &window] { return screen_at(window); });
        }

        auto *const screen = window.native<false>()->window->screen();

        if (!screen)
        {
            return std::nullopt;
        }

        return convert(screen);
    }

    std::pair<int, int> desktop::mouse_position() const
    {
        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this] { return mouse_position(); });
        }

        const auto [x, y] = QCursor::pos();
        return {x, y};
    }

    INSTANTIATE_PICKER();
} // namespace saucer::modules
