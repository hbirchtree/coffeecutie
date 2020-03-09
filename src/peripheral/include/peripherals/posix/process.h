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

namespace posix {

using posix_ec = platform::file::posix_error_code;
using fd_t     = int;
using ::pid_t;
using platform::file::posix::collect_error;

namespace fd {

FORCEDINLINE void close(int fd, posix_ec& ec)
{
    ::close(fd);
    collect_error(ec);
}

struct pipe_pair
{
    fd_t read;
    fd_t write;
};

FORCEDINLINE void pipe(pipe_pair& pipe_fd, posix_ec& ec)
{
    ::pipe(reinterpret_cast<int*>(&pipe_fd));
    collect_error(ec);
}

FORCEDINLINE void replace_fd(fd_t target, fd_t source, posix_ec& ec)
{
    ::dup2(target, source);
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
    stl_types::CString const&       program,
    posix_ec&                       ec,
    stl_types::Vector<char*> const& args = {})
{
    ::execv(program.c_str(), args.data());
    collect_error(ec);
}

FORCEDINLINE void execv(
    stl_types::CString const&                    program,
    posix_ec&                                    ec,
    stl_types::Vector<stl_types::CString> const& args = {})
{
    stl_types::Vector<char*> arg_list = {};
    std::transform(
        args.begin(),
        args.end(),
        std::back_inserter(arg_list),
        [](stl_types::CString const& arg) {
            return C_CCAST<char*>(arg.c_str());
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
    stl_types::CString         program;
    stl_types::Vector<ArgType> args;
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
    using platform::file::file_error;
    using platform::url::constructors::MkUrl;

    pid_t    childPid = 0;
    posix_ec ec;

    fd::pipe_pair out;
    fd::pipe_pair err;
    fd::pipe_pair in;

    /* Create redirection pipes */
    fd::pipe(out, ec);
    C_ERROR_CHECK(ec);
    fd::pipe(err, ec);
    C_ERROR_CHECK(ec);

    if(fork(childPid, ec) == fork_process::child)
    {
        ec = 0;

        platform::file::file_error fec;
        auto                       working_dir = exec.working_dir;
        if(!working_dir.isLocal())
            working_dir = MkUrl(".");
        platform::file::DirFun::ChDir(working_dir, fec);
        C_ERROR_CHECK(fec);

        fd::close(STDOUT_FILENO, ec);
        C_ERROR_CHECK(ec);
        fd::close(STDERR_FILENO, ec);
        C_ERROR_CHECK(ec);
        fd::close(STDIN_FILENO, ec);
        C_ERROR_CHECK(ec);

        fd::replace_fd(out.write, STDOUT_FILENO, ec);
        C_ERROR_CHECK(ec);
        fd::replace_fd(err.write, STDERR_FILENO, ec);
        C_ERROR_CHECK(ec);
        fd::replace_fd(in.read, STDIN_FILENO, ec);
        C_ERROR_CHECK(ec);

        proc::execv(exec.program, ec, exec.args);
        C_ERROR_CHECK(ec);

        /* It should never get here */
        libc::signal::exit(sig::general_error);
    }
    C_ERROR_CHECK(ec);

    fd::close(out.write, ec);
    C_ERROR_CHECK(ec);
    fd::close(err.write, ec);
    C_ERROR_CHECK(ec);

    C_ERROR_CHECK(ec);

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

} // namespace proc
} // namespace posix
#endif
