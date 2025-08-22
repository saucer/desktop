#pragma once

#include "desktop.impl.hpp"

#include <windows.h>
#include <shlobj.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace saucer::modules
{
    result<ComPtr<IShellItem>> make_shell_item(fs::path);
    result<std::vector<fs::path>> convert(IShellItemArray *);
} // namespace saucer::modules
