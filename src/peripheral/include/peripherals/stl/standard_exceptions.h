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
    undefined_behavior(std::string_view err) :
        std::logic_error(std::string(err.begin(), err.end()))
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
