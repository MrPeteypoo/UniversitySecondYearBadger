#include "BadgerSimulator.h"



// STL headers.
#include <exception>



// Engine headers.
#include <Badger/Badger.h>
#include <Framework/OgreApplication.h>
#include <Utility/Maths.h>
#include <Utility/Ogre.h>



#pragma region Constructors and destructor

BadgerSimulator::BadgerSimulator() 
{
}


BadgerSimulator::BadgerSimulator (BadgerSimulator&& move)
{
    *this = std::move (move);
}


BadgerSimulator& BadgerSimulator::operator= (BadgerSimulator&& move)
{
    if (this != &move)
    {
        m_keyboard = std::move (move.m_keyboard);
        m_badger = std::move (move.m_badger);

        m_speedRate = std::move (move.m_speedRate);
        m_turnRate = std::move (move.m_turnRate);
    }

    return *this;
}


BadgerSimulator::~BadgerSimulator() 
{
}

#pragma endregion


#pragma region ISimulator functionality

bool BadgerSimulator::initialise (OgreApplication* const ogre)
{
    // Pre-condition: We have a valid OgreApplication
    if (!ogre)
    {
        throw std::invalid_argument ("BadgerSimulator::initialiseScene(), required parameter 'ogre' is a nullptr.");
    }

    // Ensure the scene is created.
    ogre->CreateScene();

    // Create some basic materials.
    util::createMaterial ("blue", "blue.png");
    util::createMaterial ("green", "green.png");

    // Initialise the input.
    m_keyboard = ogre->GetKeyboard();

    // Set up the badgers.
    m_badger = std::make_unique<Badger>();
    m_badger->initialise (ogre, ogre->GetSceneManager()->getRootSceneNode(), "BadgerSimulator");

    return true;
}


void BadgerSimulator::reset()
{
    if (m_badger)
    {
        m_badger->reset();
        setSpeedRate (0.f);
        setTurnRate (0.f);
    }
}


void BadgerSimulator::update (const float deltaTime)
{
    // We need to update the input and have the badger reflect the change of input.
    updateInput();
    m_badger->updateSimulation (deltaTime);
}

#pragma endregion


#pragma region Simulation management

void BadgerSimulator::updateInput()
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


void BadgerSimulator::setSpeedRate (const float value)
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


void BadgerSimulator::setTurnRate (const float value)
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