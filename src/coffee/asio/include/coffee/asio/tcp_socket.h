#pragma once

#include <coffee/core/CProfiling>
#include <iostream>
#include <peripherals/stl/functional_types.h>

#include "asio_data.h"

#define ASIO_TAG "asio::"

namespace Coffee {
namespace ASIO {

namespace socket_types {
#if defined(ASIO_USE_SSL)
using ssl = asio::ssl::stream<asio::ip::tcp::socket>;
#endif
using raw = asio::ip::tcp::socket;
} // namespace socket_types

using host_t = std::string;

namespace tcp_sockets {
template<typename SocketType, typename LowestLayerType>
struct socket_base : std::istream, std::ostream, non_copy
{
    using sock_t          = SocketType;
    using lowest_sock_t   = LowestLayerType;
    using resolver_result = asio::ip::tcp::resolver::results_type;
    using resolver_iter   = asio::ip::tcp::resolver::iterator;

    template<typename... Args>
    struct async_callbacks
    {
        static void default_error_callback(
            concurrent_notif&, asio::error_code ec)
        {
            Throw(resource_error(ec.message()));
        }

        async_callbacks() :
            m_on_complete([](concurrent_notif&, Args...) {
                Throw(implementation_error(DTEXT("No callback registered")));
            }),
            m_on_error(default_error_callback)
        {
        }

        async_callbacks(
            Function<void(concurrent_notif&, Args...)>&& on_complete) :
            m_on_complete(on_complete),
            m_on_error(default_error_callback)
        {
        }

        async_callbacks(
            Function<void(concurrent_notif&, Args...)>&&          on_complete,
            Function<void(concurrent_notif&, asio::error_code)>&& on_error) :
            m_on_complete(on_complete),
            m_on_error(on_error)
        {
        }

        void on_complete(Args... args)
        {
            m_on_complete(notif, args...);
        }

        void on_error(asio::error_code ec)
        {
            m_on_error(notif, ec);
        }

        Function<void(concurrent_notif&, Args...)>          m_on_complete;
        Function<void(concurrent_notif&, asio::error_code)> m_on_error;

        concurrent_notif notif;
    };

    template<typename... Args>
    using async_ptr = ShPtr<async_callbacks<Args...>>;

    asio_context context;
    sock_t       socket;

  private:
    asio::streambuf recvp;
    asio::streambuf trans;

  protected:
    asio::error_code lastError;

  public:
#if defined(ASIO_USE_SSL)
    template<
        typename Dummy = void,
        typename std::enable_if<
            std::is_same<sock_t, socket_types::ssl>::value,
            Dummy>::type* = nullptr>
    socket_base(asio_context c) :
        std::istream(&recvp), std::ostream(&trans), context(c),
        socket(context->service, context->sslctxt)
    {
    }
#endif

    template<
        typename Dummy = void,
        typename std::enable_if<
            std::is_same<sock_t, socket_types::raw>::value,
            Dummy>::type* = nullptr>
    socket_base(asio_context c) :
        std::istream(&recvp), std::ostream(&trans), context(c),
        socket(context->service)
    {
    }

    asio::error_code error() const
    {
        return lastError;
    }

    template<typename T, typename R>
    R& operator<<(T const& v)
    {
        return (*this) << v;
    }

    template<typename T, typename R>
    R& operator>>(T& v)
    {
        return (*this) >> v;
    }

    socket_base<SocketType, LowestLayerType>& write(Vector<char>& buf)
    {
        auto var = MkShared<concurrent_notif>();
        var->prepare_lock();

        auto ref = asio::dynamic_vector_buffer<
            Vector<char>::value_type,
            Vector<char>::allocator_type>(buf);

        asio::error_code ec;
        asio::async_write(
            socket, std::move(ref), [var](asio::error_code, size_t) {
                var->notify();
            });

        var->await();

        return *this;
    }

    socket_base<SocketType, LowestLayerType>& write(CString& buf)
    {
        auto var = MkShared<concurrent_notif>();
        var->prepare_lock();

        auto ref = asio::dynamic_string_buffer<
            CString::value_type,
            CString::traits_type,
            CString::allocator_type>(buf);

        asio::error_code ec;
        asio::async_write(
            socket, std::move(ref), [var](asio::error_code, size_t) {
                var->notify();
            });

        var->await();

        return *this;
    }

    template<typename... Args>
    async_ptr<Args...> gen_handler(
        Function<void(Args...)>&&          completion,
        Function<void(asio::error_code)>&& errored)
    {
        return MkShared<async_callbacks<Args...>>(
            std::move(completion), std::move(errored));
    }

    void resolve_handler(
        LowestLayerType*                           socket_layer,
        async_ptr<asio::error_code, resolver_iter> calls,
        asio::error_code                           ec,
        resolver_iter                              r)
    {
        DProfContext _(DTEXT(ASIO_TAG "resolve"));

        if(ec)
        {
            calls->on_error(ec);
            lastError = ec;
            return;
        }

        using it_type = decltype(r);

        asio::async_connect(
            *socket_layer,
            r,
            it_type(),
            std::bind(
                &async_callbacks<asio::error_code, resolver_iter>::on_complete,
                calls,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    template<typename... Args>
    void async_connect_dispatch(
        LowestLayerType*                           socket_layer,
        async_ptr<asio::error_code, resolver_iter> callback,
        Args... args)
    {
        asio::ip::tcp::resolver::query q(args...);

        DProfContext _(DTEXT(ASIO_TAG "connection negotiation"));

        context->resolver.async_resolve(
            q,
            [this, callback, socket_layer](
                asio::error_code ec, resolver_iter it) {
                this->resolve_handler(socket_layer, callback, ec, it);
            });
    }

    template<typename... Args>
    void async_dispatch(
        async_ptr<Args...>& calls, asio::error_code ec, Args... args)
    {
        DProfContext _(DTEXT(ASIO_TAG "async dispatch"));
        if(ec)
        {
            calls->on_error(ec);
            lastError = ec;
            return;
        }

        calls->on_complete(args...);
    }

    template<typename... Args>
    void async_dispatch_skiperror(
        async_ptr<Args...>& calls, asio::error_code ec, Args... args)
    {
        DProfContext _(DTEXT(ASIO_TAG "async dispatch"));
        if(ec)
        {
            calls->on_error(ec);
            lastError = ec;
        }

        calls->on_complete(args...);
    }
    void async_read_some_receive(
        async_ptr<size_t> calls, asio::error_code ec, size_t amount_data)
    {
        DProfContext _(DTEXT(ASIO_TAG "read receive"));

        std::istream::rdbuf(&recvp);
        async_dispatch_skiperror(calls, ec, amount_data);
    }

    void async_write_receive(
        async_ptr<> call, asio::error_code ec, size_t sz)
    {
        DProfContext _(DTEXT(ASIO_TAG "write receive"));

        trans.consume(sz);
        async_dispatch_skiperror(call, ec);
    }

#if defined(ASIO_USE_SSL)
    template<
        typename Dummy = void,
        typename std::enable_if<
            std::is_same<sock_t, socket_types::ssl>::value,
            Dummy>::type* = nullptr>
    void close_internal()
    {
        DProfContext     _(DTEXT(ASIO_TAG "socket close"));
        asio::error_code ec;

        socket.shutdown(ec);

        auto& next_layer = socket.next_layer();

        next_layer.shutdown(sock_t::next_layer_type::shutdown_both, ec);
        next_layer.close(ec);

        next_layer.wait(sock_t::next_layer_type::wait_error, ec);
    }
#endif

    template<
        typename Dummy = void,
        typename std::enable_if<
            std::is_same<sock_t, socket_types::raw>::value,
            Dummy>::type* = nullptr>
    void close_internal()
    {
        DProfContext     _(DTEXT(ASIO_TAG "socket close"));
        asio::error_code ec;

        socket.shutdown(sock_t::shutdown_both, ec);
        socket.close(ec);

        socket.wait(sock_t::wait_error, ec);
    }

    void async_close_receive(async_ptr<> calls)
    {
        close_internal();

        calls->on_complete();
    }

    template<typename T>
    void async_read_until(T const& delim, async_ptr<size_t> callbacks)
    {
        asio::async_read_until(
            socket,
            recvp,
            delim,
            std::bind(
                &socket_base::async_read_some_receive,
                this,
                callbacks,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    template<typename T>
    size_t read_until(T const& delim)
    {
        size_t out = 0;

        async_ptr<size_t> callback = MkShared<async_callbacks<size_t>>(
            [&out](concurrent_notif& var, size_t amount_bytes) {
                out = amount_bytes;
                var.notify();
            },
            [](concurrent_notif& var, asio::error_code) { var.notify(); });

        auto& var = callback->notif;

        callback->notif.prepare_lock();
        async_read_until(delim, callback);
        callback->notif.await();

        return out;
    }

    void async_read_some(size_t amount_bytes, async_ptr<size_t> callbacks)
    {
        if(amount_bytes < recvp.in_avail())
        {
            callbacks->on_complete(amount_bytes);
            return;
        }

        asio::async_read(
            socket,
            recvp,
            asio::transfer_exactly(amount_bytes - recvp.in_avail()),
            std::bind(
                &socket_base::async_read_some_receive,
                this,
                callbacks,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    size_t read_some(size_t amount_bytes)
    {
        size_t out = 0;

        auto callback = MkShared<async_callbacks<size_t>>(
            [&out](concurrent_notif& var, size_t amount_bytes) {
                out = amount_bytes;
                var.notify();
            },
            [](concurrent_notif& var, asio::error_code) { var.notify(); });

        callback->notif.prepare_lock();
        async_read_some(amount_bytes, callback);
        callback->notif.await();

        return out;
    }

    void async_read_all(Function<void(size_t)>&& onRead)
    {
        asio::async_read(
            socket,
            recvp,
            bind_this::func(
                this,
                &socket_base::async_read_some_receive,
                {std::move(onRead),
                 async_callbacks<>::default_error_callback}));
    }

    void async_write(async_ptr<> callbacks)
    {
        //        std::ostream::flush();

        asio::async_write(
            socket,
            trans.data(),
            std::bind(
                &socket_base::async_write_receive,
                this,
                std::move(callbacks),
                std::placeholders::_1,
                std::placeholders::_2)
            //            bind_this::func(
            //                this, &socket_base::async_write_receive,
            //                std::move(callbacks))
        );
    }

    asio::error_code flush()
    {
        DProfContext _(DTEXT(ASIO_TAG "flushing"));

        auto callbacks = MkShared<async_callbacks<>>(
            [](concurrent_notif& var) { var.notify(); },
            [](concurrent_notif& var, asio::error_code) { var.notify(); });

        callbacks->notif.prepare_lock();
        async_write(callbacks);
        callbacks->notif.await();

        return lastError;
    }

    void async_close(async_ptr<> callback)
    {
        context->service.post(std::bind(
            &socket_base::async_close_receive, this, std::move(callback)));
    }

    void sync_close()
    {
        DProfContext _(DTEXT(ASIO_TAG "synchronous close"));

        auto callback = MkShared<async_callbacks<>>(
            [](concurrent_notif& var) { var.notify(); },
            async_callbacks<>::default_error_callback);

        callback->notif.prepare_lock();
        async_close(callback);
        callback->notif.await();
    }
};

struct raw_socket : socket_base<socket_types::raw, socket_types::raw>
{
  public:
    raw_socket(asio_context c) : socket_base(c)
    {
    }

    void connect_handler(
        async_ptr<> call, asio::error_code ec, resolver_iter)
    {
        DProfContext _(DTEXT(ASIO_TAG "connect"));
        async_dispatch(call, ec);
    }

    template<typename... Args>
    void async_connect(async_ptr<> callbacks, Args... args)
    {
        async_connect_dispatch(
            &socket,
            MkShared<async_callbacks<asio::error_code, resolver_iter>>(
                [callbacks, this](
                    concurrent_notif&,
                    asio::error_code ec,
                    resolver_iter    iter) {
                    this->connect_handler(callbacks, ec, iter);
                },
                [callbacks](concurrent_notif&, asio::error_code ec) {
                    callbacks->on_error(ec);
                }),
            args...);
    }

    template<typename Dur, typename... Args>
    NO_DISCARD asio::error_code connect(Dur const& timeout, Args... args)
    {
        DProfContext _(DTEXT(ASIO_TAG "synchronous connect"));

        auto callback = MkShared<async_callbacks<>>(
            [](concurrent_notif& var) { var.notify(); },
            [](concurrent_notif& var, asio::error_code) { var.notify(); });
        auto& var = callback->notif;

        var.prepare_lock();
        async_connect(callback, args...);

        if(var.await(timeout) == cv_status::timeout)
            return asio::error_code(
                asio::error::timed_out, asio::error::misc_category);
        else
            return asio::error_code();
    }
};

/*!
 * \brief A wrapper for asio::ssl::stream to make it work like iostream,
 *  except that it needs to have pull() called before using std::getline.
 * The rest works the same!
 * This is made as a drop-in replacement for Socket, and may not support
 *  all specifics of SSL.
 */
#if defined(ASIO_USE_SSL)
struct ssl_socket
    : socket_base<socket_types::ssl, socket_types::ssl::next_layer_type>
{
    using resolver_result = asio::ip::tcp::resolver::results_type;
    using resolver_iter   = asio::ip::tcp::resolver::iterator;

  private:
    bool do_verify;

  public:
    ssl_socket(asio_context c) : socket_base(c), do_verify(true)
    {
    }

    void connect_handler(async_ptr<> calls, asio::error_code ec, resolver_iter it)
    {
        DProfContext _(DTEXT(ASIO_TAG "connect"));

        if(ec)
        {
            calls->on_error(ec);
            lastError = ec;
            return;
        }

        socket.lowest_layer().set_option(asio::ip::tcp::no_delay(true));

        socket.set_verify_mode(asio::ssl::verify_peer);
#if !defined(COFFEE_ANDROID) && !defined(COFFEE_APPLE) && !defined(COFFEE_LINUX)
        if(do_verify)
        {
            socket.set_verify_callback(
                asio::ssl::rfc2818_verification(it->host_name()), ec);
            if(ec != asio::error_code())
            {
                calls.on_error(ec);
                lastError = ec;
                return;
            }
        } else
#endif
        {
            socket.set_verify_callback(
                [](bool, asio::ssl::verify_context&) { return true; });
        }

        socket.async_handshake(
            sock_t::client,
            std::bind(
                &ssl_socket::handshake_handler,
                this,
                calls,
                std::placeholders::_1));
    }

    void handshake_handler(async_ptr<> calls, asio::error_code ec)
    {
        DProfContext _(DTEXT(ASIO_TAG "handshake"));
        async_dispatch(calls, ec);
    }

    template<typename... Args>
    void async_connect(async_ptr<> callbacks, bool verify, Args... args)
    {
        this->do_verify = verify;

        async_connect_dispatch(
            &socket.next_layer(),
            MkShared<async_callbacks<asio::error_code, resolver_iter>>(
                [callbacks, this](
                    concurrent_notif&,
                    asio::error_code ec,
                    resolver_iter    iter) {
                    this->connect_handler(callbacks, ec, iter);
                },
                [callbacks](concurrent_notif&, asio::error_code ec) {
                    callbacks->on_error(ec);
                }),
            args...);
    }

    template<typename Dur, typename... Args>
    NO_DISCARD asio::error_code connect(
        bool verify, Dur const& timeout, Args... args)
    {
        DProfContext _(DTEXT(ASIO_TAG "synchronous connect"));

        auto callback = MkShared<async_callbacks<>>(
            [](concurrent_notif& var) { var.notify(); },
            [](concurrent_notif& var, asio::error_code) { var.notify(); });
        callback->notif.prepare_lock();

        async_connect(callback, verify, args...);

        if(callback->notif.await(timeout) == cv_status::timeout)
            return asio::error_code(
                asio::error::timed_out, asio::error::misc_category);
        else
            return asio::error_code();
    }
};
#endif

}; // namespace tcp_sockets

} // namespace ASIO

namespace TCP {
using Socket = ASIO::tcp_sockets::raw_socket;
#if defined(ASIO_USE_SSL)
using SSLSocket = ASIO::tcp_sockets::ssl_socket;
#endif
} // namespace TCP

} // namespace Coffee

#undef ASIO_TAG
