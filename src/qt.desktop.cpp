#include "desktop.hpp"

#include "instantiate.hpp"

#include <tuple>
#include <ranges>

#include <QUrl>
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
            return m_parent->dispatch([this, uri] { return open(uri); }).get();
        }

        QDesktopServices::openUrl(QString::fromStdString(uri));
    }

    template <picker::type Type>
    picker::result_t<Type> desktop::pick(const picker::options &opts)
    {
        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this, opts] { return pick<Type>(opts); }).get();
        }

        QFileDialog dialog;

        static constexpr auto mode = std::get<std::to_underlying(Type)>(modes);
        dialog.setFileMode(mode);

        if (opts.initial)
        {
            dialog.setDirectory(opts.initial.value());
        }

        const auto filters = opts.filters | std::ranges::to<QStringList>();
        dialog.setNameFilters(filters);

        dialog.exec();

        const auto result = dialog.selectedFiles() |                                              //
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

    INSTANTIATE_PICKER();
} // namespace saucer::modules
