#pragma once

#include <saucer/app.hpp>
#include <saucer/window.hpp>

#include <filesystem>
#include <type_traits>

#include <cstdint>

#include <set>
#include <vector>

#include <string>
#include <optional>

namespace saucer::modules
{
    namespace fs = std::filesystem;

    namespace picker
    {
        enum class type : std::uint8_t
        {
            file,
            files,
            folder,
            save,
        };

        struct options
        {
            std::optional<fs::path> initial;
            std::set<std::string> filters{"*"};
        };

        template <type T>
        using result_t = std::optional<std::conditional_t<T == type::files, std::vector<fs::path>, fs::path>>;
    } // namespace picker

    struct screen
    {
        std::string id;

      public:
        std::pair<int, int> size;
        std::pair<int, int> position;
    };

    class desktop
    {
        saucer::application *m_parent;

      public:
        desktop(saucer::application *parent);

      public:
        void open(const std::string &);

      public:
        template <picker::type Type>
        [[nodiscard]] picker::result_t<Type> pick(const picker::options & = {});

      public:
        [[nodiscard]] std::vector<screen> screens() const;
        [[nodiscard]] std::optional<screen> screen_at(const window &) const;

      public:
        [[nodiscard]] std::pair<int, int> mouse_position() const;
    };
} // namespace saucer::modules
