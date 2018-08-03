//
//  log.hpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#ifndef log_hpp
#define log_hpp

#include <iostream>

namespace hs {

#define HS_ERROR_STREAM(str)                                                   \
    std::cout << "[ERROR] [hsapi]: " << str << std::endl

#define HS_WARN_STREAM(str) std::cout << "[WARN] [hsapi]: " << str << std::endl

#define HS_INFO_STREAM(str) std::cout << "[INFO] [hsapi]: " << str << std::endl

#define HS_ERROR(str) std::cout << "[ERROR] [hsapi]: " << str << std::endl

#define HS_WARN(str) std::cout << "[WARN] [hsapi]: " << str << std::endl

#define HS_INFO(str) std::cout << "[INFO] [hsapi]: " << str << std::endl

#define HS_DEBUG(str) std::cout << "[DEBUG] [hsapi]: " << str << std::endl

} // namespace hs

#endif /* log_hpp */
