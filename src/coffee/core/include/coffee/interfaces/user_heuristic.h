#pragma once

namespace Coffee{

/*!
 * Follows the concepts of:
 *
 * https://developer.android.com/training/id-auth/identify
 *
 */
class UserHeuristic
{
public:
    virtual Hash userId() = 0;
};

}
