//
//  main.cpp
//  mobilenetssd
//
//  Created by horned-sungem on 2018/7/5.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "hsapi.hpp"

int main(int argc, const char *argv[])
{
    bool isTheDevHS = true;
    cv::VideoCapture capture;

    if (!isTheDevHS)
    {
        capture.open(1);
    }

    std::string graph;
    std::string category;

    std::cout << "Please input the filepath of graph: ";
    std::cin >> graph;
    std::cout << "Please input the filepath of categories: ";
    std::cin >> category;

    std::vector<float> mean = std::vector<float>{127.5, 127.5, 127.5};

    hs::HS hs = hs::HS(0, hs::Device::LogLevel::Verbose,
                       "mobilenetssd", graph,
                       category, 300,
                       mean, 0.007843, 0);
    cv::Mat cvImage;
    do
    {

        if (isTheDevHS)
        {
            cvImage = hs.getImage(true);
        }
        else
        {
            capture >> cvImage;
            hs.loadTensor(cvImage);
        }
        
        int width = cvImage.cols;
        int height = cvImage.rows;

        hs.detect();
        hs::DetectionResultPtr result = hs.getDetectionResult();

        for (auto obj : result->items_in_boxes)
        {
            std::stringstream ss;

            ss << obj.item.category << ": " << obj.item.probability * 100 << '%';
            int xmin = obj.bbox.x;
            int ymin = obj.bbox.y;
            int w = obj.bbox.width;
            int h = obj.bbox.height;

            int xmax = ((xmin + w) < width) ? (xmin + w) : width;
            int ymax = ((ymin + h) < height) ? (ymin + h) : height;

            if (xmax > width * 0.8 && ymax > height * 0.8)
            {
                continue;
            }

            cv::Point left_top = cv::Point(xmin, ymin);
            cv::Point right_bottom = cv::Point(xmax, ymax);
            cv::rectangle(cvImage, left_top, right_bottom, cv::Scalar(0, 255, 0), 1);
            cv::rectangle(cvImage, cvPoint(xmin, ymin), cvPoint(xmax, ymin + 20), cv::Scalar(0, 255, 0), -1);
            cv::putText(cvImage, ss.str(), cvPoint(xmin + 5, ymin + 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 1);
        }
        cv::imshow("press any key to exit", cvImage);
    } while (int(cv::waitKey(1) & 0xff) == 255);

    return 0;
}
