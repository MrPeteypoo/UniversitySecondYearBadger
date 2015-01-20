#include "Simulation.h"



// STL headers.
#include <exception>



// Engine headers.
#include <Badger/Badger.h>
#include <Framework/OgreApplication.h>
#include <Utility/Maths.h>
#include <Utility/Ogre.h>



#pragma region Constructors and destructor

Simulation::Simulation() 
{
}


Simulation::Simulation (Simulation&& move)
{
    *this = std::move (move);
}


Simulation& Simulation::operator= (Simulation&& move)
{
    if (this != &move)
    {
        m_badger = std::move (move.m_badger);
        m_keyboard = std::move (move.m_keyboard);
    }

    return *this;
}


Simulation::~Simulation() 
{
}

#pragma endregion


#pragma region Getters and setters

void Simulation::setKeyboard (const std::shared_ptr<OIS::Keyboard>& keyboard)
{
    m_keyboard = keyboard;
}

#pragma endregion


#pragma region Public interface

void Simulation::initialiseScene (OgreApplication* const ogre)
{
    // Pre-condition: We have a valid OgreApplication
    if (!ogre)
    {
        throw std::invalid_argument ("Simulation::initialiseScene(), required parameter 'ogre' is a nullptr.");
    }

    // Ensure the scene is created.
    ogre->CreateScene();

    // Create some basic materials.
    util::createMaterial ("blue", "blue.png");
    util::createMaterial ("green", "green.png");

    // Set up the badger.
    m_badger = std::make_unique<Badger>();
    m_badger2 = std::make_unique<Badger>();
    m_badger->initialise (ogre, ogre->GetSceneManager()->getRootSceneNode(), "BadgerControlled");
    m_badger2->initialise (ogre, ogre->GetSceneManager()->getRootSceneNode(), "BadgerPath");
}


void Simulation::reset()
{
    if (m_badger)
    {
        m_badger->reset();
        setSpeedRate (0.f);
        setTurnRate (0.f);
    }
}


void Simulation::update (const float deltaTime)
{
    updateInput();
    updateSimulation (deltaTime);
}

#pragma endregion


#pragma region Simulation management

void Simulation::updateInput()
{
    // Initialise default speed and turn rate values.
    float   speedRate   { 0.f },
            turnRate    { 0.f };
    
    // Lock the weak pointer.
    auto keyboard = m_keyboard.lock();
    
    if (keyboard)
    {
        // Add to the speed rate if up is pressed.
        if (keyboard->isKeyDown (OIS::KC_UP))
        {
            speedRate += 1.f;
        }

        // Subtract from the speed rate if down is pressed.
        if (keyboard->isKeyDown (OIS::KC_DOWN))
        {
            speedRate -= 1.f;
        }

        // Subtract from the turn rate if left is pressed.
        if (keyboard->isKeyDown (OIS::KC_LEFT))
        {
            turnRate -= 1.f;
        }

        // Add to the turn rate if right is pressed.
        if (keyboard->isKeyDown (OIS::KC_RIGHT))
        {
            turnRate += 1.f;
        }
    }

    // Update the badgers values.
    setSpeedRate (speedRate);
    setTurnRate (turnRate);
}


void Simulation::updateSimulation (const float deltaTime)
{
    // Update the simulation.
    m_badger->updateSimulation (deltaTime);
}


void Simulation::setSpeedRate (const float value)
{
    // Compare the current value to the previous to ensure there's no unnecessary updating of the Badger.
    const float previous = m_speedRate;
    m_speedRate = value;

    // Don't unnecessarily update the speed.
    if (m_speedRate != previous)
    {
        // Update the wheel and handle bar rotation.
        m_badger->setSpeedRate (m_speedRate);
    }
}


void Simulation::setTurnRate (const float value)
{
    // Compare the current value to the previous to ensure there's no unnecessary updating of the Badger.
    const float previous = m_turnRate;
    m_turnRate = value;
    
    // Don't unnecessarily update the speed.
    if (m_turnRate != previous)
    {
        // Update the wheel and handle bar rotation.
        m_badger->setTurnRate (m_turnRate);
    }
}

#pragma endregion