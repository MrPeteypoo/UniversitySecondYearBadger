#pragma once

#ifndef _BADGER_HANDLE_BAR_
#define _BADGER_HANDLE_BAR_


// Engine headers.
#include <Badger/IRotatableComponent.h>


/// <summary>
/// A representation of the handle bars for a Badger vehicle. They simply rotate left and right +- 20 degrees.
/// </summary>
class Badger::HandleBar final : public Badger::IRotatableComponent
{
    public:
        
        #pragma region Constructors and destructor

	    HandleBar()                                     = default;

        HandleBar (HandleBar&& move);
        HandleBar& operator= (HandleBar&& move);

	    ~HandleBar() override final;

        HandleBar (const HandleBar& copy)               = delete;
        HandleBar& operator= (const HandleBar& copy)    = delete;

        #pragma endregion

        #pragma region Getters and setters

        /// <summary> Will reset the rotation animator to stationary settings. </summary>
        void reset();

        #pragma endregion
        
        #pragma region Actor functionality
                
        /// <summary> Initialises the HandleBar, this will load the object at a default position. </summary>
        /// <param name="ogre"> The OgreApplication used for creating entities. </param>
        /// <param name="root"> The SceneNode to attach the HandleBar to. </param>
        /// <param name="name"> The unique name for the HandleBar scene node. </param>
        /// <returns> Returns whether the initialisation was successful. </returns>
        bool initialise (OgreApplication* const ogre = nullptr, Ogre::SceneNode* const root = nullptr, const Ogre::String& name = { }) override final;

        /// <summary> This will ensure that the orientation of the handle bars proceeds to the target point as time passes. </summary>
        /// <param name="deltaTime"> The number of seconds passed since the last update. </param>
        void updateSimulation (const float deltaTime) override final;

        #pragma endregion

};


#endif // _BADGER_HANDLE_BAR_