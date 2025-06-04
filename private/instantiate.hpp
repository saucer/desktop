#pragma once

#define INSTANTIATE_PICKER                                                                                                  \
    template picker::result_t<picker::type::file> desktop::pick<picker::type::file>(const picker::options &);               \
    template picker::result_t<picker::type::files> desktop::pick<picker::type::files>(const picker::options &);             \
    template picker::result_t<picker::type::folder> desktop::pick<picker::type::folder>(const picker::options &);           \
    template picker::result_t<picker::type::save> desktop::pick<picker::type::save>(const picker::options &);
