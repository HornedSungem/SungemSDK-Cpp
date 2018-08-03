//
//  hsapi.hpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#ifndef hsapi_hpp
#define hsapi_hpp

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "device.hpp"
#include "graph.hpp"
#include "result.hpp"

namespace hs {

class HS {
public:
    HS(int device_index, Device::LogLevel log_level,
        const std::string &cnn_type, const std::string &graph_file_path,
        const std::string &category_file_path, const int network_dimension,
        const std::vector<float> &mean, const float &scale, const int &top_n);
    ~HS();

    void classify();
    void detect();
    void loadTensor(const cv::Mat &image);
    
    ClassificationResultPtr getClassificationResult();
    DetectionResultPtr getDetectionResult();
    cv::Mat getImage(bool truthy);

private:
    void initDevice();
    void loadGraph(const std::string &graph_file_path);
    void loadCategories(const std::string &category_file_path);
    static void splitIntoLines(const std::string &content,
                               std::vector<std::string> &lines);
    static std::string getFileContent(const std::string &filename);

    Device::Ptr device_;
    Graph::Ptr graph_;
    Tensor::Ptr tensor_;
    Result::Ptr result_;

    const int device_index_;
    const Device::LogLevel log_level_;
    const std::string cnn_type_;
    std::vector<std::string> categories_;
    const int network_dimension_;
    const std::vector<float> mean_;
    const float scale_;
    const int top_n_;
    void *user_param_;
};

} // namespace hs

#endif /* hsapi_hpp */
