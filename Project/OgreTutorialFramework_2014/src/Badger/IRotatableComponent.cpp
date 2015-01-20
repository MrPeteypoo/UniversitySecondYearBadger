#include "IRotatableComponent.h"



// Engine headers.
#include <Utility/Maths.h>



// Globals.
const float angleLimit  { 0.34906585f };    //!< The angle limit used when rotation the component.
const float tolerance   { 0.0125f };        //!< The tolerance used in detecting whether the angles are correct.



#pragma region Getters and setters

float Badger::IRotatableComponent::currentYaw() const
{
    // Calculate the yaw value.
    const float yaw { util::fixYaw (m_node->getOrientation().getYaw().valueRadians(), angleLimit + tolerance * 2.f) };

    // Return zero to prevent yaw values being used to rotate the Badger when it shouldn't.
    return util::roughlyEquals (yaw, tolerance) ? 0.f : yaw;
}

#pragma endregion


#pragma region Animation

void Badger::IRotatableComponent::rotateComponent (const Ogre::Vector3& axis, const Ogre::Node::TransformSpace space, const float deltaTime)
{
    // Ensure we have a valid yaw value by clamping it.
    const float yaw             { util::fixYaw (m_node->getOrientation().getYaw().valueRadians(), angleLimit + tolerance * 2.f) };
        
    // Calculate the desired angle for the wheels. It should scale from -20* to 20*. We need to reverse the value so the yaw matches.
    const float desiredAngle    { -(angleLimit * m_targetTurn) };
        
    // Reduce the tolerance when checking if we're close to stop the wheels freaking out. I can't do this anymore, three days, I can't tweak this anymore to be more "realistic". I need my bed.....and Gossip Girl.
    if (!util::roughlyEquals (yaw, desiredAngle, tolerance))
    {
        // Prevent shooting past the limit due to high speed/deltaTime by using a slower rotation speed if necessary.
        const float radianSpeed         { m_turnSpeed * Ogre::Math::fDeg2Rad * deltaTime };
        const float difference          { std::abs (desiredAngle - yaw) };
        const float turnSpeed           { radianSpeed > difference ? difference : radianSpeed };

        // Turn negatively when necessary!
        const float finalSpeed          { yaw > desiredAngle ? -turnSpeed : turnSpeed };
                
        // Create the finished rotation based on the calculated speed.
        const Ogre::Quaternion rotation { Ogre::Radian (finalSpeed), axis };

        // Finally rotate the wheel.
        m_node->rotate (rotation, space);
    }
}

#pragma endregion