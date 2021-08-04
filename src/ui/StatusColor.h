#pragma once

#include "core/AppSettings.h"
#include "core/ServerStatus.h"

#include <QColor>

namespace ui
{
    inline QColor statusColor(core::ServerStatus status)
    {
        auto settings = core::AppSettings::instance();

        if (status == core::ServerStatus::Available)
            return settings->config().greenColor;
        else if (status == core::ServerStatus::Unstable)
            return settings->config().yellowColor;
        else if (status == core::ServerStatus::Failed)
            return settings->config().redColor;

        return QColor(Qt::black);
    }
}
