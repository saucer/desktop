#include "desktop.hpp"

#include "gtk.utils.hpp"
#include "instantiate.hpp"

namespace saucer::modules
{
    static constexpr auto dialogs = std::make_tuple(                                         //
        std::make_pair(gtk_file_dialog_open, gtk_file_dialog_open_finish),                   //
        std::make_pair(gtk_file_dialog_open_multiple, gtk_file_dialog_open_multiple_finish), //
        std::make_pair(gtk_file_dialog_select_folder, gtk_file_dialog_select_folder_finish), //
        std::make_pair(gtk_file_dialog_save, gtk_file_dialog_save_finish)                    //
    );

    void desktop::open(const std::string &uri)
    {
        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this, uri] { return open(uri); });
        }

        if (!fs::exists(uri))
        {
            auto launcher = utils::g_object_ptr<GtkUriLauncher>{gtk_uri_launcher_new(uri.c_str())};
            gtk_uri_launcher_launch(launcher.get(), nullptr, nullptr, nullptr, nullptr);
            return;
        }

        auto file     = utils::g_object_ptr<GFile>{g_file_new_for_path(uri.c_str())};
        auto launcher = utils::g_object_ptr<GtkFileLauncher>{gtk_file_launcher_new(file.get())};

        return gtk_file_launcher_launch(launcher.get(), nullptr, nullptr, nullptr, nullptr);
    }

    fs::path convert(GFile *file)
    {
        return g_file_get_path(file);
    }

    std::vector<fs::path> convert(GListModel *files)
    {
        const auto count = g_list_model_get_n_items(files);

        std::vector<fs::path> rtn;
        rtn.reserve(count);

        for (auto i = 0u; count > i; ++i)
        {
            auto *const file = reinterpret_cast<GFile *>(g_list_model_get_item(files, i));
            rtn.emplace_back(convert(file));
        }

        return rtn;
    }

    template <picker::type Type>
    picker::result_t<Type> desktop::pick(const picker::options &opts)
    {
        static constexpr auto open   = std::get<std::to_underlying(Type)>(dialogs).first;
        static constexpr auto finish = std::get<std::to_underlying(Type)>(dialogs).second;

        if (!m_parent->thread_safe())
        {
            return m_parent->dispatch([this, opts] { return pick<Type>(opts); });
        }

        auto dialog = utils::g_object_ptr<GtkFileDialog>{gtk_file_dialog_new()};

        if (opts.initial)
        {
            auto file = utils::g_object_ptr<GFile>{g_file_new_for_path(opts.initial->c_str())};

            if (fs::is_directory(opts.initial.value()))
            {
                gtk_file_dialog_set_initial_folder(dialog.get(), file.get());
            }
            else
            {
                gtk_file_dialog_set_initial_file(dialog.get(), file.get());
            }
        }

        auto filter = utils::g_object_ptr<GtkFileFilter>{gtk_file_filter_new()};

        for (const auto &pattern : opts.filters)
        {
            gtk_file_filter_add_pattern(filter.get(), pattern.c_str());
        }

        auto store = utils::g_object_ptr<GListStore>{g_list_store_new(gtk_file_filter_get_type())};

        g_list_store_append(store.get(), filter.get());
        gtk_file_dialog_set_filters(dialog.get(), G_LIST_MODEL(store.get()));

        auto promise = std::promise<picker::result_t<Type>>{};
        auto fut     = promise.get_future();

        auto callback = [](auto *dialog, auto *result, void *data)
        {
            auto *value = finish(GTK_FILE_DIALOG(dialog), result, nullptr);
            auto *res   = reinterpret_cast<decltype(promise) *>(data);

            if (!value)
            {
                res->set_value(std::nullopt);
                return;
            }

            res->set_value(convert(value));
        };

        open(dialog.get(), nullptr, nullptr, callback, &promise);

        while (fut.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
        {
            m_parent->run<false>();
        }

        return fut.get();
    }

    std::vector<screen> desktop::screens() const // NOLINT(*-static)
    {
        auto *const display = gdk_display_get_default();

        auto *const monitors = gdk_display_get_monitors(display);
        const auto size      = g_list_model_get_n_items(monitors);

        std::vector<screen> rtn{};
        rtn.reserve(size);

        for (auto i = 0ul; size > i; ++i)
        {
            auto *const current = reinterpret_cast<GdkMonitor *>(g_list_model_get_item(monitors, i));
            const auto *model   = gdk_monitor_get_model(current);

            GdkRectangle rect{};
            gdk_monitor_get_geometry(current, &rect);

            rtn.emplace_back(screen{
                .id       = model ? model : "",
                .size     = {rect.width, rect.height},
                .position = {rect.x, rect.y},
            });
        }

        return rtn;
    }

    std::pair<int, int> desktop::mouse_position() const // NOLINT(*-static)
    {
        return {};
    }

    INSTANTIATE_PICKER();
} // namespace saucer::modules
