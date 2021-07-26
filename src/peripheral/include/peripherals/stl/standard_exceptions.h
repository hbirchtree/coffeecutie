#pragma once

#include <stdexcept>

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
};

struct unimplemented_path : public std::logic_error
{
    using std::logic_error::logic_error;
};

struct memory_error : public std::logic_error
{
    using std::logic_error::logic_error;
};
