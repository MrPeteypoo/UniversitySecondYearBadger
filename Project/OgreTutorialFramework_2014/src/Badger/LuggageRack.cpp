#include "LuggageRack.h"



// STL headers.
#include <exception>
#include <iostream>



#pragma region Constructors and destructor

Badger::LuggageRack::LuggageRack()
{
}


Badger::LuggageRack::LuggageRack (LuggageRack&& move)
{
    *this = std::move (move);
}


Badger::LuggageRack& Badger::LuggageRack::operator= (LuggageRack&& move)
{
    if (this != &move)
    {
        // IActor
        m_node = std::move (move.m_node);
    }

    return *this;
}


Badger::LuggageRack::~LuggageRack()
{
}

#pragma endregion


#pragma region Actor functionality

bool Badger::LuggageRack::initialise (OgreApplication* const ogre, Ogre::SceneNode* const root, const Ogre::String& name)
{
    try
    {
        // Pre-condition: We have a valid OgreApplication and SceneNode pointer.
        if (!ogre || !root)
        {
            throw std::invalid_argument ("Badger::LuggageRack::initialise(), required parameter 'ogre' or 'root' is a nullptr.");
        }
        
        // Create parameters.
        const Ogre::Vector3     position    { 0.f, 0.018f, -0.024f };
        const Ogre::Quaternion  orientation {  };
        const Ogre::Vector3     scale       { 1.f, 1.f, 1.f };

        // Initialise the entity.
        const auto entity = constructEntity (ogre, "luggage_rack.mesh");

        // Construct the node.
        m_node.reset (constructNode (root, name, entity, position, orientation, scale));
        
        // Simples!
        return true;
    }

    catch (std::exception& error)
    {
        std::cerr << "An exception was caught in Badger::LuggageRack::initialise(): " << error.what() << std::endl;
    }

    catch (...)
    {
        std::cerr << "An unknown error occurred in Badger::LuggageRack::initialise()." << std::endl;
    }
    
    return false;
}

#pragma endregion