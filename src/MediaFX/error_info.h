// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDebug>
#include <QDebugStateSaver>
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
        errorInfo.Buffer = errorMessage;
        errorInfo.BufferSize = sizeof(errorMessage);
        errorInfo.ErrorType = errorType;
        errorInfo.SubType = subType;
    }

    inline void set(int errorType, int subType, const std::string& message)
    {
        set(errorType, subType);
        int count = message.copy(errorInfo.Buffer, errorInfo.BufferSize - 1);
        errorInfo.Buffer[count] = 0;
    }

    inline void reset()
    {
        set(FFMS_ERROR_SUCCESS, FFMS_ERROR_SUCCESS);
    }

    inline FFMS_ErrorInfo* operator&()
    {
        return &errorInfo;
    }

    friend QDebug inline operator<<(QDebug dbg, const ErrorInfo& error)
    {
        QDebugStateSaver saver(dbg);
        dbg.nospace() << error.errorMessage << " (error: " << error.errorInfo.ErrorType << ", sub: " << error.errorInfo.SubType << ")";
        return dbg.maybeSpace();
    }

private:
    char errorMessage[1024];
    FFMS_ErrorInfo errorInfo;
};
