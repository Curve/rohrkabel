#pragma once
#include "body.hpp"

#include <memory>
#include <cstddef>
#include <iterator>

struct spa_pod_prop;

namespace pipewire::spa
{
    class pod_object_body::sentinel
    {
    };

    class pod_object_body::iterator : public std::forward_iterator_tag
    {
        struct impl;

      public:
        using value_type      = pod_prop;
        using difference_type = std::ptrdiff_t;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~iterator();

      public:
        iterator();
        iterator(const iterator &);
        iterator(const pod_object_body *);

      public:
        iterator &operator=(const iterator &);

      public:
        iterator &operator++();
        iterator operator++(int) const &;

      public:
        pod_prop operator*() const;

      public:
        bool operator==(const iterator &) const;
        bool operator==(const sentinel &) const;
    };
} // namespace pipewire::spa
