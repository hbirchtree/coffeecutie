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
        static void default_error_callback(asio::error_code ec)
        {
            Throw(resource_error(ec.message()));
        }

        async_callbacks() :
            on_complete([](Args...) {
                Throw(implementation_error(DTEXT("No callback registered")));
            }),
            on_error(default_error_callback)
        {
        }

        async_callbacks(Function<void(Args...)>&& on_complete) :
            on_complete(on_complete), on_error(default_error_callback)
        {
        }

        async_callbacks(
            Function<void(Args...)>&&          on_complete,
            Function<void(asio::error_code)>&& on_error) :
            on_complete(on_complete),
            on_error(on_error)
        {
        }

        Function<void(Args...)>          on_complete;
        Function<void(asio::error_code)> on_error;
    };

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
        concurrent_notif var;
        var.prepare_lock();

        auto ref = asio::dynamic_vector_buffer<
            Vector<char>::value_type,
            Vector<char>::allocator_type>(buf);

        asio::error_code ec;
        asio::async_write(
            socket, std::move(ref), [&](asio::error_code, size_t) {
                var.notify();
            });

        var.await();

        return *this;
    }

    socket_base<SocketType, LowestLayerType>& write(CString& buf)
    {
        concurrent_notif var;
        var.prepare_lock();

        auto ref = asio::dynamic_string_buffer<
            CString::value_type,
            CString::traits_type,
            CString::allocator_type>(buf);

        asio::error_code ec;
        asio::async_write(
            socket, std::move(ref), [&](asio::error_code, size_t) {
                var.notify();
            });

        var.await();

        return *this;
    }

    template<typename... Args>
    async_callbacks<Args...> gen_handler(
        Function<void(Args...)>&&          completion,
        Function<void(asio::error_code)>&& errored)
    {
        return async_callbacks<Args...>(
            std::move(completion), std::move(errored));
    }

    void resolve_handler(
        std::tuple<
            LowestLayerType*,
            async_callbacks<asio::error_code, resolver_iter>> fun,
        asio::error_code                                      ec,
        resolver_iter                                         r)
    {
        DProfContext _(DTEXT(ASIO_TAG "resolve"));

        async_callbacks<asio::error_code, resolver_iter> calls;
        LowestLayerType*                                 socket_layer = nullptr;

        std::tie(socket_layer, calls) = fun;

        if(ec)
        {
            calls.on_error(ec);
            lastError = ec;
            return;
        }

        using it_type = decltype(r);

        asio::async_connect(*socket_layer, r, it_type(), calls.on_complete);
    }

    template<typename... Args>
    void async_connect_dispatch(
        LowestLayerType&                                 socket_layer,
        async_callbacks<asio::error_code, resolver_iter> onConnect,
        Args... args)
    {
        asio::ip::tcp::resolver::query q(args...);

        DProfContext _(DTEXT(ASIO_TAG "connection negotiation"));

        context->resolver.async_resolve(
            q,
            bind_this::func(
                this,
                &socket_base::resolve_handler,
                std::make_tuple(&socket_layer, onConnect)));
    }

    template<typename... Args>
    void async_dispatch(
        async_callbacks<Args...> const& calls,
        asio::error_code                ec,
        Args... args)
    {
        DProfContext _(DTEXT(ASIO_TAG "async dispatch"));
        if(ec)
        {
            calls.on_error(ec);
            lastError = ec;
            return;
        }

        calls.on_complete(args...);
    }

    template<typename... Args>
    void async_dispatch_skiperror(
        async_callbacks<Args...> const& calls,
        asio::error_code                ec,
        Args... args)
    {
        DProfContext _(DTEXT(ASIO_TAG "async dispatch"));
        if(ec)
        {
            calls.on_error(ec);
            lastError = ec;
        }

        calls.on_complete(args...);
    }
    void async_read_some_receive(
        std::tuple<async_callbacks<size_t>> funs,
        asio::error_code                    ec,
        size_t                              amount_data)
    {
        DProfContext            _(DTEXT(ASIO_TAG "read receive"));
        async_callbacks<size_t> calls;
        std::tie(calls) = funs;

        std::istream::rdbuf(&recvp);
        async_dispatch_skiperror(calls, ec, amount_data);
    }

    void async_write_receive(
        std::tuple<async_callbacks<>> fun, asio::error_code ec, size_t sz)
    {
        DProfContext _(DTEXT(ASIO_TAG "write receive"));

        async_callbacks<> call;
        std::tie(call) = fun;

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

    void async_close_receive(std::tuple<async_callbacks<>> fun)
    {
        async_callbacks<> calls;
        std::tie(calls) = fun;

        close_internal();

        calls.on_complete();
    }

    template<typename T>
    void async_read_until(T const& delim, async_callbacks<size_t>&& callbacks)
    {
        asio::async_read_until(
            socket,
            recvp,
            delim,
            bind_this::func(
                this, &socket_base::async_read_some_receive, callbacks));
    }

    template<typename T>
    size_t read_until(T const& delim)
    {
        size_t           out = 0;
        concurrent_notif var;
        var.prepare_lock();

        async_read_until(
            delim,
            {[&](size_t amount_bytes) {
                 out = amount_bytes;
                 var.notify();
             },
             [&](asio::error_code) { var.notify(); }});

        var.await();
        return out;
    }

    void async_read_some(
        size_t amount_bytes, async_callbacks<size_t>&& callbacks)
    {
        if(amount_bytes < recvp.in_avail())
        {
            callbacks.on_complete(amount_bytes);
            return;
        }

        asio::async_read(
            socket,
            recvp,
            asio::transfer_exactly(amount_bytes - recvp.in_avail()),
            bind_this::func(
                this, &socket_base::async_read_some_receive, callbacks));
    }

    size_t read_some(size_t amount_bytes)
    {
        size_t           out = 0;
        concurrent_notif var;
        var.prepare_lock();

        async_read_some(
            amount_bytes,
            {[&](size_t amount_bytes) {
                 out = amount_bytes;
                 var.notify();
             },
             [&](asio::error_code) { var.notify(); }});

        var.await();
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

    void async_write(async_callbacks<>&& callbacks)
    {
        //        std::ostream::flush();

        asio::async_write(
            socket,
            trans.data(),
            bind_this::func(
                this, &socket_base::async_write_receive, std::move(callbacks)));
    }

    asio::error_code pull(size_t amount_bytes)
    {
        DProfContext     _(DTEXT(ASIO_TAG "synchronous read"));
        concurrent_notif var;
        var.prepare_lock();

        async_read_some(amount_bytes, [&](size_t) { var.notify(); });

        var.await();

        return lastError;
    }

    asio::error_code flush()
    {
        DProfContext     _(DTEXT(ASIO_TAG "flushing"));
        concurrent_notif var;
        var.prepare_lock();

        async_write(
            {[&]() { var.notify(); }, [&](asio::error_code) { var.notify(); }});

        var.await();

        return lastError;
    }

    void async_close(Function<void()>&& onClose)
    {
        context->service.post(bind_this::func(
            this,
            &socket_base::async_close_receive,
            {std::move(onClose), async_callbacks<>::default_error_callback}));
    }

    void sync_close()
    {
        DProfContext     _(DTEXT(ASIO_TAG "synchronous close"));
        concurrent_notif var;
        var.prepare_lock();

        async_close([&]() { var.notify(); });

        var.await();
    }

    asio::error_code pull()
    {
        DProfContext _(DTEXT(ASIO_TAG "synchronous read"));

        concurrent_notif var;

        var.prepare_lock();

        async_read_all([&](size_t) { var.notify(); });

        var.await();

        return lastError;
    }
};

struct raw_socket : socket_base<socket_types::raw, socket_types::raw>
{
  public:
    raw_socket(asio_context c) : socket_base(c)
    {
    }

    void connect_handler(
        std::tuple<async_callbacks<>> fun, asio::error_code ec, resolver_iter)
    {
        DProfContext      _(DTEXT(ASIO_TAG "connect"));
        async_callbacks<> call;
        std::tie(call) = fun;

        async_dispatch(call, ec);
    }

    template<typename... Args>
    void async_connect(async_callbacks<> callbacks, Args... args)
    {
        auto error_copy = callbacks.on_error;

        async_connect_dispatch(
            socket,
            bind_this::func(
                C_CAST<socket_base*>(this),
                &socket_base::resolve_handler,
                std::make_tuple(
                    &socket,
                    gen_handler(
                        bind_this::func(
                            this, &raw_socket::connect_handler, callbacks),
                        std::move(error_copy)))),
            args...);
    }

    template<typename... Args>
    void connect(Args... args)
    {
        DProfContext     _(DTEXT(ASIO_TAG "synchronous connect"));
        concurrent_notif var;
        var.prepare_lock();

        async_connect(
            {[&]() { var.notify(); }, [&](asio::error_code) { var.notify(); }},
            args...);

        var.await();
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

    void connect_handler(
        std::tuple<async_callbacks<>> fun,
        asio::error_code              ec,
        resolver_iter                 it)
    {
        DProfContext      _(DTEXT(ASIO_TAG "connect"));
        async_callbacks<> calls;
        std::tie(calls) = fun;

        if(ec)
        {
            calls.on_error(ec);
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
            bind_this::func(this, &ssl_socket::handshake_handler, fun));
    }

    void handshake_handler(
        std::tuple<async_callbacks<>> fun, asio::error_code ec)
    {
        DProfContext      _(DTEXT(ASIO_TAG "handshake"));
        async_callbacks<> calls;
        std::tie(calls) = fun;

        async_dispatch(calls, ec);
    }

    template<typename... Args>
    void async_connect(async_callbacks<>&& callbacks, bool verify, Args... args)
    {
        auto error_copy = callbacks.on_error;
        this->do_verify = verify;

        async_connect_dispatch(
            socket.next_layer(),
            bind_this::func(
                C_CAST<socket_base*>(this),
                &socket_base::resolve_handler,
                &socket.next_layer(),
                gen_handler(
                    bind_this::func(
                        this, &ssl_socket::connect_handler, callbacks),
                    std::move(error_copy))),
            args...);
    }

    template<typename... Args>
    void connect(bool verify, Args... args)
    {
        DProfContext     _(DTEXT(ASIO_TAG "synchronous connect"));
        asio::error_code ec;
        concurrent_notif var;
        var.prepare_lock();

        async_connect(
            {[&]() { var.notify(); }, [&](asio::error_code) { var.notify(); }},
            verify,
            args...);

        var.await();

        if(ec)
            lastError = ec;
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
