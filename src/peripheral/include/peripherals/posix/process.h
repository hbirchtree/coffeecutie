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

#include <algorithm>

#if defined(COFFEE_LINUX)
#include <sys/wait.h>
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

enum class wait_by
{
    any       = WAIT_ANY,
    pid_group = WAIT_MYPGRP,
    child_pid = 1,
};

enum class fork_process
{
    child,
    parent,
};

FORCEDINLINE pid_t
             wait_for(wait_by flag, posix_ec& ec, pid_t target = 0, int* status = nullptr)
{
    pid_t out = ::waitpid(
        flag == wait_by::any
            ? WAIT_ANY
            : (flag == wait_by::pid_group ? WAIT_MYPGRP : target),
        status ? status : nullptr,
        0);

    if(WIFEXITED(out))
        collect_error(ec);

    return out;
}

FORCEDINLINE bool is_exited(pid_t target)
{
    int   status = 0xFFFF;
    pid_t pid    = ::waitpid(target, &status, WNOHANG);

    if(pid < 0)
        return true;

    return WIFEXITED(status);
}

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
        auto working_dir = exec.working_dir;
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
    fprintf(stderr, "parent: %i\n", childPid);

    fd::close(out.write, ec);
    C_ERROR_CHECK(ec);
    fd::close(err.write, ec);
    C_ERROR_CHECK(ec);

    C_ERROR_CHECK(ec);

    return {out.read, err.read, in.write, childPid};
}

} // namespace proc
} // namespace posix
#endif
