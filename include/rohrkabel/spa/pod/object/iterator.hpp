#pragma once
#include "../prop.hpp"

#include <memory>

struct spa_pod_prop;
namespace pipewire::spa
{
    class pod_object_body;
    class pod_object_body_iterator
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~pod_object_body_iterator();

      public:
        pod_object_body_iterator(std::size_t, spa_pod_prop *, const pod_object_body &);

      public:
        pod_prop operator*();
        std::unique_ptr<pod_prop> operator->();
        pod_object_body_iterator &operator++();
        bool operator!=(const pod_object_body_iterator &);
    };
} // namespace pipewire::spa