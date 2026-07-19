#include "module/impl/module.hpp"
#include "context.hpp"

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

        if (!data.descrption.empty())
        {
            out += std::format("node.description = {} ", escape(data.descrption));
        }
    }

    loopback_options::operator module_options() const
    {
        std::string args = "{ capture.props = { ";
        {
            build_props(args, capture);
        }
        args += "} playback.props = { ";
        {
            build_props(args, playback);
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
    struct module::impl
    {
        pw_unique_ptr<raw_type> module;
    };

    // clang-format off
    // clang-format behaves weirdly here because the name clashes with C++20 modules
    module::module(deleter<raw_type> deleter, raw_type *raw)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}))
    {
    }
    // clang-format on

    module::module(module &&) noexcept = default;

    module &module::operator=(module &&) noexcept = default;

    module::~module() = default;

    module_info module::info() const
    {
        return module_info::from(pw_impl_module_get_info(m_impl->module.get()));
    }

    properties module::properties() const
    {
        return pipewire::properties::from(pw_properties_copy(pw_impl_module_get_properties(m_impl->module.get())));
    }

    void module::update_properties(pipewire::properties props)
    {
        pw_impl_module_update_properties(m_impl->module.get(), &props.get()->dict);
    }

    module::raw_type *module::get() const
    {
        return m_impl->module.get();
    }

    module::raw_type *module::release() &&
    {
        return m_impl->module.release();
    }

    module::operator raw_type *() const &
    {
        return get();
    }

    module module::from(raw_type *raw)
    {
        return {pw_impl_module_destroy, raw};
    }

    module module::view(raw_type *raw)
    {
        return {view_deleter<raw_type>, raw};
    }
} // namespace pipewire::impl
