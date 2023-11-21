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

#include "clip.h"
#include <QObject>
#include <QtQmlIntegration>
class QUrl;
struct Interval;

class AudioClip : public Clip {
    Q_OBJECT
    QML_ELEMENT

public:
    using Clip::Clip;
    explicit AudioClip(QObject* parent = nullptr)
        : Clip(parent) {};

protected:
    void setActive(bool active) override;

    void loadMedia(const QUrl&) override;
    bool renderClip(const Interval& globalTime) override;

    virtual void stop() override;

private:
};
