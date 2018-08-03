//
//  graph.hpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#ifndef graph_hpp
#define graph_hpp

#include <opencv2/opencv.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "device.hpp"
#include "result.hpp"
#include "tensor.hpp"

namespace hs {

class Graph {
public:
    using Ptr = std::shared_ptr<Graph>;
    using ConstPtr = std::shared_ptr<Graph const>;

    Graph(const Device::Ptr &device, const std::string &graph_file,
          int network_dimension);
    ~Graph();

    void allocate(void *device_handle);
    void deallocate();
    float getTimeTaken();
    std::string getDebugInfo();
    void *getHandle();

    int getNetworkDim() const { return network_dimension_; }

    cv::Mat getGraphImage(void *user_param, float std, float mean, bool truthy);

private:
    std::string graph_buf_;
    const int network_dimension_;
    void *handle_;
};

} // namespace hs

#endif /* graph_hpp */
