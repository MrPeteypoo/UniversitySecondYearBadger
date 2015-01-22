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

        m_distanceTravelled = std::move (move.m_distanceTravelled);
        m_segmentStart = std::move (move.m_segmentStart);
        m_segmentEnd = std::move (move.m_segmentEnd);
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

        // Obtain our first segment.
        obtainSegment (0);

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
    // Reset the badger.
    m_badger->reset();

    // Start the path again.
    m_distanceTravelled = 0.f;
    m_segmentStart = 0.f;
    m_segmentEnd = 0.f;
    obtainSegment (0);
}


void PathSimulator::update (const float deltaTime)
{
    // Correct our distance value.
    updateDistance (deltaTime);

    // Use the correct segment.
    updateSegment();

    // Move the badger according to the curve.
    updateBadger();
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
    const auto location = obtainFileLocation();

    // Attempt to initialise the path.
    m_path = std::make_unique<Path>();
    
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

void PathSimulator::obtainSegmentByDistance()
{
    // Obtain the desired segment.
    m_segment = m_path->segmentByDistance (m_distanceTravelled);

    // Ensure it's valid.
    if (m_segment)
    {
        // Adjust our start and end values.
        m_segmentStart = m_segmentEnd;
        m_segmentEnd = m_segmentStart + m_segment->getLength();
    }
}


void PathSimulator::obtainSegment (const size_t segment)
{
    // Clamp the segment value between the path count.
    const size_t workingSegment { segment % m_path->getSegmentCount() };

    // Obtain the desired segment.
    m_segment = m_path->getSegment (workingSegment);

    // Ensure it's valid.
    if (m_segment)
    {
        // Adjust our start and end values.
        m_segmentStart = m_segmentEnd;
        m_segmentEnd = m_segmentStart + m_segment->getLength();
    }
}


void PathSimulator::updateDistance (const float deltaTime)
{
    
}


void PathSimulator::updateSegment()
{
    
}


void PathSimulator::updateBadger()
{
    
}

#pragma endregion