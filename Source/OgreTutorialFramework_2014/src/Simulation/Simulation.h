#pragma once

#ifndef _OGRE_WORLD_H_
#define _OGRE_WORLD_H_


// STL headers.
#include <memory>


// Forward declarations.
class Badger;
class OgreApplication;


/// <summary>
/// A management class which manages the simulation required by the ICA.
/// </summary>
class Simulation final
{
    public:

        #pragma region Constructors and destructor

        Simulation();

        Simulation (Simulation&& move);
        Simulation& operator= (Simulation&& move);

        ~Simulation();

        Simulation (const Simulation& copy)             = delete;
        Simulation& operator= (const Simulation& copy)  = delete;

        #pragma endregion

        #pragma region Getters and setters
        
        /// <summary> Sets the keyboard to use when checking for input. </summary>
        void setKeyboard (const std::shared_ptr<OIS::Keyboard>& keyboard);
        
        #pragma endregion

        #pragma region Public interface

        /// <summary>
        /// Initialises the scene and every object inside the simulation.
        /// </summary>
        /// <param name="ogre"> The OgreApplication which contains the SceneManager needed to create SceneNodes. </param>
        void initialiseScene (OgreApplication* const ogre);

        /// <summary> Resets the simulation to initial values, completely restarting the simulation. </summary>
        void reset();

        /// <summary>
        /// Updates the entire simulation.
        /// </summary>
        /// <param name="deltaTime"> The value in seconds to update the simulation with. </param>
        void update (const float deltaTime);

        #pragma endregion

    private:

        #pragma region Simulation management

        /// <summary> Updates the input to ensure the correct input is being used. </summary>
        void updateInput();

        /// <summary> Updates the badger so that it can continue on its' path. </summary>
        void updateSimulation (const float deltaTime);
        
        /// <summary> Sets the desired forward speed of the wheels. </summary>
        /// <param name="value"> The value for the forward speed, this should be clamped between -1.f and 1.f. </param>
        void setSpeedRate (const float value);

        /// <summary> Sets how much the badger should turn by. </summary>
        /// <param name="value"> The value for the turn rate, this should be clamped between -1.f and 1.f. </param>
        void setTurnRate (const float value);

        #pragma endregion

        #pragma region Implmentation data
        
        std::unique_ptr<Badger>         m_badgerControlled  { nullptr };    //!< The badger vehicle used to demonstrate the use of keyboard input.
        std::unique_ptr<Badger>         m_badgerPath        { nullptr };    //!< The badger vehicle used to demonstrate the following of a bezier curve path.
        std::weak_ptr<OIS::Keyboard>    m_keyboard          {  };           //!< A weak reference to the keyboard when input is required.

        float                           m_speedRate         { 0.f };        //!< A normalised wheel speed for the Badger, from -1.f to 1.f.
        float                           m_turnRate          { 0.f };        //!< A normalised turn rate for the Badger, from -1.f to 1.f.

        #pragma endregion

};

#endif