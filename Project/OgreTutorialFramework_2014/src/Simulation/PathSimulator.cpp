#include "PathSimulator.h"



// STL headers.
#include <exception>
#include <iostream>



// Engine headers.
#include <Badger/Badger.h>
#include <Framework/OgreApplication.h>
#include <Path/Segment.h>
#include <Utility/Maths.h>
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

        m_timeToComplete = std::move (move.m_timeToComplete);

        m_time = std::move (move.m_time);
        m_timeForSegment = std::move (move.m_timeForSegment);
    }

    return *this;
}


PathSimulator::~PathSimulator()
{
}

#pragma endregion


#pragma region Getters and setters

void PathSimulator::setTimeToComplete (const float time)
{
    // Silently ignore 0.f.
    if (time != 0.f)
    {
        m_timeToComplete = std::abs (time);
    }
}


void PathSimulator::movementSpeed (const float distancePerSecond)
{
    if (distancePerSecond != 0.f)
    {
        m_timeToComplete = m_path->getLength() / std::abs (distancePerSecond);
    }
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
        loadPath (ogre, root);
        loadBadger (ogre, root);

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
    /// We can use the length of the tangent vector to normalise the time it takes to move across an arc length. This works by 
    /// calculating the first derivative of a point on the bezier curve (tangent vector), the magnitude represents the speed of
    /// the curve at that point. If we then inverse the magnitude we can increment time by a correct value to maintain a smooth 
    /// curve.
    ///
    /// By default this method at 60 units per second on any given curve which we can take advantage of to ensure the path is
    /// at the pace we desire.
    ///
    /// I chose to use this method of interpolating through the arc length of the complete path due to the ease at which it can
    /// be done, and also because of the memory savings since we don't have to store arc length/paramatric entry pairs. The time
    /// it takes to calculate at runtime is also likely to be less since we can cache the tangent vector of the current point and
    /// we don't need to lookup the current distance we're at. Load time is also comparable, we need to segment the curve to
    /// determine the total path/curve length but we save on time by not storing each segments value.
    
    // We need to calculate the distance we should move each frame based on the desired time to completion.
    const float arcDistancePerFrame { m_path->getLength() / m_timeToComplete * deltaTime };

    // We need to update the time it's taken to move across the current segment. This is useful for showing consistency.
    m_timeForSegment += deltaTime;

    // Increase our time value by a calculated increment.
    m_time += timeIncrement() * arcDistancePerFrame;

    // Obtain the position and tangent of the desired point of the bezier curve.
    const auto position     = m_segment->curvePoint (m_time);
    const auto tangent      = m_segment->curvePoint (m_time, Derivative::First);
    //const auto curvature    = m_segment->curvePoint (m_time, Derivative::Second);

    //const auto u            = tangent.crossProduct (curvature);
    //const auto v            = tangent.crossProduct (u);

    // Update the badgers position and orientation.
    m_badger->setPosition (position);    
    m_badger->getNode()->setDirection (tangent, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);

    // Move the badgers wheels. Unfortunately I haven't had time to try and rotate the wheels properly.
    m_badger->revolveWheels (arcDistancePerFrame);

    // Cache the tangent vector of the desired point.
    m_previousTangent = tangent;

    // Check if we need to move on to the next segment.
    if (m_time >= 1.f)
    {
        // The first segment will when the application starts will be longer than normal because loading times effect deltaTime.
        std::cout << "Path: " << std::to_string (m_path->getLength())
                  << ", segment: " << std::to_string (m_segment->getLength()) 
                  << ", completed: " << std::to_string (m_timeForSegment) << " seconds."<< std::endl;

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
    const auto location = obtainFileLocation();

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

        // Cache the tangent of this point.
        m_previousTangent = m_segment->curvePoint (m_time, Derivative::First);
    }
}


float PathSimulator::timeIncrement() const
{
    // We need the magnitude of the tangent to determine how fast the curve is moving.
    const float tangentSpeed    { m_previousTangent.length() };

    // We should then inverse it to get an incrementable value. This should represent steps per second.
    const float inverse         { 1.f / tangentSpeed };

    // Return the increment.
    return inverse;
}

#pragma endregion