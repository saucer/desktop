#include "win32.desktop.impl.hpp"

#include <saucer/win32.utils.hpp>
#include <saucer/win32.error.hpp>

namespace saucer
{
    result<ComPtr<IShellItem>> modules::make_shell_item(fs::path file)
    {
        auto rtn        = ComPtr<IShellItem>{};
        const auto path = file.make_preferred().wstring();

        if (auto status = SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&rtn)); !SUCCEEDED(status))
        {
            return err(status);
        }

        return rtn;
    }

    result<std::vector<fs::path>> modules::convert(IShellItemArray *files)
    {
        DWORD count{};

        if (auto status = files->GetCount(&count); !SUCCEEDED(status))
        {
            return err(status);
        }

        std::vector<fs::path> rtn;
        rtn.reserve(count);

        for (auto i = 0; count > i; ++i)
        {
            ComPtr<IShellItem> item;

            if (auto status = files->GetItemAt(i, &item); !SUCCEEDED(status))
            {
                return err(status);
            }

            utils::string_handle path;

            if (auto status = item->GetDisplayName(SIGDN_FILESYSPATH, &path.reset()); !SUCCEEDED(status))
            {
                return err(status);
            }

            rtn.emplace_back(path.get());
        }

        return rtn;
    }
} // namespace saucer
