#pragma once

#ifndef _BADGER_SIMULATOR_
#define _BADGER_SIMULATOR_


// STL headers.
#include <memory>


// Engine headers.
#include <Simulation/ISimulator.h>


// Forward declarations.
class Badger;


/// <summary>
/// A simulator which manages the keyboard-controlled badger required by the ICA.
/// </summary>
class BadgerSimulator final : public ISimulator
{
    public:

        #pragma region Constructors and destructor

        BadgerSimulator();

        BadgerSimulator (BadgerSimulator&& move);
        BadgerSimulator& operator= (BadgerSimulator&& move);

        ~BadgerSimulator() override final;

        BadgerSimulator (const BadgerSimulator& copy)             = delete;
        BadgerSimulator& operator= (const BadgerSimulator& copy)  = delete;

        #pragma endregion

        #pragma region ISimulator functionality

        /// <summary> Initialises the keyboard input and badger ready for simulation. </summary>
        /// <param name="ogre"> The OgreApplication which contains the SceneManager and Keyboard required to function. </param>
        bool initialise (OgreApplication* const ogre) override final;

        /// <summary> Resets the BadgerSimulator to initial values, completely restarting the BadgerSimulator. </summary>
        void reset() override final;

        /// <summary> Updates the entire BadgerSimulator. </summary>
        /// <param name="deltaTime"> The value in seconds to update the BadgerSimulator with. </param>
        void update (const float deltaTime) override final;

        #pragma endregion

    private:

        #pragma region Simulation management

        /// <summary> Updates the input to ensure the correct input is being used. </summary>
        void updateInput();
        
        /// <summary> Sets the desired forward speed of the wheels. </summary>
        /// <param name="value"> The value for the forward speed, this should be clamped between -1.f and 1.f. </param>
        void setSpeedRate (const float value);

        /// <summary> Sets how much the badger should turn by. </summary>
        /// <param name="value"> The value for the turn rate, this should be clamped between -1.f and 1.f. </param>
        void setTurnRate (const float value);

        #pragma endregion

        #pragma region Implmentation data
        
        std::weak_ptr<OIS::Keyboard>    m_keyboard  {  };           //!< A weak reference to the keyboard when input is required.
        std::unique_ptr<Badger>         m_badger    { nullptr };    //!< The badger vehicle used to demonstrate the use of keyboard input.

        float                           m_speedRate { 0.f };        //!< A normalised wheel speed for the Badger, from -1.f to 1.f.
        float                           m_turnRate  { 0.f };        //!< A normalised turn rate for the Badger, from -1.f to 1.f.

        #pragma endregion

};

#endif // _BADGER_SIMULATOR_