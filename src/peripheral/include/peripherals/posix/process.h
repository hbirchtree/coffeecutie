#pragma once

#include <peripherals/error/posix.h>
#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/identify/system.h>
#include <peripherals/libc/signals.h>
#include <peripherals/stl/types.h>

#include <platforms/file.h>

#include <url/url.h>

#if defined(COFFEE_UNIXPLAT)
#include <signal.h>
#include <sys/resource.h>
#include <unistd.h>

#if defined(COFFEE_ANDROID) || defined(COFFEE_LINUX)
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include <algorithm>

#if defined(COFFEE_LINUX)
#include <sys/wait.h>
#endif

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace platform::common::posix {

using posix_ec = platform::common::posix::posix_error_code;
using fd_t     = int;
using ::pid_t;
using platform::common::posix::collect_error;

namespace fd {

[[nodiscard]] FORCEDINLINE std::optional<posix_error> close(int fd)
{
    if(::close(fd) == -1)
        return get_error();
    return std::nullopt;
}

struct pipe_pair
{
    fd_t read;
    fd_t write;
};

[[nodiscard]] FORCEDINLINE std::optional<posix_error> pipe(pipe_pair& pipe_fd)
{
    pipe_fd = { .read = -1, .write = -1 };
    if(::pipe(reinterpret_cast<int*>(&pipe_fd)) == -1)
        return get_error();
    return std::nullopt;
}

[[nodiscard]] FORCEDINLINE std::optional<posix_error> replace_fd(fd_t target, fd_t source)
{
    if(::dup2(target, source) == -1)
        return get_error();
    return std::nullopt;
}

} // namespace fd
namespace proc {

enum class fork_process
{
    child,
    parent,
    error,
};

enum class wait_by
{
    any       = -1,
    pid_group = 0,
    child_pid = 1,
};

FORCEDINLINE pid_t wait_by_to_value(wait_by cond, pid_t target)
{
    if(cond == wait_by::any || cond == wait_by::pid_group)
        return static_cast<pid_t>(cond);
    else
        return target;
}

#if !defined(COFFEE_NO_WAITPID)
FORCEDINLINE pid_t
wait_for(wait_by flag, posix_ec& ec, pid_t target = 0, int* status = nullptr)
{
    pid_t out = ::waitpid(wait_by_to_value(flag, target), status, 0);

    if(WIFEXITED(*status) || WIFSIGNALED(*status))
        collect_error(ec);

    return out;
}

FORCEDINLINE bool is_exited(pid_t target, int* exitCode)
{
    int   status = 0xFFFF;
    pid_t pid    = ::waitpid(target, &status, WNOHANG);

    if(pid < 0)
        return true;

    if(WIFEXITED(status))
        *exitCode = WEXITSTATUS(status);
    else if(WIFSIGNALED(status))
        *exitCode = WTERMSIG(status);
    else if(WCOREDUMP(status))
        *exitCode = -6;

    return WIFEXITED(status);
}

#endif

FORCEDINLINE void execv(
    std::string const&       program,
    posix_ec&                       ec,
    std::vector<const char*> const& args = {})
{
    ::execv(program.c_str(), const_cast<char* const*>(args.data()));
    collect_error(ec);
}

FORCEDINLINE void execv(
    std::string const&                    program,
    posix_ec&                                    ec,
    std::vector<std::string> const& args = {})
{
    std::vector<const char*> arg_list = {};
    std::transform(
        args.begin(),
        args.end(),
        std::back_inserter(arg_list),
        [](std::string const& arg) {
            return const_cast<char*>(arg.c_str());
        });
    arg_list.push_back(nullptr);

    proc::execv(program, ec, arg_list);
}

FORCEDINLINE fork_process fork(pid_t& childPid, posix_ec& ec)
{
    childPid = ::fork();
    collect_error(ec);
    if(childPid == -1)
        return fork_process::error;
    return childPid == 0 ? fork_process::child : fork_process::parent;
}

enum class spawn_mode
{
    fork_redirected, /* fork process, redirect STDOUT/STDERR/STDIN for parent */
};

template<typename ArgType>
struct exec_info
{
    std::string         program;
    std::vector<ArgType> args;
    platform::url::Url         working_dir;
};

struct spawn_info
{
    fd_t out;
    fd_t err;
    fd_t in;

    pid_t child;
};

template<typename ArgType>
spawn_info spawn(exec_info<ArgType> const& exec)
{
    using libc::signal::sig;
    using platform::url::constructors::MkUrl;

    pid_t    childPid = 0;
    posix_ec ec;

    fd::pipe_pair out;
    fd::pipe_pair err;
    fd::pipe_pair in;

    /* Create redirection pipes */
    if(auto e = fd::pipe(out))
        Throw(posix_runtime_error(*e));
    if(auto e = fd::pipe(err))
        Throw(posix_runtime_error(*e));
    if(auto e = fd::pipe(in))
        Throw(posix_runtime_error(*e));
    if(fork(childPid, ec) == fork_process::child)
    {
        ec = 0;

        auto working_dir = exec.working_dir;
        if(!working_dir.isLocal())
            working_dir = MkUrl(".");
        platform::path::change_dir(working_dir);

        if(auto e = fd::close(STDOUT_FILENO))
            Throw(posix_runtime_error(*e));
        if(auto e = fd::close(STDERR_FILENO))
            Throw(posix_runtime_error(*e));
        if(auto e = fd::close(STDIN_FILENO))
            Throw(posix_runtime_error(*e));

        if(auto e = fd::replace_fd(out.write, STDOUT_FILENO))
            Throw(posix_runtime_error(*e));
        if(auto e = fd::replace_fd(err.write, STDERR_FILENO))
            Throw(posix_runtime_error(*e));
        if(auto e = fd::replace_fd(in.read, STDIN_FILENO))
            Throw(posix_runtime_error(*e));

        proc::execv(exec.program, ec, exec.args);
        C_ERROR_CHECK(ec);

        /* It should never get here */
        libc::signal::exit(sig::general_error);
    }
    C_ERROR_CHECK(ec);

    if(auto e = fd::close(out.write))
        Throw(posix_runtime_error(*e));
    if(auto e = fd::close(err.write))
        Throw(posix_runtime_error(*e));

    return {out.read, err.read, in.write, childPid};
}

inline pid_t self()
{
    return getpid();
}

inline void send_sig(pid_t target, libc::signal::sig signal)
{
    errno = 0;
    ::kill(target, static_cast<int>(signal));

    posix_ec ec;
    collect_error(ec);
    C_ERROR_CHECK(ec);
}

inline const char* code_to_string(int exit_code)
{
#define CODE_TO_STRING(code) \
    case code:               \
        return #code;

    switch(exit_code)
    {
        CODE_TO_STRING(SIGINT);
        CODE_TO_STRING(SIGQUIT);
        CODE_TO_STRING(SIGILL);
        CODE_TO_STRING(SIGTRAP);
        CODE_TO_STRING(SIGABRT);
        CODE_TO_STRING(SIGFPE);
#if !defined(COFFEE_WINDOWS)
        CODE_TO_STRING(SIGKILL);
#endif
        CODE_TO_STRING(SIGSEGV);
        CODE_TO_STRING(SIGBUS);
        CODE_TO_STRING(SIGTERM);
    case -6:
        return "COREDUMPED";
    default:
        return "0";
    }
#undef CODE_TO_STRING
}

inline void breakpoint()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
    //    if(getenv("DEBUG_BREAK"))
    std::raise(SIGINT);
#elif defined(COFFEE_EMSCRIPTEN)
    emscripten_debugger();
#endif

    /* TODO: Reimplement Windows breakpoints */
}

inline void core_dump_enable()
{
    struct rlimit lim;
    lim.rlim_cur = lim.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &lim);
}

} // namespace proc
} // namespace platform::common::posix
#endif
