#pragma once

#undef Always

namespace typing {

enum class ValueComparison
{
    Never,
    Less,
    LEqual,
    Equal,
    NEqual,
    Greater,
    GEqual,
    Always,
};
}
