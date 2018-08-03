//
//  result.hpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#ifndef result_hpp
#define result_hpp

#include <memory>
#include <string>
#include <vector>

namespace hs {
    
struct Item {
    std::string category;
    float probability;
};

struct BBox {
    int x;
    int y;
    int width;
    int height;
};

struct ItemInBBox {
    Item item;
    BBox bbox;
};

using Items = std::vector<Item>;
using ItemsPtr = std::shared_ptr<Items>;
using ItemInBBoxArray = std::vector<ItemInBBox>;
using ItemInBBoxArrayPtr = std::shared_ptr<ItemInBBoxArray>;

struct ClassificationResult {
    Items items;
    float time_taken;
};

struct DetectionResult {
    ItemInBBoxArray items_in_boxes;
    float time_taken;
};

using ClassificationResultPtr = std::shared_ptr<ClassificationResult>;
using DetectionResultPtr = std::shared_ptr<DetectionResult>;

class Result {
public:
    using Ptr = std::shared_ptr<Result>;
    using ConstPtr = std::shared_ptr<Result const>;

    explicit Result(const std::string &cnn_type);
    ClassificationResultPtr getClassificationResult();
    DetectionResultPtr getDetectionResult();
    void setClassificationResult(Item item);
    void setClassificationResult(float time);
    void setDetectionResult(ItemInBBox item);
    void setDetectionResult(float time);
    void parseYoloResult(const std::vector<float> &result,
                         const std::vector<std::string> categories,
                         int img_width, int img_height);
    void parseSSDResult(const std::vector<float> &result,
                        const std::vector<std::string> categories,
                        int img_width, int img_height);

private:
    void NMS(ItemInBBoxArrayPtr objs_in_bboxes);
    float IOU(ItemInBBox box1, ItemInBBox box2);

    ClassificationResultPtr classification_result;
    DetectionResultPtr detection_result;
};

} // namespace hs

#endif /* result_hpp */
