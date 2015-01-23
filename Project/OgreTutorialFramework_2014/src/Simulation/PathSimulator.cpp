#include "PathSimulator.h"



// STL headers.
#include <exception>
#include <iostream>



// Engine headers.
#include <Badger/Badger.h>
#include <Framework/OgreApplication.h>
#include <Path/Segment.h>
#include <Utility/Ogre.h>



#pragma region Constructors and destructor

PathSimulator::PathSimulator()
{
}


PathSimulator::PathSimulator (PathSimulator&& move)
{
    *this = std::move (move);
}


PathSimulator& PathSimulator::operator= (PathSimulator&& move)
{
    if (this != &move)
    {
        // PathSimulator.
        m_badger = std::move (move.m_badger);
        m_path = std::move (move.m_path);

        m_segment = std::move (move.m_segment);

        m_previousTangent = std::move (move.m_previousTangent);

        m_segmentIndex = std::move (move.m_segmentIndex);
        m_time = std::move (move.m_time);
        m_timeForSegment = std::move (move.m_timeForSegment);
    }

    return *this;
}


PathSimulator::~PathSimulator()
{
}

#pragma endregion


#pragma region ISimulator functionality

bool PathSimulator::initialise (OgreApplication* const ogre)
{
    try
    {
        // Pre-condition: A valid ogre pointer exists.
        if (!ogre)
        {
            throw std::invalid_argument ("PathSimulator::initialise(), required parameter is a nullptr.");
        }

        // Create necessary materials.
        util::createMaterial ("blue", "blue.png");
        util::createMaterial ("green", "green.png");
        util::createMaterial ("red", "red.png");

        // Obtain the root SceneNode.
        const auto root = ogre->GetSceneManager()->getRootSceneNode();

        // Initialise each required object.
        loadBadger (ogre, root);
        loadPath (ogre, root);

        std::cout << "Path length: " << std::to_string (m_path->getLength()) << std::endl;

        // Reset ourself.
        reset();

        // We're finished and can begin the simulation.
        return true;
    }

    catch (const std::exception& error)
    {
        std::cerr << "Exception caught in PathSimulator::initialise(): " << error.what() << std::endl;
    }

    catch (...)
    {
        std::cerr << "An unknown error was caught in PathSimulator::initialise()." << std::endl;
    }

    // Indicate failure.
    return false;
}


void PathSimulator::reset()
{
    // Start the path again.
    obtainSegment (0);
    m_segmentIndex = 0;
    m_time = 0.f;
    m_timeForSegment = 0.f;

    // Move the badger to the start point.
    m_badger->reset();
    m_badger->setPosition (m_segment->getPoint (0));
    m_badger->setMaxSpeed (100.f);
}


void PathSimulator::update (const float deltaTime)
{
    m_timeForSegment += deltaTime;
    
    // Obtain the tangent vector.
    const auto tangent      = m_segment->curvePoint (m_time, Derivative::First);

    const float velocity    = tangent.length();

    const float inverse     = 1 / velocity;

    const float modifier    = m_path->getLength() / 20.f / 60.f;

    m_time                  += inverse * modifier;

    const auto curve        = m_segment->curvePoint (m_time);

    m_badger->setPosition (curve);

    const auto correctTanget = m_segment->curvePoint (m_time, Derivative::First);

    m_badger->getNode()->setDirection (correctTanget.normalisedCopy(), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);

    if (m_time >= 1.f)
    {
        // The first segment will when the application starts will be longer than normal because loading times effect the calculations.
        std::cout << "Segment length: " << std::to_string (m_segment->getLength()) << " completed in " << std::to_string (m_timeForSegment) << " seconds."<< std::endl;

        // Reset the time counter and obtain the next segment.
        obtainSegment (++m_segmentIndex);
    }
}

#pragma endregion


#pragma region Initialisation

void PathSimulator::loadBadger (OgreApplication* const ogre, Ogre::SceneNode* const root)
{
    // Loading the badger is simple.
    m_badger = std::make_unique<Badger>();

    if (!m_badger->initialise (ogre, root, "PathSimulator"))
    {
        throw std::runtime_error ("PathSimulator::loadBadger(), unable to initialise the badger.");
    }
}


void PathSimulator::loadPath (OgreApplication* const ogre, Ogre::SceneNode* const root)
{
    // Obtain a working xml file location.
    const auto location = "../../path.xml";//obtainFileLocation();

    // Attempt to initialise the path.
    m_path = std::make_unique<Path>();
    m_path->setWaypointScale ({ 200.f, 200.f, 200.f });
    
    if (!m_path->loadFromXML (location, ogre, root))
    {
        throw std::runtime_error ("PathSimulator::loadPath(), unable to load the path from the given file location.");
    }
}


std::string PathSimulator::obtainFileLocation() const
{
    // Loop forever until we have a working file location.
    while (true)
    {
		std::cout << std::endl << "Please enter the location of an xml file containing path data (e.g path.xml):" << std::endl;

		// Use std::getline() to allow for spaces in the file location and to allow the use of the string class.
		std::string location;
		std::getline (std::cin, location);

		// Test if the location ends in .xml
		const size_t ext = location.find_last_of (".");

		if (ext != std::string::npos && location.substr (ext) == ".xml")
		{
			return location;
		}

        // Report the error and start again.
        std::cerr << "Invalid file location given. Ensure it ends in .xml" << std::endl;
	}
}

#pragma endregion


#pragma region Simulation management

void PathSimulator::obtainSegment (const size_t segment)
{
    // Clamp the segment value between the path count.
    const size_t workingSegment { segment % m_path->getSegmentCount() };

    // Obtain the desired segment.
    m_segment = m_path->getSegment (workingSegment);

    // Ensure it's valid.
    if (m_segment)
    {
        // Adjust the time value back a step.
        m_time -= 1.f;
        m_timeForSegment = 0.f;
    }
}

#pragma endregion