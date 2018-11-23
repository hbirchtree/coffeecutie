#pragma once

#include <peripherals/stl/types.h>

namespace {

using namespace ::stl_types;
}

namespace platform {
namespace info {

struct HardwareDevice
{
    FORCEDINLINE HardwareDevice()
    {
    }
    FORCEDINLINE HardwareDevice(CString model, CString firmware) :
        model(model), firmware(firmware)
    {
    }
    FORCEDINLINE HardwareDevice(
        CString manufacturer, CString model, CString firmware) :
        manufacturer(manufacturer),
        model(model), firmware(firmware)
    {
    }
    FORCEDINLINE HardwareDevice(
        CString manufacturer, CString model, CString firmware, CString serial) :
        manufacturer(manufacturer),
        model(model), firmware(firmware), serial(serial)
    {
    }

    const CString manufacturer;
    const CString model;
    const CString firmware;
    const CString serial;
};

} // namespace info
} // namespace platform

namespace Coffee {

using HWDeviceInfo = platform::info::HardwareDevice;
}
