//
//  device.cpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#include <cassert>
#include <string>

#include "hs_cpp.hpp"
#include "log.hpp"

#include "device.hpp"
#include "exception.hpp"
#include "exception_util.hpp"

namespace hs {

Device::Device(int index, LogLevel log_level)
    : index_(index), handle_(nullptr) {
    assert(index_ >= 0);
    setLogLevel(log_level);
    find();
    open();
}

Device::~Device() {
    try {
        close();
    } catch (HSException &e) {
        HS_ERROR_STREAM("Exception caught on device[" << index_ << "], "
                                                      << e.what());
    }
}

std::string Device::getName() const {
    assert(handle_ != nullptr);
    return name_;
}

void Device::open() {
    HS_INFO_STREAM("opening device #" << index_ << " name=" << name_);
    int ret = hsOpenDevice(name_.c_str(), &handle_);
    ExceptionUtil::tryToThrowHsException(ret);
}

void Device::close() {
    if (handle_ == nullptr) {
        return;
    }

    HS_INFO_STREAM("close device #" << index_ << " name=" << name_);
    int ret = hsCloseDevice(handle_);
    ExceptionUtil::tryToThrowHsException(ret);
}

void Device::monitorThermal() const {
    ThermalThrottlingLevel level = getThermalThrottlingLevel();

    if (level == High) {
        throw HsHighThermal();
    }

    if (level == Aggressive) {
        throw HsAggressiveThermal();
    }
}

void Device::setLogLevel(LogLevel log_level) {
    int level = log_level;
    int ret = hsSetGlobalOption(HS_LOG_LEVEL, static_cast<void *>(&level),
                                sizeof(level));
    ExceptionUtil::tryToThrowHsException(ret);
}

Device::LogLevel Device::getLogLevel() {
    int level;
    unsigned int size_of_level = sizeof(level);
    int ret = hsGetGlobalOption(HS_LOG_LEVEL, reinterpret_cast<void **>(&level),
                                &size_of_level);
    ExceptionUtil::tryToThrowHsException(ret);
    return static_cast<LogLevel>(level);
}

Device::ThermalThrottlingLevel Device::getThermalThrottlingLevel() const {
    assert(handle_ != nullptr);
    int throttling;
    unsigned int size_of_throttling = sizeof(throttling);
    int ret = hsGetDeviceOption(handle_, HS_THERMAL_THROTTLING_LEVEL,
                                reinterpret_cast<void **>(&throttling),
                                &size_of_throttling);
    ExceptionUtil::tryToThrowHsException(ret);
    return static_cast<ThermalThrottlingLevel>(throttling);
}

void *Device::getHandle() {
    assert(handle_ != nullptr);
    return handle_;
}

void Device::find() {
    assert(handle_ == nullptr);
    char name[HS_MAX_NAME_SIZE];
    int ret = hsGetDeviceName(index_, name, sizeof(name));
    ExceptionUtil::tryToThrowHsException(ret);
    name_ = name;
}

cv::Mat Device::getDeviceImage(bool truthy) {
    uint8_t *device_image_data;
    cv::Mat mat;
    int ret = hsDeviceGetImage(
        getHandle(), reinterpret_cast<void **>(&device_image_data), truthy);
    ExceptionUtil::tryToThrowHsException(ret);
    
    if (truthy) {
        std::vector<uint8_t> result_vector(
            reinterpret_cast<uint8_t *>(device_image_data),
            reinterpret_cast<uint8_t *>(device_image_data) + 640 * 360 * 3);
        cv::Mat A(1, 640 * 360 * 3, CV_8UC1, device_image_data);
        mat = A.reshape(3, 360);
    } else {
        std::vector<uint8_t> result_vector(
            reinterpret_cast<uint8_t *>(device_image_data),
            reinterpret_cast<uint8_t *>(device_image_data) + 1920 * 1080 * 3);
        cv::Mat R(1, 1920 * 1080, CV_8UC1, device_image_data);
        cv::Mat G(1, 1920 * 1080, CV_8UC1, device_image_data + 1920 * 1080);
        cv::Mat B(1, 1920 * 1080, CV_8UC1, device_image_data + 1920 * 1080 * 2);
        std::vector<cv::Mat> channels;
        channels.push_back(B);
        channels.push_back(G);
        channels.push_back(R);
        cv::Mat bgr;
        cv::merge(channels, bgr);
        mat = bgr.reshape(3, 1080);
    }

    return mat;
}

} // namespace hs
