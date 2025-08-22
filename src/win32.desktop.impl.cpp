#include "win32.desktop.impl.hpp"

#include <saucer/win32.utils.hpp>

namespace saucer
{
    std::optional<std::vector<fs::path>> modules::convert(IShellItemArray *files)
    {
        DWORD count{};

        if (!SUCCEEDED(files->GetCount(&count)))
        {
            return std::nullopt;
        }

        std::vector<fs::path> rtn;
        rtn.reserve(count);

        for (auto i = 0; count > i; ++i)
        {
            ComPtr<IShellItem> item;

            if (!SUCCEEDED(files->GetItemAt(i, &item)))
            {
                continue;
            }

            utils::string_handle path;

            if (!SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path.reset())))
            {
                continue;
            }

            rtn.emplace_back(path.get());
        }

        return rtn;
    }
} // namespace saucer
