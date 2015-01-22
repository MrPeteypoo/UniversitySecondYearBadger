#include "Segment.h"



// Engine headers.
#include <Utility/Maths.h>



#pragma region Constructors

Path::Segment::Segment()
{
    // Ensure the segment has the correctly sized vector.
    m_points.resize (4);
}


Path::Segment::Segment (const Ogre::Vector3& p0, const Ogre::Vector3& p1, const Ogre::Vector3& p2, const Ogre::Vector3& p3)
{
    // Ensure the segment has the correctly sized vector.
    m_points.resize (4);

    // Set each point.
    m_points[0] = p0;
    m_points[1] = p1;
    m_points[2] = p2;
    m_points[3] = p3;
}


Path::Segment::Segment (Segment&& move)
{
    *this = std::move (move);
}


Path::Segment& Path::Segment::operator= (Segment&& move)
{
    if (this != &move)
    {
        // Path::Segment.
        m_points = std::move (move.m_points);
        
        m_length = std::move (move.m_length);
    }

    return *this;
}

#pragma endregion


#pragma region Getters and setters

const Ogre::Vector3& Path::Segment::getPoint (const unsigned int index) const
{
    // It's quicker to try, catch and succeed than to test each index value.
    try
    {
        return m_points.at (index);
    }

    // We don't need to care about the exception we caught.
    catch (...)
    {
        return m_points.back();
    }
}


void Path::Segment::setPoint (const unsigned int index, const Ogre::Vector3& point)
{
    // It's quicker to try, catch and succeed than to test each index value.
    try
    {
        m_points.at (index) = point;
    }

    // Do nothing with the caught exception.
    catch (...)
    {
    }
}

#pragma endregion


#pragma region Core functionality

float Path::Segment::calculateLength (const unsigned int samples)
{
    // Start by creating our accumulator. If the samples are equal to one then we should "uninitialise" the segment.
    float accumulator   { samples == 0 ? -1.f : 0.f };

    // We'll need to store the previous position.
    Ogre::Vector3 previous { m_points.front() };

    // Begin sampling at the desired frequency.
    for (unsigned int i = 1; i <= samples; ++i)
    {
        // Obtain the current position on the curve.
        const auto current = curvePosition (i / static_cast<float> (samples));

        // Add the magnitude onto the accumulator.
        accumulator += (current - previous).length();

        // Move onto the next sample.
        previous = current;
    }

    return (m_length = accumulator);
}


Ogre::Vector3 Path::Segment::curvePoint (const float delta, const Derivative derivative) const
{
    // Clamp the delta.
    const float clampedDelta    { util::clamp (delta, 0.f, 1.f) };

    // Determine which function to use.
    switch (derivative)
    {
        case Derivative::None:
            return curvePosition (clampedDelta);

        case Derivative::First:
            return curveTangent (clampedDelta);

        case Derivative::Second:
            return curveCurvature (clampedDelta);

        default:
            return curvePosition (clampedDelta);
    }
}

#pragma endregion


#pragma region Curve calculation

Ogre::Vector3 Path::Segment::curvePosition (const float delta) const
{
    // The formula for the bezier curve is:
    // P(s) = (1-s)^3 P0 + 3s(1-s)^2 P1 + 3s^2 (1-s)P2 + s^3 P3.

    // Start by calculating the inverse delta calculations.
    const float inverseDelta        { 1.f - delta },
                inverseDeltaSqr     { inverseDelta * inverseDelta },
                inverseDeltaCubed   { inverseDeltaSqr * inverseDelta };

    // Next calculate the delta square and cubic values.
    const float deltaSqr            { delta * delta },
                deltaCubed          { deltaSqr * delta };

    // Now for each part of the equation.
    const auto  first   = inverseDeltaCubed * m_points[0],
                second  = 3.f * delta * inverseDeltaSqr * m_points[1],
                third   = 3 * deltaSqr * inverseDelta * m_points[2],
                last    = deltaCubed * m_points[3];

    // Return the completed equation.
    return first + second + third + last;
}


Ogre::Vector3 Path::Segment::curveTangent (const float delta) const
{
    // The formula for the tangent vector is:
    // P'(s) = 3(1-s)^2 (P1-P0) + 6s(1-s)(P2-P1) + 3s^2 (P3-P2).

    // Start by calculating the inverse delta calculations.
    const float inverseDelta        { 1.f - delta },
                inverseDeltaSqr     { inverseDelta * inverseDelta };

    // Next calculate the delta square value.
    const float deltaSqr            { delta * delta };

    // Now for each part of the equation.
    const auto  first   = 3 * inverseDeltaSqr * (m_points[1] - m_points[0]),
                second  = 6 * delta * inverseDelta * (m_points[2] - m_points[1]),
                last    = 3 * deltaSqr * (m_points[3] - m_points[2]);

    // Return the completed equation.
    return first + second + last;
}


Ogre::Vector3 Path::Segment::curveCurvature (const float delta) const
{
    // The formula for the tangent vector is:
    // P''(s) = 6(1-s)(P2-2P1+P0) + 6s(P3-2P2+P1).

    // Start by calculating the inverse delta.
    const float inverseDelta        { 1.f - delta };

    // Now for each part of the equation.
    const auto  first   = 6 * inverseDelta * (m_points[2] - 2 * m_points[1] + m_points[0]),
                last    = 6 * delta * (m_points[3] - 2 * m_points[2] + m_points[1]);

    // Return the completed equation.
    return first + last;
}

#pragma endregion