#include "desktop.hpp"

namespace saucer::modules
{
    struct desktop::impl
    {
        application *parent;

      public:
        [[nodiscard]] position mouse_position() const;

      public:
        template <picker::type T>
        [[nodiscard]] std::optional<picker::result_t<T>> pick(picker::options);

      public:
        void open(const std::string &);
    };
} // namespace saucer::modules
