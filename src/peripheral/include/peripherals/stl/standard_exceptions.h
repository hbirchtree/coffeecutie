#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

/* Custom exception types, for more readability */

struct resource_leak : public std::logic_error
{
    using std::logic_error::logic_error;
};

struct resource_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct implementation_error : public std::logic_error
{
    using std::logic_error::logic_error;
};

struct releasemode_error : public std::logic_error
{
    using std::logic_error::logic_error;
};

struct undefined_behavior : public std::logic_error
{
    using std::logic_error::logic_error;

    undefined_behavior(std::string_view err)
        : std::logic_error(std::string(err.begin(), err.end()))
    {
    }
};

struct unimplemented_path : public std::logic_error
{
    using std::logic_error::logic_error;
};

struct memory_error : public std::logic_error
{
    using std::logic_error::logic_error;
};

#define C_ERROR_CODE_OUT_OF_BOUNDS() \
    Throw(undefined_behavior("invalid error code"))

#define C_PTR_CHECK(ptr)                                        \
    if(!ptr)                                                    \
        Throw(undefined_behavior("bad pointer deref: " __FILE__ \
                                 ":" C_STR(__LINE__)));
#define C_PTR_CHECK_MSG(ptr, msg) \
    if(!ptr)                      \
        Throw(undefined_behavior("bad pointer deref: " msg));

#define C_THIS_CHECK                                              \
    if(!this)                                                     \
        Throw(undefined_behavior("bad access to *this: " __FILE__ \
                                 ":" C_STR(__LINE__)));

#if MODE_DEBUG
#define C_ERROR_CHECK(ec)                                           \
    {                                                               \
        if(ec)                                                      \
            Throw(implementation_error(                             \
                __FILE__ ":" C_STR(__LINE__) ": " + ec.message())); \
    }

#define C_ERROR_CHECK_TYPED(ec, etype)                                      \
    {                                                                       \
        if(ec)                                                              \
            Throw(etype(__FILE__ ":" C_STR(__LINE__) ": " + ec.message())); \
    }

#else
#define C_ERROR_CHECK(ec)
#define C_ERROR_CHECK_TYPED(ec, etype)
#endif

#if MODE_DEBUG
#define CASSERT(condition) \
    if(!(condition))       \
        Throw(undefined_behavior("assertion failed: " C_STR(condition)));
#else
#define CASSERT(condition)
#endif
