#pragma once

#ifndef _BADGER_IROTATABLE_COMPONENT_
#define _BADGER_IROTATABLE_COMPONENT_


// Engine headers.
#include <Badger/Badger.h>


/// <summary>
/// An abstract class used to provide the handle bars and wheels with the same rotation mechanics.
/// </summary>
class Badger::IRotatableComponent : public IActor
{
    public:
        
        /// <summary> Keep the destructor virtual for child classes. </summary>
        virtual ~IRotatableComponent() { };

        #pragma region Getters and setters

        /// <summary> Gets the turn speed of the component. </summary>
        float getTurnSpeed() const                  { return m_turnSpeed; }

        /// <summary> Gets the target turn value which is a normalised value between -1.f and 1.f. </summary>
        float getTargetTurn() const                 { return m_targetTurn; }

        /// <summary> Calculates the current yaw of the wheels. </summary>
        float currentYaw() const;

        /// <summary> Sets the sideways turn speed of the wheels. Negative values may have strange outcomes. </summary>
        void setTurnSpeed (const float speed)       { m_turnSpeed = speed; }

        /// <summary> Sets the target X rotation using a value between -1.f and 1.f, 0.f meaning neutral. </summary>
        void setTargetTurn (const float target)     { m_targetTurn = target; }

        #pragma endregion

    protected:

        #pragma region Animation

        /// <summary> Rotates the object on a given axis based on the target turn and turn speed values available. </summary>
        /// <param name="axis"> The axis the component should be rotated on. </param>
        /// <param name="space"> How the rotation should be applied to the component, commonly TS_PARENT and TS_LOCAL. </param>
        /// <param name="deltaTime"> The delta time used to normalise the rotation speed. </param>
        void rotateComponent (const Ogre::Vector3& axis, const Ogre::Node::TransformSpace space, const float deltaTime);
        
        #pragma endregion

        #pragma region Implementation data
        
        float       m_turnSpeed     { 80.f };   //!< The turn rate at which the component rotates sideways in degrees per second.
        float       m_targetTurn    { 0.f };    //!< A normalised target turn rotation between -1.f and 1.f.

        #pragma endregion

};

#endif // _BADGER_IROTATABLE_COMPONENT_