// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2019 Intel Corporation. All Rights Reserved.
#include "device_watcher.h"
#include <vector>
#include <sstream>
#include "../usb/usb-device.h"
#include "../usb/usb-enumerator.h"
#include "../hid/hid-device.h"
#include "../uvc/uvc-device.h"

using namespace std;
using namespace librealsense;
using namespace librealsense::platform;

std::shared_ptr<device_watcher_usbhost> device_watcher_usbhost::instance()
{
    static std::shared_ptr<device_watcher_usbhost> instance = std::make_shared<device_watcher_usbhost>();
    // w/a so that the device_watcher would be "stopped" after its creation
    if (instance)
        instance->stop();
    return instance;
}

void device_watcher_usbhost::notify()
{
    backend_device_group curr;
    backend_device_group prev;
    librealsense::platform::device_changed_callback callback;

    curr.uvc_devices = query_uvc_devices_info();
    curr.hid_devices = query_hid_devices_info();

    {
        std::lock_guard<std::mutex> lk(_mutex);
        prev = _prev_group;
        _prev_group = curr;
        callback = _callback;
    }

    if(callback)
        callback(prev, curr);
}

void device_watcher_usbhost::start(librealsense::platform::device_changed_callback callback)
{
    std::lock_guard<std::mutex> lk(_mutex);
    _callback = callback;
    _is_stopped = false;
}

void device_watcher_usbhost::stop()
{
    std::lock_guard<std::mutex> lk(_mutex);
    _callback = nullptr;
    _is_stopped = true;
}

bool device_watcher_usbhost::is_stopped() const
{
    return _is_stopped;
}
