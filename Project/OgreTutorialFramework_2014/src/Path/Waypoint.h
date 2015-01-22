#pragma once

#ifndef _WAYPOINT_
#define _WAYPOINT_


// Engine headers.
#include <Misc/IActor.h>


class Waypoint final : public IActor
{
    public:

        #pragma region Constructors and destructor

        Waypoint() = default;

        Waypoint (Waypoint&& move);
        Waypoint& operator= (Waypoint&& move);
        
        ~Waypoint();

        Waypoint (const Waypoint& copy)             = delete;
        Waypoint& operator= (const Waypoint& copy)  = delete;

        #pragma endregion

};

#endif // _WAYPOINT_