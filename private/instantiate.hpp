#include <saucer/instantiate.hpp>

#define PICKER_INSTANTIATE_IMPL(N, ...)                                                                                     \
    template picker::result_t<static_cast<picker::type>(N)> desktop::pick<static_cast<picker::type>(N)>(                    \
        const picker::options &);

#define INSTANTIATE_PICKER() SAUCER_INSTANTIATE(4, PICKER_INSTANTIATE_IMPL, NULL)
