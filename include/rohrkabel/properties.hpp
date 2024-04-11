#pragma once

#include <map>
#include <string>
#include <memory>

struct pw_properties;

namespace pipewire
{
    class properties
    {
        struct impl;

      public:
        using raw_type = pw_properties;

      private:
        using underlying = std::map<std::string, std::string>;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~properties();

      public:
        properties();
        properties(properties &&) noexcept;

      public:
        properties(underlying);
        properties(std::initializer_list<std::pair<const std::string, std::string>>);

      public:
        void set(std::string key, std::string value);

      public:
        [[nodiscard]] std::string get(const std::string &value) const;

      public:
        [[nodiscard]] underlying::const_iterator end() const;
        [[nodiscard]] underlying::const_iterator begin() const;

      public:
        [[nodiscard]] raw_type *get() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;
    };
} // namespace pipewire
