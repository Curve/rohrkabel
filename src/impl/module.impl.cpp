#include "module/impl/module.hpp"
#include "context.hpp"

#include <ranges>
#include <format>

#include <pipewire/pipewire.h>
#include <pipewire/impl-module.h>

namespace pipewire
{
    std::string escape(const std::string &str)
    {
        const auto needed = spa_json_encode_string(nullptr, 0, str.c_str());
        auto buffer       = std::string(needed, '\0');
        spa_json_encode_string(buffer.data(), static_cast<int>(buffer.size()), str.c_str());

        return buffer;
    }

    template <typename T>
    void build_props(std::string &out, const T &data)
    {
        if (data.node.has_value())
        {
            out += std::format("node.target = {} ", *data.node);
        }

        if (!data.name.empty())
        {
            out += std::format("node.name = {} ", escape(data.name));
        }

        if (!data.description.empty())
        {
            out += std::format("node.description = {} ", escape(data.description));
        }
    }

    loopback_options::operator module_options() const
    {
        static constexpr auto fmt = [](auto &&item)
        {
            return std::format("{} = {} ", item.first, item.second);
        };

        const auto additional = props                        //
                                | std::views::transform(fmt) //
                                | std::views::join           //
                                | std::ranges::to<std::string>();

        std::string args = "{ capture.props = { ";
        {
            build_props(args, capture);
            args += additional;
        }
        args += "} playback.props = { ";
        {
            build_props(args, playback);
            args += additional;
        }
        args += "} }";

        return {
            .name = "libpipewire-module-loopback",
            .args = std::move(args),
        };
    }
} // namespace pipewire

namespace pipewire::impl
{
    struct mod::impl
    {
        pw_unique_ptr<raw_type> module;
    };

    // clang-format off
    // clang-format behaves weirdly here because the name clashes with C++20 modules
    mod::mod(deleter<raw_type> deleter, raw_type *raw)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}))
    {
    }
    // clang-format on

    mod::mod(mod &&) noexcept = default;

    mod &mod::operator=(mod &&) noexcept = default;

    mod::~mod() = default;

    module_info mod::info() const
    {
        return module_info::from(pw_impl_module_get_info(m_impl->module.get()));
    }

    properties mod::properties() const
    {
        return pipewire::properties::from(pw_properties_copy(pw_impl_module_get_properties(m_impl->module.get())));
    }

    void mod::update_properties(pipewire::properties props)
    {
        pw_impl_module_update_properties(m_impl->module.get(), &props.get()->dict);
    }

    mod::raw_type *mod::get() const
    {
        return m_impl->module.get();
    }

    mod::raw_type *mod::release() &&
    {
        return m_impl->module.release();
    }

    mod::operator raw_type *() const &
    {
        return get();
    }

    mod mod::from(raw_type *raw)
    {
        // We avoid `pw_impl_module_destroy`, because e.g. a pw-loopback can destroy itself when a connected node disconnects
        // (and is using node.dont-fallback)
        return {pw_impl_module_schedule_destroy, raw};
    }

    mod mod::view(raw_type *raw)
    {
        return {view_deleter<raw_type>, raw};
    }
} // namespace pipewire::impl
