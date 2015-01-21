#pragma once

#ifndef _SIMULATION_
#define _SIMULATION_


// STL headers.
#include <vector>


// Forward declarations.
class OgreApplication;
class ISimulator;


/// <summary>
/// An encapsulation for the simulation systems in the application.
/// </summary>
class Simulation final
{
    public:

        #pragma region Constructors and destruction

        Simulation() = default;

        Simulation (Simulation&& move);
        Simulation& operator= (Simulation&& move);
        
        ~Simulation();

        Simulation (const Simulation& copy)             = delete;
        Simulation& operator= (const Simulation& copy)  = delete;

        #pragma endregion

        #pragma region Public interface

        /// <summary> Causes the simulation to initialise. </summary>
        /// <param name="ogre"> The OgreApplication used for interacting with the Ogre engine. </param>
        /// <returns> Returns whether the initialisation worked or not. </returns>
        void initialise (OgreApplication* const ogre);

        /// <summary> Forces the simulation to completely reset. </summary>
        void reset();

        /// <summary> Updates the simulation; this should be called each frame. </summary>
        /// <param name="deltaTime"> The amount of time in seconds passed since the last update. </param>
        void update (const float deltaTime);

        #pragma endregion

    private:

        #pragma region Implementation data

        std::vector<ISimulator*>    m_simulators    { };    //!< A container of each simulator in the application.

        #pragma endregion

};

#endif // _SIMULATION_