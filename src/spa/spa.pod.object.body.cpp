#include "spa/pod/object/body.hpp"
#include "spa/pod/object/iterator.hpp"

#include <format>
#include <stdexcept>
#include <algorithm>

#include <spa/pod/pod.h>
#include <spa/pod/iter.h>
#include <spa/debug/pod.h>

namespace pipewire::spa
{
    struct pod_object_body::impl
    {
        std::size_t size;
        spa_pod_object_body *body;
    };

    pod_object_body::~pod_object_body() = default;

    pod_object_body::pod_object_body() : m_impl(std::make_unique<impl>()) {}

    pod_object_body::pod_object_body(pod_object_body &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    pod_object_body::pod_object_body(const pod_object_body &other) noexcept : pod_object_body()
    {
        *m_impl = *other.m_impl;
    }

    pod_object_body &pod_object_body::operator=(pod_object_body &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    pod_object_body &pod_object_body::operator=(const pod_object_body &other) noexcept
    {
        if (&other != this)
        {
            *m_impl = *other.m_impl;
        }

        return *this;
    }

    bool pod_object_body::has(std::uint32_t key) const
    {
        return std::ranges::any_of(*this, [&](const auto &item) {
            return item.key() == key;
        });
    }

    pod_prop pod_object_body::at(std::uint32_t key) const
    {
        for (auto item : *this)
        {
            if (item.key() != key)
            {
                continue;
            }

            return item;
        }

        [[unlikely]] throw std::out_of_range(std::format("{} does not exist", key));
    }

    std::size_t pod_object_body::size() const
    {
        return m_impl->size;
    }

    // NOLINTNEXTLINE(*-static)
    pod_object_body::sentinel pod_object_body::end() const
    {
        return {};
    }

    pod_object_body::iterator pod_object_body::begin() const
    {
        return {this};
    }

    pod_type pod_object_body::type() const
    {
        return static_cast<pod_type>(m_impl->body->type);
    }

    std::uint32_t pod_object_body::id() const
    {
        return m_impl->body->id;
    }

    spa_pod_object_body *pod_object_body::get() const
    {
        return m_impl->body;
    }

    const spa_type_info *pod_object_body::type_info() const
    {
        return spa_debug_type_find(nullptr, m_impl->body->type);
    }

    pod_object_body::operator spa_pod_object_body *() const &
    {
        return get();
    }

    pod_object_body pod_object_body::view(spa_pod_object_body *body, std::size_t size)
    {
        pod_object_body rtn;

        rtn.m_impl->size = size;
        rtn.m_impl->body = body;

        return rtn;
    }
} // namespace pipewire::spa
