#include "desktop.impl.hpp"

#include "instantiate.hpp"

#include <saucer/error.impl.hpp>

#include <QFileDialog>
#include <QDesktopServices>

namespace saucer::modules
{
    using impl = desktop::impl;

    static constexpr auto modes = std::make_tuple( //
        QFileDialog::ExistingFile,                 //
        QFileDialog::ExistingFiles,                //
        QFileDialog::Directory,                    //
        QFileDialog::AnyFile                       //
    );

    position impl::mouse_position() const // NOLINT(*-static)
    {
        const auto pos = QCursor::pos();
        return {.x = pos.x(), .y = pos.y()};
    }

    template <picker::type Type>
    result<picker::result_t<Type>> impl::pick(picker::options opts)
    {
        static constexpr auto mode = std::get<std::to_underlying(Type)>(modes);
        auto dialog                = QFileDialog{};

        dialog.setFileMode(mode);

        if (opts.initial)
        {
            dialog.setDirectory(QString::fromStdString(opts.initial.value()));
        }

        const auto filters = opts.filters | std::ranges::to<QStringList>();
        dialog.setNameFilters(filters);

        dialog.exec();

        auto result = dialog.selectedFiles() |                                                                        //
                      std::views::transform([]<typename T>(T &&str) { return std::forward<T>(str).toStdString(); }) | //
                      std::ranges::to<std::vector<fs::path>>();

        if (result.empty())
        {
            return err(std::errc::operation_canceled);
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

    void impl::open(const std::string &uri) // NOLINT(*-static)
    {
        QDesktopServices::openUrl(QString::fromStdString(uri));
    }

    SAUCER_INSTANTIATE_DESKTOP_PICKERS(SAUCER_INSTANTIATE_DESKTOP_IMPL_PICKER)
} // namespace saucer::modules
