#include "Path.h"


// STL headers.
#include <cmath>
#include <exception>
#include <iostream>



// Third party headers.
#include <ThirdParty/pugixml.hpp>



// Engine headers.
#include <Path/Segment.h>
#include <Path/Waypoint.h>
#include <Utility/Maths.h>



#pragma region Constructors and destructor

Path::Path()
{
}


Path::Path (Path&& move)
{
    *this = std::move (move);
}


Path& Path::operator= (Path&& move)
{
    if (this != &move)
    {
        // Path.
        m_segments = std::move (move.m_segments);
        m_waypoints = std::move (move.m_waypoints);

        m_length = std::move (move.m_length);
    }

    return *this;
}


Path::~Path()
{
}

#pragma endregion


#pragma region Getters and setters

const std::shared_ptr<const Path::Segment> Path::segmentByDistance (const float distance) const
{
    // Pre-condition: We have valid length values.
    if (m_length <= 0.f)
    {
        return nullptr;
    }

    // Calculate whether we need to scale the distance back or not.
    const float workingDistance { distance > m_length ? std::fmod (distance, m_length) : distance };

    // Add up the distances until we reach a valid value.
    float previous  { 0.f };

    for (unsigned int i = 0; i < m_segments.size(); ++i)
    {
        const float current { previous + m_segments[i]->getLength() };

        // Return the previous segment if we're in the correct segment.
        if (previous >= distance && current < distance)
        {
            return m_segments[i - 1];
        }

        previous = current;
    }

    return m_segments.front();
}

const std::shared_ptr<const Path::Segment> Path::getSegment (const unsigned int index) const
{
    // It's quicker try, catch and succeed than it is to test each index.
    try
    {
        return m_segments.at (index);
    }

    // We don't need to care about exception thrown.
    catch (...)
    {
        return nullptr;
    }
}


void Path::setWaypointScale (const Ogre::Vector3& scale, const bool updateCurrent)
{
    // Change the scale.
    m_waypointScale = scale;

    // Update the stored waypoints if desired.
    if (updateCurrent)
    {
        for (const auto& waypoint : m_waypoints)
        {
            if (waypoint)
            {
                waypoint->setScale (m_waypointScale);
            }
        }
    }
}

#pragma endregion


#pragma region Initialisation

bool Path::loadFromXML (const std::string& fileLocation, OgreApplication* const ogre, Ogre::SceneNode* const root)
{
    // We'll use pugixml for loading via xml, the structure should be Path (Name, SamplersPerSegment) -> Segment -> Point (X, Y, Z). 
    // Segments are not limited but each segment must have four points.
    try
    {
        // Pre-condition: We have valid pointers.
        if (!ogre || !root)
        {
            throw std::invalid_argument ("Path::loadFromXML(), required parameter is a nullptr.");
        }

        // Clear our current data.
        m_segments.clear();
        m_waypoints.clear();

        // Create and load the xml document.
        pugi::xml_document xml  {  };
        if (!xml.load_file (fileLocation.c_str()))
        {
            throw std::invalid_argument ("Path::loadFromXML(), \"" + fileLocation + "\" is not a valid XML document.");
        }

        // Attempt to parse through the document. Start by obtaining the sample amount.
        const auto pathNode             = xml.child ("Path");
        const auto pathName             = std::string (pathNode.attribute ("Name").as_string());
        const auto samplesPerSegment    = pathNode.attribute ("SamplesPerSegment").as_uint();
        const auto forceContinuity      = pathNode.attribute ("ForceContinuity").as_bool();

        // Keep track of the number of segments.
        size_t segmentCount             { 0 };

        // Now attempt to create each segment.
        for (const auto segmentNode : pathNode.children ("Segment"))
        {
            // Create our new segment.
            auto segment = std::make_shared<Segment>();

            // We'll keep track of the number of points to ensure we have a valid number of points.
            size_t pointCount   { 0 };

            // Initialise each point.
            for (const auto pointNode : segmentNode.children ("Point"))
            {
                // Ensure we don't add more than four points.
                if (pointCount == 4)
                {
                    break;
                }

                // Create the point and add the waypoint.
                addPointFromXML (*segment, pointNode, pointCount++);
            }

            if (pointCount != 4)
            {
                throw std::runtime_error ("Path::loadFromXML(), segment did not have enough points, ensure each segment has four points.");
            }

            // Move the segment into our vector. Unfortunately we can't guess how big the vector should be.
            m_segments.push_back (segment);
        }

        // Now we can calculate the length of the path!
        calculateLength (samplesPerSegment);
        
        // Check if we should stitch the curves together.
        if (forceContinuity)
        {
            enforceContinuity();
        }

        // Construct the way points.
        constructWaypoints (ogre, root, pathName + "-Waypoint-");

        // And we're done with this lengthy process!
        return true;
    }

    catch (const std::exception& error)
    {
        // Output the actual error.
        std::cerr << "An error was caught in Path::loadFromXML(): " << error.what() << std::endl;
    }

    catch (...)
    {
        std::cerr << "An unknown error was caught in Path::loadFromXML()." << std::endl;
    }
    
    // Clear our data since we've failed.
    m_segments.clear();
    m_waypoints.clear();

    // Indicate failure.
    return false;
}

#pragma endregion


#pragma region Core functionality

float Path::calculateLength (const unsigned int samplesPerSegment)
{
    // Pre-condition: Ensure we have a valid sample count.
    const auto samples  = samplesPerSegment == 0 ? 100 : samplesPerSegment;

    // Start by creating our accumulator. Check whether we actually have any segments to calculate. -1.f represents an "uninitialised" state.
    float accumulator   { m_segments.size() == 0 ? -1.f : 0.f };

    // Calculate the length of each segment.
    for (auto& segment : m_segments)
    {
        // We should store the value and check if it is valid.
        const float segmentLength   { segment->calculateLength (samples) };

        if (segmentLength <= 0.f)
        {
            // Exit the loop and report the bad values.
            accumulator = -1.f;
            break;
        }

        // Add the segment length since it is valid.
        accumulator += segmentLength;
    }

    // Update the length variable and return it.
    return (m_length = accumulator);
}


Ogre::Vector3 Path::curvePoint (const unsigned int index, const float delta, const Derivative derivative) const
{
    // Use at to ensure there are no access violation errors.
    try
    {
        return m_segments.at (index)->curvePoint (delta, derivative);
    }

    // Just ignore the exception and return a standard vector.
    catch (...)
    {
        return { };
    }
}

#pragma endregion


#pragma region Helper functions

void Path::addPointFromXML (Segment& segment, const pugi::xml_node& pointNode, const size_t pointIndex)
{
    // Read in the point.
    Ogre::Vector3 point {   pointNode.attribute ("X").as_float(),
                            pointNode.attribute ("Y").as_float(),
                            pointNode.attribute ("Z").as_float() };

    // Set the point in the segment and increment the point count.
    segment.setPoint (pointIndex, point);
}


void Path::constructWaypoints (OgreApplication* const ogre, Ogre::SceneNode* const root, const std::string& name)
{
    // Loop through the entire data structure making waypoints.
    const unsigned int controlPoints = 4;
    const unsigned int curvePoints = 200 / m_segments.size();

    // Get the vector ready.
    m_waypoints.clear();
    m_waypoints.resize (controlPoints * m_segments.size() + curvePoints * m_segments.size());

    for (unsigned int y = 0; y < m_segments.size(); ++y)
    {
        // Cache the segment.
        auto& segment = m_segments[y];

        // Create each control point.
        for (unsigned int x = 0; x < controlPoints; ++x)
        {
            // We need a unique name.
            const auto waypointName = name + "Control-" + std::to_string (y) + "-" + std::to_string (x);

            // Add it to the vector.
            m_waypoints.push_back ( std::unique_ptr<Waypoint> (createWaypoint (ogre, root, waypointName, segment->getPoint (x))));
        }

        // Create each curve point.
        for (unsigned int x = 0; x <= curvePoints; ++x)
        {
            // We need a unique name.
            const auto waypointName = name + "Curve-" + std::to_string (y) + "-" + std::to_string (x);

            // The point on the bezier curve.
            const float curve       = x / static_cast<float> (curvePoints);

            // Add it to the vector.
            m_waypoints.push_back ( std::unique_ptr<Waypoint> (createWaypoint (ogre, root, waypointName, segment->curvePoint (curve))));
        }
    }
}


Path::Waypoint* Path::createWaypoint (OgreApplication* const ogre, Ogre::SceneNode* const root, const std::string& name, const Ogre::Vector3& position)
{
    // Create the waypoint.
    auto waypoint = new Waypoint();
    waypoint->initialise (ogre, root, name);

    // Move the waypoint to the correct position.
    waypoint->setPosition (position);
    waypoint->setScale (m_waypointScale);

    return waypoint;
}


void Path::enforceContinuity()
{
    // Initialise the two pointer variables we will be using.
    std::shared_ptr<Segment>    previous    { m_segments.front() }, 
                                current     { nullptr };

    // Start at the second segment since that is where the stitching first occurs.
    for (unsigned int i = 1; i < m_segments.size(); ++i)
    {
        // Update the current value.
        current = m_segments[i];

        // We need to calculate the angle between the directions A3-A2 and B1-B0.
        const auto perpendicular    = (previous->getPoint (3) - previous->getPoint (2)).normalisedCopy();
        const auto collinearGoal    = (current->getPoint (1) - current->getPoint (0)).normalisedCopy();

        // Calculate the angle between the points we need to make collinear.
        const auto angle            = perpendicular.angleBetween (collinearGoal).valueDegrees();
        
        std::cout << std::to_string (Ogre::Degree (angle).valueRadians()) << std::endl;

        // Rotate each point.
        const auto rotation = util::rotationY (std::abs (angle));
        current->rotate (rotation);        
        

        // We must make sure that A3 and B0 match by translating the entire curve.
        const auto matchPoints = previous->getPoint (3) - current->getPoint (0);
        current->translate (matchPoints);

        // Now we need to ensure that we have tangential continuity.
        const auto direction        = (previous->getPoint(3) - previous->getPoint (2));
        const auto lengthCorrection = direction.length();

        // Translate B1-B3 to B0.
        current->translate (current->getPoint(0) - current->getPoint (1));

        // Ensure B1 has an equal distance from B0 and A2.
        current->translate (direction);

        // Correct B0.
        current->translatePoint (0, previous->getPoint (3) - current->getPoint (0));

        // Don't forget to loop through each segment! I actually forgot, silly sausage.
        previous = current;
    }
}

#pragma endregion