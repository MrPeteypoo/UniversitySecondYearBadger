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
        m_node = std::move (move.m_node);
        
        m_handleBar = std::move (move.m_handleBar);
        m_luggageRack = std::move (move.m_luggageRack);
        m_wheels = std::move (move.m_wheels);
    }

    return *this;
}


Badger::~Badger()
{
}

#pragma endregion


#pragma region Getters and setters

void Badger::setSpeedRate (const float speed)
{
    // Clamp the given speed rate and then scale it to between 0.f and 1.f.
    const float min             { -1.f }, 
                max             { 1.f }, 
                clampedSpeed    { util::clamp (speed, min, max) },
                scaledSpeed     { clampedSpeed * 0.5f + 0.5f };
    
    m_tracker.setNormalisedTarget (scaledSpeed);
}


void Badger::setTurnRate (const float turn)
{
    // Clamp the given turn rate.
    const float min         { -1.f }, 
                max         { 1.f }, 
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

    m_tracker.setValues (0.f, 5.f, 2.5, 2.49999f);

    // Reset the handle bar.
    m_handleBar->reset();

    // Reset the wheels.
    for (auto& wheel : m_wheels)
    {
        wheel->reset();
    }

    setupWheels();
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
        
        // Ensure we allocate all the required memory.
	    m_wheels.resize (4);
        
        // Create parameters.        
        const Ogre::Vector3     position    { 0.f, 4.f, 0.f };
        const Ogre::Quaternion  orientation {  };
        const Ogre::Vector3     scale       { 200.f, 200.f, 200.f };
        
        // Load the chasis.
        const auto entity = constructEntity (ogre, "chassis.mesh");
        m_node.reset (constructNode (root, name, entity, position, orientation, scale));

        // Load the child nodes.
        createChildren (ogre, name);
        setupWheels();

        // Finally we're finished!
        m_tracker.setValues (0.f, 5.f, 2.5, 2.49999f);

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
    // Scale the normalised target to -1.f/1.f. We'll use this to move and rotate in the correct direction.
    const float modifier    { m_tracker.normalisedCurrent() * 2.f - 1.f };

    // Simulate the badgers movement and rotation. Use a fudge-factor of 4 to make the rotation feel a little more natural.
    moveForward (modifier, deltaTime);
    rotate (modifier * 4, deltaTime);

    // Update each component. We can ignore the luggage rack.
    m_tracker.updateTime (deltaTime);
    m_handleBar->updateSimulation (deltaTime);
    
    for (auto& wheel : m_wheels)
    {
        wheel->updateSimulation (deltaTime);
    }
}

#pragma endregion


#pragma region Simulation

void Badger::moveForward (const float modifier, const float deltaTime)
{
    // Calculate the distance to travel. Take the scale of the badger into account.
    const float distance    { m_maxSpeed * modifier * deltaTime };

    // Don't waste extra computation time.
    if (!util::roughlyEquals (distance, 0.f, 0.001f))
    {
        // Calculate the forward direction.
        const auto& orientation = m_node->getOrientation();
        const auto  forward     = orientation * Ogre::Vector3::UNIT_Z;

        // Move the badger.
        m_node->setPosition (m_node->getPosition() + forward * distance);

        // Revolve the wheels accordingly.
        for (auto& wheel : m_wheels)
        {
            wheel->revolve (distance);
        }
    }
}


void Badger::rotate (const float modifier, const float deltaTime)
{
    // Calculate the angle to rotate the badger by.
    const float angle   { m_handleBar->currentYaw() * modifier * deltaTime };

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

    
    // Initialise each object.
    if (!m_handleBar->initialise (ogre, m_node.get(), name + "-HandleBar"))
    {
        throw std::runtime_error ("Badger::initialise(), the handle bars couldn't be initialised.");
    }

    if (!m_luggageRack->initialise (ogre, m_node.get(), name + "-LuggageRack"))
    {
        throw std::runtime_error ("Badger::initialise(), the luggage rack couldn't be initialised.");
    }

    for (unsigned int i = 0; i < m_wheels.size(); ++i)
    {
        if (!m_wheels[i]->initialise (ogre, m_node.get(), name + "-Wheel-" + std::to_string (i)))
        {
            throw std::runtime_error ("Badger::initialise(), a wheel couldn't be initialised.");
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
    const auto  leftOrientation = Ogre::Quaternion (Ogre::Degree (-90.f), Ogre::Vector3::UNIT_Z),
                rightOrientation = Ogre::Quaternion (Ogre::Degree (90.f), Ogre::Vector3::UNIT_Z);

    for (unsigned int i = 0; i < m_wheels.size(); ++i)
    {
        // Even indexes are wheels that are situated on the left.
        if (i % 2 == 0)
        {
            m_wheels[i]->setRevolveModifier (1.f);
            m_wheels[i]->setOrientation (leftOrientation);
        }

        // Right wheels move in reverse.
        else
        {
            m_wheels[i]->setRevolveModifier (-1.f);
            m_wheels[i]->setOrientation (rightOrientation);
        }
    }
}

#pragma endregion