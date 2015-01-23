#pragma once

#ifndef _BADGER_
#define _BADGER_


// Engine headers.
#include <Misc/IActor.h>


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

        ~Badger() override final;

        Badger (const Badger& copy)             = delete;
        Badger& operator= (const Badger& copy)  = delete;

        #pragma endregion
        
        #pragma region Getters and setters

        float getWheelBase() const              { return m_wheelBase; }
        float getAcceleration() const           { return m_acceleration; }
        float getBrakePower() const             { return m_brakePower; }
        float getCurrentSpeed() const           { return m_currentSpeed; }
        float getMaxSpeed() const               { return m_maxSpeed; }


        /// <summary> Sets the maximum speed the badger can reach. </summary>
        void setMaxSpeed (const float speed)    { m_maxSpeed = speed; }

        /// <summary> Sets the target speed rate of the maximum speed. This will be achieved over time. </summary>
        /// <param name="speed"> This will be clamped between -1.f and 1.f. </param>
        void setSpeedRate (const float speed);

        /// <summary> Sets how much the wheels and handle bars of the badger should be turned. </summary>
        /// <param name="turn"> This will be clamped between -1.f and 1.f. </param>
        void setTurnRate (const float turn);

        /// <summary> Will reset the position and orientation of the Badger to its intial values. </summary>
        void reset();

        #pragma endregion
        
        #pragma region Actor functionality
                
        /// <summary> Initialises the Badger, this will load the entire vehicle and place it at a default position. </summary>
        /// <returns> Returns whether the initialisation was successful. </returns>
        /// <param name="ogre"> The OgreApplication used for creating entities. </param>
        /// <param name="root"> The SceneNode to attach the Badger to. </param>
        /// <param name="name"> The unique name to give the badger. </param>
        bool initialise (OgreApplication* const ogre = nullptr, Ogre::SceneNode* const root = nullptr, const Ogre::String& name = { }) override final;

        /// <summary> Moves and rotates the badger and each component according to its current state. </summary>
        /// <param name="deltaTime"> The number of seconds passed since the last update. </param>
        void updateSimulation (const float deltaTime) override final;

        #pragma endregion

    private:

        #pragma region Simulation

        /// <summary> Updates the current speed before simulating movement. </summary>
        void updateSpeed (const float deltaTime);

        /// <summary> Moves the badger forward according to its speed. </summary>
        /// <param name="distance"> The distance in metres that the badger should travel. </param>
        void moveForward (const float distance);

        /// <summary> Rotates the badger according to the current wheel rotation. </summary>
        /// <param name="distance"> The distance in metres that the badger has travelled. </param>
        void rotate (const float distance);

        #pragma endregion

        #pragma region Helper functions

        /// <summary> Creates the actual child nodes. </summary>
        void createChildren (OgreApplication* const ogre, const Ogre::String& name);

        /// <summary> Sets the correct position and orientation of each wheel. </summary>
        void setupWheels();

        /// <summary> Sets the current speed value whilst ensuring its clamped to the max speed. </summary>
        void setCurrentSpeed (const float speed);

        #pragma endregion
        
        #pragma region Implementation data

        // Forward declarations.
        class IRotatableComponent;
        class HandleBar;
        class LuggageRack;
        class Wheel;


        std::unique_ptr<HandleBar>          m_handleBar         { nullptr };    //!< The moving handle bars of the Badger.
        std::unique_ptr<LuggageRack>        m_luggageRack       { nullptr };    //!< The static luggage rack of the Badger.
        std::vector<std::unique_ptr<Wheel>> m_wheels            {  };           //!< The four wheels of the Badger, front wheels come first.
        
        float                               m_wheelBase         { 8.f };        //!< The length between the front and rear wheels.

        float                               m_acceleration      { 15.f };       //!< How quickly the badger can change its current speed.
        float                               m_brakePower        { 50.f };       //!< How quickly the badger can break.
        
        float                               m_currentSpeed      { 0.f };        //!< Keeps track of how quickly the badger is currently moving.
        float                               m_maxSpeed          { 80.f };       //!< The vehicle speed in meters per second.
        float                               m_targetSpeedRate   { 0.f };        //!< A normalised value between -1.f and 1.f which represents the desired max speed value.


        #pragma endregion

};

#endif // _BADGER_