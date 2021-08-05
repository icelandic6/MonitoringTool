#pragma once

#include "core/AppSettings.h"
#include "core/ServerStatus.h"

namespace ui
{
    inline QColor statusColor(core::ServerStatus status)
    {
        if (status == core::ServerStatus::Available)
            return core::AppSettings::instance()->greenColor();
        else if (status == core::ServerStatus::Unstable)
            return core::AppSettings::instance()->yellowColor();
        else if (status == core::ServerStatus::Failed)
            return core::AppSettings::instance()->redColor();

        return Qt::black;
    }
}
