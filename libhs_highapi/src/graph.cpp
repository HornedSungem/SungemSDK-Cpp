//
//  graph.cpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#if defined(__i386__) || defined(__x86_64__)
#include <x86intrin.h>
#endif
#include "exception.hpp"
#include "exception_util.hpp"
#include "graph.hpp"
#include "log.hpp"
#include "tensor.hpp"
#include "hs_cpp.hpp"

namespace hs {

Graph::Graph(const std::shared_ptr<Device> &device,
             const std::string &graph_buf, int network_dimension)
    : graph_buf_(graph_buf), network_dimension_(network_dimension),
      handle_(nullptr) {
    allocate(device->getHandle());
}

Graph::~Graph() {
    try {
        deallocate();
    } catch (HSException &e) {
        HS_ERROR_STREAM(e.what());
    }
}

std::string Graph::getDebugInfo() {
    assert(handle_ != nullptr);
    char *debug_info;
    unsigned int length;
    int ret = hsGetGraphOption(handle_, HS_DEBUG_INFO,
                               reinterpret_cast<void **>(&debug_info), &length);
    ExceptionUtil::tryToThrowHsException(ret);
    std::string result(debug_info);
    return result;
}

float Graph::getTimeTaken() {
    assert(handle_ != nullptr);
    float *time_taken;
    unsigned int length;
    int ret = hsGetGraphOption(handle_, HS_TIME_TAKEN,
                               reinterpret_cast<void **>(&time_taken), &length);
    ExceptionUtil::tryToThrowHsException(ret);
    length /= sizeof(*time_taken);
    float sum = 0;

    for (unsigned int i = 0; i < length; ++i) {
        sum += time_taken[i];
    }

    return sum;
}

void *Graph::getHandle() {
    assert(handle_ != nullptr);
    return handle_;
}

void Graph::allocate(void *device_handle) {
    int ret = hsAllocateGraph(device_handle, &handle_, graph_buf_.c_str(),
                              graph_buf_.size());
    ExceptionUtil::tryToThrowHsException(ret);
}

void Graph::deallocate() {
    int ret = hsDeallocateGraph(handle_);
    ExceptionUtil::tryToThrowHsException(ret);
}

cv::Mat Graph::getGraphImage(void *user_param, float std, float mean,
                             bool truthy) {
    uint8_t *image_data;
    unsigned int length;
    cv::Mat mat;

    int ret = hsGetImage(getHandle(), reinterpret_cast<void **>(&image_data),
                         user_param, std, mean, truthy);
    ExceptionUtil::tryToThrowHsException(ret);
    if (truthy) {
        length = 360 * 640 * 3;
    } else {
        length = 1920 * 1080 * 3;
    }
    std::vector<uint8_t> result_vector(reinterpret_cast<uint8_t *>(image_data),
                                       reinterpret_cast<uint8_t *>(image_data) +
                                           length);
    if (truthy) {
        cv::Mat A(1, length, CV_8UC1, image_data);
        mat = A.reshape(3, 360);
    } else {
        cv::Mat R(1, 1920 * 1080, CV_8UC1, image_data);
        cv::Mat G(1, 1920 * 1080, CV_8UC1, image_data + 1920 * 1080);
        cv::Mat B(1, 1920 * 1080, CV_8UC1, image_data + 1920 * 1080 * 2);
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
