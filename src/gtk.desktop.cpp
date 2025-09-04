#include "gtk.desktop.impl.hpp"

#include "instantiate.hpp"

#include <saucer/gtk.utils.hpp>
#include <saucer/error.impl.hpp>
#include <saucer/gtk.app.impl.hpp>

namespace saucer::modules
{
    using impl = desktop::impl;

    static constexpr auto dialogs = std::make_tuple(                                         //
        std::make_pair(gtk_file_dialog_open, gtk_file_dialog_open_finish),                   //
        std::make_pair(gtk_file_dialog_open_multiple, gtk_file_dialog_open_multiple_finish), //
        std::make_pair(gtk_file_dialog_select_folder, gtk_file_dialog_select_folder_finish), //
        std::make_pair(gtk_file_dialog_save, gtk_file_dialog_save_finish)                    //
    );

    position impl::mouse_position() const // NOLINT(*-static)
    {
        return {};
    }

    template <picker::type T>
    result<picker::result_t<T>> impl::pick(picker::options opts)
    {
        static constexpr auto open   = std::get<std::to_underlying(T)>(dialogs).first;
        static constexpr auto finish = std::get<std::to_underlying(T)>(dialogs).second;

        auto dialog = utils::g_object_ptr<GtkFileDialog>{gtk_file_dialog_new()};
        auto ec     = std::error_code{};

        if (opts.initial.has_value())
        {
            auto file               = utils::g_object_ptr<GFile>{g_file_new_for_path(opts.initial->c_str())};
            const auto is_directory = fs::is_directory(opts.initial.value(), ec);
            auto *const set_initial = is_directory ? gtk_file_dialog_set_initial_folder : gtk_file_dialog_set_initial_file;

            set_initial(dialog.get(), file.get());
        }

        auto filter = utils::g_object_ptr<GtkFileFilter>{gtk_file_filter_new()};
        auto store  = utils::g_object_ptr<GListStore>{g_list_store_new(gtk_file_filter_get_type())};

        for (const auto &pattern : opts.filters)
        {
            gtk_file_filter_add_pattern(filter.get(), pattern.c_str());
        }

        g_list_store_append(store.get(), filter.get());
        gtk_file_dialog_set_filters(dialog.get(), G_LIST_MODEL(store.get()));

        auto userdata     = std::tuple<result<picker::result_t<T>>, bool>{};
        auto &[rtn, done] = userdata;

        auto callback = [](auto *dialog, auto *res, void *data)
        {
            auto *value          = finish(GTK_FILE_DIALOG(dialog), res, nullptr);
            auto &[result, done] = *reinterpret_cast<decltype(userdata) *>(data);

            if (value)
            {
                result = convert(value);
            }
            else
            {
                result = err(std::errc::operation_canceled);
            }

            done = true;
        };
        open(dialog.get(), nullptr, nullptr, callback, &userdata);

        while (!done)
        {
            parent->native<false>()->platform->iteration();
        }

        return rtn;
    }

    void impl::open(const std::string &uri) // NOLINT(*-static)
    {
        std::error_code ec{};

        if (!fs::exists(uri, ec))
        {
            auto launcher = utils::g_object_ptr<GtkUriLauncher>{gtk_uri_launcher_new(uri.c_str())};
            gtk_uri_launcher_launch(launcher.get(), nullptr, nullptr, nullptr, nullptr);
            return;
        }

        auto file     = utils::g_object_ptr<GFile>{g_file_new_for_path(uri.c_str())};
        auto launcher = utils::g_object_ptr<GtkFileLauncher>{gtk_file_launcher_new(file.get())};

        return gtk_file_launcher_launch(launcher.get(), nullptr, nullptr, nullptr, nullptr);
    }

    SAUCER_INSTANTIATE_DESKTOP_PICKERS(SAUCER_INSTANTIATE_DESKTOP_IMPL_PICKER)
} // namespace saucer::modules
