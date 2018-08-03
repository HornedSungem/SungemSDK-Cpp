//
//  exception.cpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#include "exception.hpp"

namespace hs {

const char *HsHighThermal::what() const noexcept {
    return "lower guard temperature threshold is reached, short throtting is "
           "in action";
}

const char *HsAggressiveThermal::what() const noexcept {
    return "upper guard temperature threshold is reached, long throtting is in "
           "action";
}

const char *HSGraphFileError::what() const noexcept {
    return "cannot load graph";
}

const char *HSMeanAndStddevError::what() const noexcept {
    return "cannot load mean and stddev";
}

const char *HSInputSizeFileError::what() const noexcept {
    return "cannot open inputsize file";
}

const char *HSInputSizeError::what() const noexcept {
    return "cannot load inputsize";
}

const char *HSLoadCategoriesError::what() const noexcept {
    return "cannot load categories";
}

} // namespace hs
