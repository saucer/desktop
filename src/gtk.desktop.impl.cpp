#include "gtk.desktop.impl.hpp"

namespace saucer
{
    fs::path modules::convert(GFile *file)
    {
        return g_file_get_path(file);
    }

    std::vector<fs::path> modules::convert(GListModel *model)
    {
        const auto count = g_list_model_get_n_items(model);
        auto rtn         = std::vector<fs::path>{};

        rtn.reserve(count);

        for (auto i = 0u; count > i; ++i)
        {
            auto *const item = g_list_model_get_item(model, i);
            rtn.emplace_back(convert(G_FILE(item)));
        }

        return rtn;
    }
} // namespace saucer
