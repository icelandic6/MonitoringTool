#pragma once

namespace core
{
    enum class ServerStatus
    {
        Available,
        Unstable,
        Failed
    };

    inline bool operator<(ServerStatus l, ServerStatus r)
    {
        if (l == r)
            return false;
        else if (l == ServerStatus::Available && r != ServerStatus::Available)
            return false;
        else if (l == ServerStatus::Failed && r != ServerStatus::Failed)
            return true;
        else if (r == ServerStatus::Available)
            return true;
        else
            return false;
    }
}
