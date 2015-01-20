#pragma once

#ifndef _BADGER_
#define _BADGER_


// Engine headers.
#include <Misc/IActor.h>
#include <Simulation/TimeTracker.h>


/// <summary>
/// A simple class which represents a "Badger" vehicle in-game.
/// </summary>
class Badger final : public IActor
{
    public:
        
        #pragma region Constructors and destructor

        Badger();

        Badger (Badger&& move);
        Badger& operator= (Badger&& move);

        ~Badger();

        Badger (const Badger& copy)             = delete;
        Badger& operator= (const Badger& copy)  = delete;

        #pragma endregion
        
        #pragma region Getters and setters

        /// <summary> Sets the maximum speed the badger can reach. </summary>
        void setMaxSpeed (const float speed)    { m_maxSpeed = speed; }

        /// <summary> Sets the target speed of the Badger. This will be achieved over time. </summary>
        /// <param name="speed"> This will be clamped between -1.f and 1.f. </param>
        void setSpeedRate (const float speed);

        /// <summary> Sets how much the wheels and handle bars of the badger should be turned. </summary>
        /// <param name="turn"> This will be clamped between -1.f and 1.f. </param>
        void setTurnRate (const float turn);

        /// <summary> Will reset the position and orientation of the Badger to its intial values. </summary>
        void reset();

        #pragma endregion
        
        #pragma region Actor functionality
                
        /// <summary>
        /// Initialises the Badger, this will load the entire vehicle and place it at a default position.
        /// <returns> Returns whether the initialisation was successful. </returns>
        /// </summary>
        /// <param name="ogre"> The OgreApplication used for creating entities. </param>
        /// <param name="root"> The SceneNode to attach the Badger to. </param>
        bool initialise (OgreApplication* const ogre = nullptr, Ogre::SceneNode* const root = nullptr, const Ogre::String& name = { }) override final;

        /// <summary> 
        /// Moves and rotates the badger and each component according to its current state.
        /// </summary>
        /// <param name="deltaTime"> The number of seconds passed since the last update. </param>
        void updateSimulation (const float deltaTime) override final;

        #pragma endregion

    private:

        #pragma region Simulation

        /// <summary> Moves the badger forward according to its speed. </summary>
        /// <param name="modifier"> A modifier used to move the badger in the correct direction. </param>
        void moveForward (const float modifier, const float deltaTime);

        /// <summary> Rotates the badger according to the current wheel rotation. </summary>
        /// <param name="modifier"> A modifier used to rotate the badger in the correct direction and at the right pace. </param>
        void rotate (const float modifier, const float deltaTime);

        #pragma endregion

        #pragma region Helper functions

        /// <summary> Creates the actual child nodes. </summary>
        void createChildren (OgreApplication* const ogre, const Ogre::String& name);

        /// <summary> Sets the correct position and orientation of each wheel. </summary>
        void setupWheels();

        #pragma endregion
        
        #pragma region Implementation data

        // Forward declarations.
        class IRotatableComponent;
        class HandleBar;
        class LuggageRack;
        class Wheel;


        std::unique_ptr<HandleBar>                      m_handleBar     { nullptr };    //!< The moving handle bars of the Badger.
        std::unique_ptr<LuggageRack>                    m_luggageRack   { nullptr };    //!< The static luggage rack of the Badger.
        std::vector<std::unique_ptr<Wheel>>             m_wheels        {  };           //!< The four wheels of the Badger, front wheels come first.
        
        TimeTracker                                     m_tracker       {  };           //!< Used for creating a smooth transition during movement.
        float                                           m_maxSpeed      { 60.f };       //!< The vehicle speed in meters per second.

        #pragma endregion

};

#endif // _BADGER_