#pragma once

#include "utils/deleter.hpp"

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
        using mapped   = std::map<std::string, std::string>;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~properties();

      private:
        properties(deleter<raw_type>, raw_type *);

      public:
        properties(properties &&) noexcept;

      public:
        properties &operator=(properties &&) noexcept;

      public:
        void set(const std::string &key, const std::string &value);

      public:
        [[nodiscard]] mapped map() const;
        [[nodiscard]] std::string get(const std::string &key) const;

      public:
        [[nodiscard]] raw_type *get() const;

      public:
        [[nodiscard]] static properties create();
        [[nodiscard]] static properties create(const mapped &);

      public:
        [[nodiscard]] static properties from(raw_type *);
        [[nodiscard]] static properties view(raw_type *);

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;
    };
} // namespace pipewire
