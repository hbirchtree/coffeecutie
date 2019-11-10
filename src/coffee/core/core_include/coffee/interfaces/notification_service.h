#pragma once

#include <coffee/core/base/files/url.h>

namespace Coffee{

enum class NotificationState
{
    Undefined = 0,
    Normal    = 1,
    Secure    = 2,
};

struct NotificationInfo
{
    Url               image;
    CString           text;
    CString           descriptiveText;
    NotificationState state;
};

class Notifications
{
public:
    virtual void persistent(NotificationInfo&&) = 0;
    virtual void temporary(NotificationInfo&&, Chrono::duration) = 0;
};

}
