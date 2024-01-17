// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#include "interval.h"

/*!
    \qmlvaluetype interval
    \ingroup qmlvaluetypes
    \inqmlmodule MediaFX
    \brief Specifies a time interval.

    interval specifis a start and end time, in milliseconds.
*/

/*!
    \qmlproperty int interval::start

    The start time of the interval, in milliseconds.
*/

/*!
    \qmlproperty int interval::end

    The end time of the interval, in milliseconds.
*/

/*!
    \qmlproperty int interval::duration

    The duration of the interval, in milliseconds.
    This is (\l end - \l start).
*/

/*!
    \qmlmethod bool interval::contains(int time)

    Returns \c true if \a time (milliseconds) is contained within the interval.
*/

/*!
    \qmlmethod bool interval::containedBy(int start, int end)

    Returns \c true if the interval is contained by \a start and \a end times (milliseconds).
*/