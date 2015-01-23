#pragma once

#ifndef _PATH_WAYPOINT_
#define _PATH_WAYPOINT_


// Engine headers.
#include <Misc/IActor.h>
#include <Path/Path.h>


/// <summary>
/// A simple class which is a visual representation of a waypoint on the path.
/// </summary>
class Path::Waypoint final : public IActor
{
    public:

        #pragma region Constructors and destructor

        Waypoint()                                  = default;

        Waypoint (Waypoint&& move);
        Waypoint& operator= (Waypoint&& move);
        
        ~Waypoint() override final;

        Waypoint (const Waypoint& copy)             = delete;
        Waypoint& operator= (const Waypoint& copy)  = delete;

        #pragma endregion

        #pragma region Actor functionality

        /// <summary> Initialises the waypoint, this will load the object at a default position. </summary>
        /// <param name="ogre"> The OgreApplication used for creating entities. </param>
        /// <param name="root"> The SceneNode to attach the waypoint to. </param>
        /// <param name="name"> The unique name for the waypoint scene node. </param>
        /// <returns> Returns whether the initialisation was successful. </returns>
        bool initialise (OgreApplication* const ogre = nullptr, Ogre::SceneNode* const root = nullptr, const Ogre::String& name = { }) override final;

        #pragma endregion

};

#endif // _PATH_WAYPOINT_