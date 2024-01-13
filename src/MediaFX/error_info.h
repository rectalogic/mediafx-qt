// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDebug>
#include <QDebugStateSaver>
#include <ffms.h>

class ErrorInfo {
public:
    ErrorInfo()
    {
        reset();
    }

    inline void reset()
    {
        errorInfo.Buffer = errorMessage;
        errorInfo.BufferSize = sizeof(errorMessage);
        errorInfo.ErrorType = FFMS_ERROR_SUCCESS;
        errorInfo.SubType = FFMS_ERROR_SUCCESS;
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
