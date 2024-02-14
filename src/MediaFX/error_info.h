// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDebug>
#include <QDebugStateSaver>
#include <array>
#include <cstdio>
#include <ffms.h>
#include <string>

class ErrorInfo {
public:
    ErrorInfo()
    {
        reset();
    }

    inline void set(int errorType, int subType)
    {
        errorInfo.Buffer = errorMessage.data();
        errorInfo.BufferSize = sizeof(errorMessage);
        errorInfo.ErrorType = errorType;
        errorInfo.SubType = subType;
    }

    inline void set(int errorType, int subType, const std::string& message)
    {
        set(errorType, subType);
        std::size_t count = message.copy(errorMessage.data(), errorMessage.size() - 1);
        errorMessage.at(count) = 0;
    }

    inline void reset()
    {
        set(FFMS_ERROR_SUCCESS, FFMS_ERROR_SUCCESS);
    }

    inline FFMS_ErrorInfo* operator&()
    {
        return &errorInfo;
    }

    friend inline QDebug& operator<<(QDebug& dbg, const ErrorInfo& error)
    {
        QDebugStateSaver saver(dbg);
        dbg.nospace() << error.errorMessage.data() << " (error: " << error.errorInfo.ErrorType << ", sub: " << error.errorInfo.SubType << ")";
        return dbg.maybeSpace();
    }

private:
    static constexpr int MAX_ERROR = 1024;
    std::array<char, MAX_ERROR> errorMessage { 0 };
    FFMS_ErrorInfo errorInfo { 0 };
};
