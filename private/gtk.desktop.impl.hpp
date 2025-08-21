#pragma once

#include "desktop.impl.hpp"

#include <gtk/gtk.h>

namespace saucer::modules
{
    fs::path convert(GFile *);
    std::vector<fs::path> convert(GListModel *);
} // namespace saucer::modules
