#pragma once

#include <saucer/instantiate.hpp>

#define SAUCER_INSTANTIATE_PICKER(CLASS, TYPE)                                                                              \
    template std::optional<picker::result_t<TYPE>> CLASS::pick<TYPE>(picker::options);

#define SAUCER_INSTANTIATE_DESKTOP_PICKER(TYPE) SAUCER_INSTANTIATE_PICKER(desktop, TYPE)
#define SAUCER_INSTANTIATE_DESKTOP_IMPL_PICKER(TYPE) SAUCER_INSTANTIATE_PICKER(desktop::impl, TYPE)

#define SAUCER_INSTANTIATE_DESKTOP_PICKERS(MACRO)                                                                           \
    SAUCER_RECURSE(MACRO, picker::type::file, picker::type::files, picker::type::folder, picker::type::save)
