#pragma once

#include <saucer/app.hpp>

#include <filesystem>
#include <type_traits>

#include <set>
#include <optional>

namespace saucer::modules
{
    namespace fs = std::filesystem;

    namespace picker
    {
        enum class type
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
    };
} // namespace saucer::modules
