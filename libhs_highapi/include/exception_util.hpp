//
//  exception_util.hpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#ifndef exception_util_hpp
#define exception_util_hpp

#include <map>

namespace hs {

class ExceptionUtil {
public:
    static void tryToThrowHsException(int code);
};

} // namespace hs

#endif /* exception_util_hpp */
