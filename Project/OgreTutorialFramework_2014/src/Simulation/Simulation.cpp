#include "Simulation.h"



// STL headers.
#include <iostream>



// Engine headers.
#include <Simulation/BadgerSimulator.h>
#include <Simulation/PathSimulator.h>



#pragma region Constructors

Simulation::Simulation (Simulation&& move)
{
    *this = std::move (move);
}


Simulation& Simulation::operator= (Simulation&& move)
{
    if (this != &move)
    {
        // Simulation.
        m_simulators = std::move (m_simulators);
    }

    return *this;
}


Simulation::~Simulation()
{
    // Clean up after ourselves.
    for (auto& simulator : m_simulators)
    {
        if (simulator)
        {
            delete simulator;
            simulator = nullptr;
        }
    }
}

#pragma endregion


#pragma region Public interface

void Simulation::initialise (OgreApplication* const ogre)
{
    // Add the simulators.
    m_simulators.push_back (new BadgerSimulator());
    m_simulators.push_back (new PathSimulator());

    // Initialise each simulator.
    for (unsigned int i = 0; i < m_simulators.size(); ++i)
    {
        // Cache the simulator.
        auto& simulator = m_simulators[i];

        // Ensure it's valid.
        if (simulator)
        {
            // Report an error and remove the ISimulator if it can't be initialised.
            if (!simulator->initialise (ogre))
            {
                delete simulator;
                simulator = nullptr;

                std::cout << "Simulation::initialise(): unable to initialise an ISimulator." << std::endl;
            }
        }
    }
}


void Simulation::reset()
{
    // Inform each simulator to reset itself.
    for (auto simulator : m_simulators)
    {
        if (simulator)
        {
            simulator->reset();
        }
    }
}


void Simulation::update (const float deltaTime)
{
    // Inform each simulator to update itself.
    for (auto simulator : m_simulators)
    {
        if (simulator)
        {
            simulator->update (deltaTime);
        }
    }
}

#pragma endregion