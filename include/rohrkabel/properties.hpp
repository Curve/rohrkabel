#pragma once
#include <map>
#include <memory>
#include <string>
#include <initializer_list>

struct pw_properties;
namespace pipewire
{
    class properties
    {
        struct impl;
        using map_t = std::map<std::string, std::string>;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~properties();

      public:
        properties();
        properties(map_t &&);
        properties(const map_t &);
        properties(std::initializer_list<std::pair<const std::string, std::string>> &&);

      public:
        properties(properties &&) noexcept;

      public:
        void set(const std::string &key, const std::string &value);
        [[nodiscard]] std::string get(const std::string &value) const;

      public:
        [[nodiscard]] pw_properties *get() const;

      public:
        map_t::const_iterator end() const;
        map_t::const_iterator begin() const;
    };
} // namespace pipewire