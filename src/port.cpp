#include "port/port.hpp"
#include "port/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct port::impl
    {
        raw_type *port;
        port_info info;
    };

    port::~port() = default;

    port::port(port &&other) noexcept : proxy(std::move(other)), m_impl(std::move(other.m_impl)) {}

    port::port(proxy &&base, port_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->port = reinterpret_cast<raw_type *>(proxy::get());
        m_impl->info = std::move(info);
    }

    port &port::operator=(port &&other) noexcept
    {
        proxy::operator=(std::move(other));
        m_impl = std::move(other.m_impl);
        return *this;
    }

    lazy<port::params_t> port::params() const
    {
        struct state
        {
            port_listener listener;

          public:
            params_t params;
        };

        auto m_state    = std::make_shared<state>(get());
        auto weak_state = std::weak_ptr{m_state};

        m_state->listener.on<port_event::param>([weak_state](auto, auto id, auto, auto, spa::pod param) {
            weak_state.lock()->params.emplace(id, std::move(param));
        });

        for (const auto &param : m_impl->info.params)
        {
            pw_port_enum_params(m_impl->port, 0, param.id, 0, 1, nullptr);
        }

        return make_lazy<params_t>([m_state]() -> params_t {
            return m_state->params;
        });
    }

    port::raw_type *port::get() const
    {
        return m_impl->port;
    }

    port_info port::info() const
    {
        return m_impl->info;
    }

    template port_listener port::listen<port_listener>();

    template <class Listener>
        requires valid_listener<Listener, port::raw_type>
    Listener port::listen()
    {
        return {get()};
    }

    port::operator raw_type *() const &
    {
        return get();
    }

    lazy<expected<port>> port::bind(raw_type *raw)
    {
        struct state
        {
            port_listener listener;

          public:
            std::promise<port_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<proxy::raw_type *>(raw));

        auto m_state    = std::make_shared<state>(raw);
        auto weak_state = std::weak_ptr{m_state};

        m_state->listener.once<port_event::info>([weak_state](port_info info) {
            weak_state.lock()->info.set_value(std::move(info));
        });

        return make_lazy<expected<port>>([m_state, proxy_fut = std::move(proxy)]() mutable -> expected<port> {
            auto proxy = proxy_fut.get();

            if (!proxy.has_value())
            {
                return tl::make_unexpected(proxy.error());
            }

            return port{std::move(proxy.value()), m_state->info.get_future().get()};
        });
    }

    const char *port::type            = PW_TYPE_INTERFACE_Port;
    const std::uint32_t port::version = PW_VERSION_PORT;
} // namespace pipewire
