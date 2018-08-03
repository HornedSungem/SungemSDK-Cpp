//
//  tensor.hpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#ifndef tensor_hpp
#define tensor_hpp

#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <utility>
#include <vector>

namespace hs {

class Tensor {
public:
    using Ptr = std::shared_ptr<Tensor>;
    using ConstPtr = std::shared_ptr<Tensor const>;

    Tensor(const std::pair<int, int> &net_dim, const std::vector<float> &mean,
           const float &scale);

    void loadImageData(const cv::Mat &image);
    void clearTensor();

    inline const uint16_t *raw() const { return &tensor_[0]; }
    inline const size_t size() const {
        return 3 * net_height_ * net_width_ * sizeof(uint16_t);
    }
    inline int getImageWidth() { return image_width_; }
    inline int getImageHeight() { return image_height_; }
#ifndef SUPPORT_F16C
    static void fp32tofp16(uint16_t *__restrict out, float in);
    static void fp16tofp32(float *__restrict out, uint16_t in);
#endif

private:
    std::vector<uint16_t> tensor_;
    int net_width_;
    int net_height_;
    int image_width_;
    int image_height_;
    std::vector<float> mean_;
    float scale_;
};

} // namespace hs

#endif /* tensor_hpp */
