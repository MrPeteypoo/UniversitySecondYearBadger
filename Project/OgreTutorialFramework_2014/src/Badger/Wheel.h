#pragma once

#ifndef _BADGER_WHEEL_
#define _BADGER_WHEEL_


// Engine headers.
#include <Badger/IRotatableComponent.h>


/// <summary>
/// A basic wheel class which will continously roll forward
/// </summary>
class Badger::Wheel final : public Badger::IRotatableComponent
{
    public:
        
        #pragma region Constructors and destructor

	    Wheel();

        Wheel (Wheel&& move);
        Wheel& operator= (Wheel&& move);

	    ~Wheel();

        Wheel (const Wheel& copy)             = delete;
        Wheel& operator= (const Wheel& copy)  = delete;

        #pragma endregion

        #pragma region Getters and setters
  
        float getDiameter() const                       { return m_diameter; }
        float getRevolveModifier() const                { return m_revolveModifier; }

        /// <summary> Sets the wheels diameter, this effects how fast the wheels should revolve based on a distance. </summary>
        void setDiameter (const float diameter)         { m_diameter = diameter; }

        /// <summary> Used to change how revolutions are applied. -1.f will cause a reverse rotation. </summary>
        void setRevolveModifier (const float modifier)  { m_revolveModifier = modifier; }

        /// <summary> Will reset the rotation animator to stationary settings. </summary>
        void reset();

        #pragma endregion
        
        #pragma region Actor functionality
                
        /// <summary>
        /// Initialises the Wheel, this will load the object at a default position.
        /// <returns> Returns whether the initialisation was successful. </returns>
        /// </summary>
        /// <param name="ogre"> The OgreApplication used for creating entities. </param>
        /// <param name="root"> The SceneNode to attach the Wheel to. </param>
        bool initialise (OgreApplication* const ogre = nullptr, Ogre::SceneNode* const root = nullptr, const Ogre::String& name = { }) override final;

        /// <summary>
        /// This will ensure that the orientation of the wheels proceeds to the target point as time passes.
        /// </summary>
        /// <param name="deltaTime"> The number of seconds passed since the last update. </param>
        void updateSimulation (const float deltaTime) override final;

        #pragma endregion

        #pragma region Simulation

        /// <summary> Revolves the wheels to indicate a forward motion based on distance value. </summary>
        /// <param name="distance"> A distance in metres to simulate. </param>
        void revolve (const float distance);

        #pragma endregion

    private:

        #pragma region Implementation data

        float       m_diameter          { 7.f };    //!< The wheel diameter in metres. 
        float       m_revolveModifier   { 1.f };    //!< A modifier used to change the way revolutions are applied. Should be clamped between -1.f and 1.f, fixes wrong rotation direction.

        #pragma endregion

};


#endif // _BADGER_WHEEL_