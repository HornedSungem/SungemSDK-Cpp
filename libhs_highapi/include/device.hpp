//
//  device.hpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#ifndef device_hpp
#define device_hpp

#include <opencv2/opencv.hpp>
#include <memory>
#include <string>

namespace hs {
    
class Device {
public:
    using Ptr = std::shared_ptr<Device>;
    using ConstPtr = std::shared_ptr<Device const>;

    enum LogLevel { Nothing, Errors, Verbose };

    Device(int index, LogLevel log_level);
    ~Device();

    void setLogLevel(LogLevel level);
    LogLevel getLogLevel();
    void *getHandle();
    std::string getName() const;
    void monitorThermal() const;
    
    cv::Mat getDeviceImage(bool truthy);

private:
    enum ThermalThrottlingLevel { Normal, High, Aggressive };

    void open();
    void close();

    void find();
    ThermalThrottlingLevel getThermalThrottlingLevel() const;

    int index_;
    std::string name_;
    void *handle_;
};
} // namespace hs

#endif /* device_hpp */
