#pragma once

namespace typing {

enum class ValueHandling
{
    Zero,
    Keep,
    Replace,
    Increment,
    Decrement,
    WrapIncrement,
    WrapDecrement,
    Invert,
};

enum class Operator
{
    Add,
    Sub,
    RevSub,
    Min,
    Max,
};

} // namespace typing
