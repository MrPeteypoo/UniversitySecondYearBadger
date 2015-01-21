#pragma once

#ifndef _ISIMULATOR_
#define _ISIMULATOR_


// Forward declarations.
class OgreApplication;


/// <summary>
/// A basic simulator interface, useful for black-boxing simulations to ease readability and maintainability.
/// </summary>
class ISimulator
{
    public:

        #pragma region Simulator functionality

        /// <summary> Virtual destructor ensures no inheritance issues. </summary>
        virtual ~ISimulator() {}

        /// <summary> Causes the simulator to intialise itself and all actors contained within. </summary>
        /// <param name="ogre"> The OgreApplication used for interacting with the Ogre engine. </param>
        /// <returns> Returns whether the initialisation worked or not. </returns>
        virtual bool initialise (OgreApplication* const ogre) = 0;

        /// <summary> Forces the simulator to completely reset itself. </summary>
        virtual void reset() {}

        /// <summary> Updates the simulator, this should be called each frame to create a simulation. </summary>
        /// <param name="deltaTime"> The amount of time in seconds passed since the last update. </param>
        virtual void update (const float deltaTime) {}

        #pragma endregion

};

#endif // _ISIMULATOR_