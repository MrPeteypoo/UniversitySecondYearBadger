#include "Waypoint.h"



// STL headers.
#include <iostream>



#pragma region Constructors

Path::Waypoint::Waypoint (Waypoint&& move)
{
    *this = std::move (move);   
}


Path::Waypoint& Path::Waypoint::operator= (Waypoint&& move)
{
    if (this != &move)
    {
        // IActor.
        m_node = std::move (move.m_node);
    }

    return *this;
}


Path::Waypoint::~Waypoint()
{
}

#pragma endregion


#pragma region Actor functionality

bool Path::Waypoint::initialise (OgreApplication* const ogre, Ogre::SceneNode* const root, const Ogre::String& name)
{    
    try
    {
        // Pre-condition: We have a valid OgreApplication and SceneNode pointer.
        if (!ogre || !root)
        {
            throw std::invalid_argument ("Path::Waypoint::initialise(), required parameter 'ogre' or 'root' is a nullptr.");
        }

        // Initialise the entity.
        const auto entity = constructEntity (ogre, "Wheel.mesh", "red");

        // Construct the node.
        m_node.reset (constructNode (root, name, entity));
        
        // We're done, yay!
        return true;
    }

    catch (const std::exception& error)
    {
        std::cerr << "An exception was caught in Path::Waypoint::initialise(): " << error.what() << std::endl;
    }

    catch (...)
    {
        std::cerr << "An unknown error occurred in Path::Waypoint::initialise()." << std::endl;
    }
    
    return false;
}

#pragma endregion