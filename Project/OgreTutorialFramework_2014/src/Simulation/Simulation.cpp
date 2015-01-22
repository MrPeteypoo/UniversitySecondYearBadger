#include "Simulation.h"



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
    for (auto simulator : m_simulators)
    {
        if (simulator)
        {
            if (!simulator->initialise (ogre))
            {
                throw std::runtime_error ("Simulation::initialise(), unable to initialise an ISimulator.");
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