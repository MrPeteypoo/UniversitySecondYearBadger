#include "Badger.h"



// STL headers.
#include <exception>
#include <iostream>



// Engine headers.
#include <Badger/HandleBar.h>
#include <Badger/LuggageRack.h>
#include <Badger/Wheel.h>
#include <Framework/OgreApplication.h>
#include <Utility/Maths.h>



#pragma region Constructors and destructor

Badger::Badger()
{
}


Badger::Badger (Badger&& move)
{
    *this = std::move (move);
}


Badger& Badger::operator= (Badger&& move)
{
    if (this != &move)
    {
        // IActor.
        m_node = std::move (move.m_node);
        
        // Badger.
        m_handleBar = std::move (move.m_handleBar);
        m_luggageRack = std::move (move.m_luggageRack);
        m_wheels = std::move (move.m_wheels);

        m_wheelBase = std::move (move.m_wheelBase);

        m_acceleration = std::move (move.m_acceleration);
        m_brakePower = std::move (move.m_brakePower);
        
        m_currentSpeed = std::move (move.m_currentSpeed);
        m_maxSpeed = std::move (move.m_maxSpeed);
        m_targetSpeedRate = std::move (move.m_targetSpeedRate);
    }

    return *this;
}


Badger::~Badger()
{
}

#pragma endregion


#pragma region Getters and setters

float Badger::getTurnLimitRadians() const
{
    return m_handleBar->getTurnLimitRadians();
}


void Badger::setSpeedRate (const float speed)
{
    // Clamp the given speed rate and then scale it to between 0.f and 1.f.
    const float min { -1.f }, max { 1.f };

    m_targetSpeedRate = util::clamp (speed, min, max);
}


void Badger::setTurnRate (const float turn)
{
    // Clamp the given turn rate.
    const float min         { -1.f }, max { 1.f },
                clampedTurn { util::clamp (turn, min, max) };
    
    // Ensure the handle bar follows the wheels.
    m_handleBar->setTargetTurn (clampedTurn);
    
    // Only update the first two wheels.
    if (m_wheels.size() >= 2)
    {
        m_wheels[0]->setTargetTurn (clampedTurn);
        m_wheels[1]->setTargetTurn (clampedTurn);
    }
}


void Badger::reset()
{
    // Reset the badger itself.
    m_node->setPosition ({ 0.f, 4.f, 0.f });
    m_node->setOrientation ({ });
    m_node->setScale ({ 200.f, 200.f, 200.f });

    // Reset the handle bar.
    m_handleBar->reset();

    // Reset the wheels.
    for (auto& wheel : m_wheels)
    {
        wheel->reset();
    }

    setupWheels();

    // Reset our speed.
    m_currentSpeed = 0.f;
    m_targetSpeedRate = 0.f;
}

#pragma endregion


#pragma region Actor functionality

bool Badger::initialise (OgreApplication* const ogre, Ogre::SceneNode* const root, const Ogre::String& name)
{
    try
    {
        // Pre-condition: We have a valid OgreApplication and SceneNode pointer.
        if (!ogre || !root)
        {
            throw std::invalid_argument ("Badger::initialise(), required parameter 'ogre' or 'root' is a nullptr.");
        }    
        
        // Ensure we allocate all the required memory to speed up intialisation.
	    m_wheels.resize (4);
        
        // Load the chasis.
        const auto entity = constructEntity (ogre, "chassis.mesh");
        m_node.reset (constructNode (root, name, entity));

        // Load the child nodes.
        createChildren (ogre, name);

        // Prepare each element.
        reset();

        // Finally we're done!
        return true;
    }

    catch (const std::exception& e)
    {
        std::cerr << "An exception was caught in Badger::initialise(): " << e.what() << std::endl;
    }

    catch (...)
    {
        std::cerr << "An unknown error occurred in Badger::initialise()." << std::endl;
    }

    return false;
}


void Badger::updateSimulation (const float deltaTime)
{
    // Ensure we have correct speed values.
    updateSpeed (deltaTime);

    // Calculate the distance to travel. We don't take the badgers scale into account, instead we assume the model and speed values have been scaled correctly.
    const float distance    { m_currentSpeed * deltaTime };

    // Simulate the badgers movement and rotation.
    moveForward (distance);
    rotate (distance);

    // Update each component. We can ignore the luggage rack.
    m_handleBar->updateSimulation (deltaTime);
    
    for (auto& wheel : m_wheels)
    {
        wheel->updateSimulation (deltaTime);
    }
}

#pragma endregion


#pragma region External simulation

void Badger::revolveWheels (const float distance)
{
    // Inform each wheel that it needs to revolve.
    for (auto& wheel : m_wheels)
    {
        wheel->revolve (distance);
    }
}

#pragma endregion


#pragma region Simulation

void Badger::updateSpeed (const float deltaTime)
{
    // Calculate whether we've reached our target speed.
    const float currentSpeedRate    { m_currentSpeed / m_maxSpeed };

    if (!util::roughlyEquals (currentSpeedRate, m_targetSpeedRate, 0.001f))
    {
        // The modifier decides if we use negative or positive speed increases.
        const float modifier            { m_targetSpeedRate > currentSpeedRate ? 1.f : -1.f };

        // Create the a variable ready for calculating the increase in speed.
        float increase = 0.f;

        // Detect if we're supposed to be braking. Only brake if we want to swap between forward and reverse movement and vice versa.
        if ((currentSpeedRate > 0.f && m_targetSpeedRate < 0.f) ||
            (currentSpeedRate < 0.f && m_targetSpeedRate > 0.f))
        {
            // Instead of duplicating code we'll just use absolute values and take advantage of the modifier.
            const float absSpeed        { std::abs (m_currentSpeed) };

            // Calculate whether we've braked too far and therefore should use a proportion of acceleration for accurate speed.
            const float deltaBrake      { m_brakePower * deltaTime };
            const float brakeDifference { absSpeed - deltaBrake };

            if (brakeDifference < 0.f)
            {
                // Calculate the proportion of acceleration speed to use.
                const float brakeProportion { std::abs (brakeDifference / m_brakePower) };
                const float acceleration    { m_acceleration * deltaTime * (1.f - brakeProportion) };

                // The increase should be the brake speed + acceleration at the calculated proportion.
                increase = (deltaBrake * brakeProportion + acceleration) * modifier;
            }

            else
            {
                // Just reduce the speed by the brake power.
                increase = deltaBrake * modifier;
            }
        }

        // Just increase speed to the target rate.
        else
        {
            increase = m_acceleration * modifier * deltaTime;
        }

        // Finally update our speed.
        setCurrentSpeed (m_currentSpeed + increase);
    }
}


void Badger::moveForward (const float distance)
{
    // Don't waste extra computation time.
    if (!util::roughlyEquals (distance, 0.f, 0.001f))
    {
        // Calculate the forward direction.
        const auto& orientation = m_node->getOrientation();
        const auto  forward     = orientation * Ogre::Vector3::UNIT_Z;

        // Move the badger.
        m_node->setPosition (m_node->getPosition() + forward * distance);

        // Revolve the wheels accordingly.
        revolveWheels (distance);
    }
}


void Badger::rotate (const float distance)
{
    // Calculate the angle to rotate the badger by.
    const float angle   { distance * std::sin (m_handleBar->currentYaw()) / m_wheelBase };

    // Don't waste extra computation time.
    if (!util::roughlyEquals (angle, 0.f, 0.001f))
    {
        // Create the additional rotation.
        const auto rotation = Ogre::Quaternion (Ogre::Radian (angle), Ogre::Vector3::UNIT_Y);

        // Rotate the badger.
        m_node->rotate (rotation, Ogre::Node::TS_LOCAL);
    }
}

#pragma endregion


#pragma region Helper functions

void Badger::createChildren (OgreApplication* const ogre, const Ogre::String& name)
{
    // Create the actual data.
    m_handleBar = std::make_unique<HandleBar>();
    m_luggageRack = std::make_unique<LuggageRack>();

    for (unsigned int i = 0; i < m_wheels.size(); ++i)
    {
        m_wheels[i] = std::make_unique<Wheel>();
    }

    
    // Initialise each child object.
    if (!m_handleBar->initialise (ogre, m_node.get(), name + "-HandleBar"))
    {
        throw std::runtime_error ("Badger::createChildren(), the handle bars couldn't be initialised.");
    }

    if (!m_luggageRack->initialise (ogre, m_node.get(), name + "-LuggageRack"))
    {
        throw std::runtime_error ("Badger::createChildren(), the luggage rack couldn't be initialised.");
    }

    for (unsigned int i = 0; i < m_wheels.size(); ++i)
    {
        if (!m_wheels[i]->initialise (ogre, m_node.get(), name + "-Wheel-" + std::to_string (i)))
        {
            throw std::runtime_error ("Badger::createChildren(), a wheel couldn't be initialised.");
        }
    }
}


void Badger::setupWheels()
{
    // Set the correct wheel positions.
    m_wheels[0]->setPosition ({ 0.014f, -0.0025f, 0.0254f });       // Front left.
    m_wheels[1]->setPosition ({ -0.014f, -0.0025f, 0.0254f });      // Front right.
    m_wheels[2]->setPosition ({ 0.014f, -0.0038f, -0.0254f });      // Back left.
    m_wheels[3]->setPosition ({ -0.014f, -0.0038f, -0.0254f });     // Back right.
    
    // Calculate the orientation quaternions.
    const auto  leftOrientation     = Ogre::Quaternion (Ogre::Degree (-90.f), Ogre::Vector3::UNIT_Z),
                rightOrientation    = Ogre::Quaternion (Ogre::Degree (90.f), Ogre::Vector3::UNIT_Z);

    for (unsigned int i = 0; i < m_wheels.size(); ++i)
    {
        // Even indices are wheels situated on the left.
        if (i % 2 == 0)
        {
            m_wheels[i]->setRevolveModifier (1.f);
            m_wheels[i]->setOrientation (leftOrientation);
        }

        // Right wheels needs a reverse modifier.
        else
        {
            m_wheels[i]->setRevolveModifier (-1.f);
            m_wheels[i]->setOrientation (rightOrientation);
        }
    }
}


void Badger::setCurrentSpeed (const float speed)
{
    m_currentSpeed = util::clamp (speed, -m_maxSpeed, m_maxSpeed);
}

#pragma endregion