//
//  exception_util.cpp
//  libhsapi
//
//  Created by horned-sungem on 2018/7/2.
//  Copyright © 2018年 Senscape. All rights reserved.
//

#include "exception_util.hpp"
#include "hs_cpp.hpp"
#include <cassert>
#include <exception.hpp>
#include <map>
#include <string>

namespace hs {

std::map<int, std::string> CODE2STR = {
    {HS_BUSY, "device is busy, retry later"},
    {HS_ERROR, "an unexpected error was encontered during the function call"},
    {HS_OUT_OF_MEMORY, "the host is out of memory"},
    {HS_DEVICE_NOT_FOUND, "there is no device at the given index or name"},
    {HS_INVALID_PARAMETERS, "at least one of the given parameters is invalid "
                            "in the context of the function call"},
    {HS_TIMEOUT, "timeout in the communication with the device"},
    {HS_MVCMD_NOT_FOUND,
     "the file named MvNCAPI.mvcmd should be installed in the hs direcotry"},
    {HS_NO_DATA, "no data to return"},
    {HS_GONE, "the graph or device has been closed during the operation"},
    {HS_UNSUPPORTED_GRAPH_FILE,
     "the graph file may have been created with an incompatible prior version "
     "of the Toolkit"},
    {HS_MYRIAD_ERROR, "an error has been reported by VPU"}};

void ExceptionUtil::tryToThrowHsException(int code) {
    assert(HS_OK >= code && code >= HS_MYRIAD_ERROR);

    try {
        const std::string msg = CODE2STR.at(code);

        if (code == HS_BUSY) {
            throw HsBusy(msg);
        }

        if (code == HS_ERROR) {
            throw HsError(msg);
        }

        if (code == HS_OUT_OF_MEMORY) {
            throw HsOutOfMemory(msg);
        }

        if (code == HS_DEVICE_NOT_FOUND) {
            throw HsDeviceNotFound(msg);
        }

        if (code == HS_INVALID_PARAMETERS) {
            throw HsInvalidParameters(msg);
        }

        if (code == HS_TIMEOUT) {
            throw HsTimeout(msg);
        }

        if (code == HS_MVCMD_NOT_FOUND) {
            throw HsMvCmdNotFound(msg);
        }

        if (code == HS_NO_DATA) {
            throw HsNoData(msg);
        }

        if (code == HS_GONE) {
            throw HsGone(msg);
        }

        if (code == HS_UNSUPPORTED_GRAPH_FILE) {
            throw HsUnsupportedGraphFile(msg);
        }

        if (code == HS_MYRIAD_ERROR) {
            throw HsMyriadError(msg);
        }
    } catch (std::out_of_range &e) {
    }
}

} // namespace hs
