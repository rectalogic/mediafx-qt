/*
 * Copyright (C) 2023 Andrew Wason
 *
 * This file is part of mediaFX.
 *
 * mediaFX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with mediaFX.
 * If not, see <https://www.gnu.org/licenses/>.
 */
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
