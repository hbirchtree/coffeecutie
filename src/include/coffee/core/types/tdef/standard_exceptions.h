#pragma once

#include <stdexcept>

/* Custom exception types, for more readability */

struct resource_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct implementation_error : std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

struct releasemode_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct undefined_behavior : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct memory_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};
