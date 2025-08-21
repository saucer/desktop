#pragma once

#include <saucer/app.hpp>
#include <saucer/window.hpp>

#include <cstdint>
#include <filesystem>

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

        template <type>
        struct result
        {
            using type = fs::path;
        };

        template <>
        struct result<type::files>
        {
            using type = std::vector<fs::path>;
        };

        template <type T>
        using result_t = result<T>::type;
    } // namespace picker

    struct desktop
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        desktop(saucer::application *parent);

      public:
        ~desktop();

      public:
        [[nodiscard]] position mouse_position() const;

      public:
        template <picker::type T>
        [[nodiscard]] std::optional<picker::result_t<T>> pick(picker::options = {});

      public:
        void open(const std::string &);
    };
} // namespace saucer::modules
