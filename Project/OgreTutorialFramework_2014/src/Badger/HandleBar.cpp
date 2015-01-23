#include "HandleBar.h"



// STL headers.
#include <exception>
#include <iostream>



#pragma region Constructors and destructor


Badger::HandleBar::HandleBar (HandleBar&& move)
{
    *this = std::move (move);
}


Badger::HandleBar& Badger::HandleBar::operator= (HandleBar&& move)
{
    if (this != &move)
    {
        // IActor.
        m_node = std::move (move.m_node);

        // Badger::IRotatableComponent.
        m_turnSpeed = std::move (move.m_turnSpeed);
        m_targetTurn = std::move (move.m_targetTurn);
    }

    return *this;
}


Badger::HandleBar::~HandleBar()
{
}

#pragma endregion


#pragma region Getters and setters

void Badger::HandleBar::reset()
{
    // Reset the node itself.
    m_node->setPosition ({ 0.f, 0.0182f, 0.01f });
    m_node->setOrientation ({ Ogre::Degree (-37.784f), Ogre::Vector3::UNIT_X });
    m_node->setScale ({ 1.f, 1.f, 1.f });

    // Ensure the handle bars won't turn without more input.
    m_targetTurn = 0.f;
}

#pragma endregion


#pragma region Actor functionality

bool Badger::HandleBar::initialise (OgreApplication* const ogre, Ogre::SceneNode* root, const Ogre::String& name)
{
    try
    {
        // Pre-condition: We have a valid OgreApplication and SceneNode pointer.
        if (!ogre || !root)
        {
            throw std::invalid_argument ("Badger::HandleBar::initialise(), required parameter 'ogre' or 'root' is a nullptr.");
        }

        // Initialise the entity.
        const auto entity = constructEntity (ogre, "handlebar.mesh", "blue");

        // Construct the node.
        m_node.reset (constructNode (root, name, entity));

        // Reset each attribute.
        reset();
        
        // We're done, yay!
        return true;
    }

    catch (const std::exception& error)
    {
        std::cerr << "An exception was caught in Badger::HandleBar::initialise(): " << error.what() << std::endl;
    }

    catch (...)
    {
        std::cerr << "An unknown error occurred in Badger::HandleBar::initialise()." << std::endl;
    }
    
    return false;
}


void Badger::HandleBar::updateSimulation (const float deltaTime)
{
    // We simply need to rotate the handle bars according to their target turn rate.
    rotateComponent (Ogre::Vector3::UNIT_Y, Ogre::Node::TS_LOCAL, deltaTime);
}

#pragma endregion 