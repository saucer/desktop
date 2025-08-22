#include "win32.desktop.impl.hpp"

#include "instantiate.hpp"

#include <saucer/win32.utils.hpp>

namespace saucer::modules
{
    using impl = desktop::impl;

    using Microsoft::WRL::ComPtr;

    position impl::mouse_position() const // NOLINT(*-static)
    {
        POINT pos{};
        GetCursorPos(&pos);
        return {.x = pos.x, .y = pos.y};
    }

    template <picker::type Type>
    std::optional<picker::result_t<Type>> impl::pick(picker::options opts)
    {
        ComPtr<IFileOpenDialog> dialog;

        if (!SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog))))
        {
            return std::nullopt;
        }

        auto transform = [](auto &path)
        {
            auto rtn        = ComPtr<IShellItem>{};
            auto wide       = path.make_preferred().wstring();
            auto *const ppv = reinterpret_cast<void **>(rtn.GetAddressOf());

            SHCreateItemFromParsingName(wide.c_str(), nullptr, IID_IShellItem, ppv);

            return rtn;
        };

        auto initial = opts.initial.transform(transform);

        if (initial.has_value())
        {
            dialog->SetFolder(initial->Get());
        }

        auto allowed = opts.filters                                                          //
                       | std::views::transform([](auto &&str) { return utils::widen(str); }) //
                       | std::views::join_with(L';')                                         //
                       | std::ranges::to<std::wstring>();

        COMDLG_FILTERSPEC filters[] = {{L"Allowed Files", allowed.c_str()}};
        dialog->SetFileTypes(1, filters);

        FILEOPENDIALOGOPTIONS options{};
        dialog->GetOptions(&options);

        if constexpr (Type == picker::type::files)
        {
            dialog->SetOptions(options | FOS_ALLOWMULTISELECT);
        }
        else if constexpr (Type == picker::type::folder)
        {
            dialog->SetOptions(options | FOS_PICKFOLDERS);
        }
        else if constexpr (Type == picker::type::save)
        {
            dialog->SetOptions(options & ~FOS_PATHMUSTEXIST & ~FOS_FILEMUSTEXIST);
        }

        dialog->Show(nullptr);

        ComPtr<IShellItemArray> results;

        if (!SUCCEEDED(dialog->GetResults(&results)))
        {
            return std::nullopt;
        }

        auto rtn = convert(results.Get());

        if constexpr (Type == picker::type::files)
        {
            return rtn;
        }
        else
        {
            return rtn.transform([](auto &&result) { return result.front(); });
        }
    }

    void impl::open(const std::string &uri) // NOLINT(*-static)
    {
        ShellExecuteW(nullptr, L"open", utils::widen(uri).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }

    SAUCER_INSTANTIATE_DESKTOP_PICKERS(SAUCER_INSTANTIATE_DESKTOP_IMPL_PICKER);
} // namespace saucer::modules
