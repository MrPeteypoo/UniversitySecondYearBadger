#pragma once

#ifndef _BADGER_LUGGAGE_RACK_
#define _BADGER_LUGGAGE_RACK_


// Engine headers.
#include <Badger/Badger.h>


/// <summary>
/// A static object which represents the luggage rack of a Badger, simply for graphical effect.
/// </summary>
class Badger::LuggageRack final : public IActor
{
    public:
        
        #pragma region Constructors and destructor

	    LuggageRack()                                       = default;

        LuggageRack (LuggageRack&& move);
        LuggageRack& operator= (LuggageRack&& move);

	    ~LuggageRack() override final;

        LuggageRack (const LuggageRack& copy)               = delete;
        LuggageRack& operator= (const LuggageRack& copy)    = delete;

        #pragma endregion
        
        #pragma region Actor functionality
                
        /// <summary> Initialises the LuggageRack, this will load the object at a default position. </summary>
        /// <param name="ogre"> The OgreApplication used for creating entities. </param>
        /// <param name="root"> The SceneNode to attach the LuggageRack to. </param>
        /// <param name="name"> The unique name given to the SceneNode upon intialisation. </param>
        /// <returns> Returns whether the initialisation was successful. </returns>
        bool initialise (OgreApplication* const ogre = nullptr, Ogre::SceneNode* const root = nullptr, const Ogre::String& name = { }) override final;

        #pragma endregion

};


#endif // _BADGER_LUGGAGE_RACK_