#include <coffee/core/CApplication>
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/misc/dbus/dbus-proc.h>

#include <dbus-c++-1/dbus-c++/dbus.h>
#include <coffee/misc/dbus/mediaplayer2.h>

using namespace Coffee;

class MediaPlayer2
        : public ::DBus::IntrospectableProxy,
        public ::DBus::ObjectProxy,
        org::freedesktop::MediaPlayer_proxy
{
public:
    MediaPlayer2(::DBus::Connection& bus,
                 cstring path, cstring target)
        : ::DBus::ObjectProxy(bus, path, target)
    {
    }

    // MediaPlayer_proxy interface
    void TrackChange(const std::map<std::string, ::DBus::Variant> &argin0);
    void StatusChange(const::DBus::Struct<int32_t, int32_t, int32_t, int32_t> &argin0);
    void CapsChange(const int32_t &argin0);
    void TrackListChange(const int32_t &argin0);
};

i32 coffee_main(i32, cstring_w*)
{
    ::DBus::BusDispatcher dispatcher;
    ::DBus::default_dispatcher = &dispatcher;

    ::DBus::Connection bus = ::DBus::Connection::SessionBus();

    MediaPlayer2 player(bus, "/Player", "org.mpris.MediaPlayer2.spotify");

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
