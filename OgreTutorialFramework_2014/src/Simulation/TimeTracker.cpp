#include "TimeTracker.h"



// STL headers.
#include <cmath>



// Engine headers.
#include <Utility/Maths.h>



#pragma region Constructors and destructor

TimeTracker::TimeTracker (float min, float max) : m_min (min), m_max (max)
{
    // Default to having the tracker go from min to max.
    setValues (min, max, max, min);
}


TimeTracker::TimeTracker (TimeTracker&& move)
{
    *this = std::move (move);
}


TimeTracker& TimeTracker::operator= (TimeTracker&& move)
{
    if (this != &move)
    {
        m_min = std::move (move.m_min);
        m_max = std::move (move.m_max);
        m_current = std::move (move.m_current);
        m_target = std::move (move.m_target);
        m_isAscending = std::move (move.m_isAscending);
    }

    return *this;
}

#pragma endregion


#pragma region Getters and setters

void TimeTracker::setMin (const float min)
{
    // Value must be positive and lower than max.
    m_min = util::clamp (min, 0.f, m_max);
    m_current = util::max (m_min, m_current);
    m_target = util::max (m_min, m_target);
}


void TimeTracker::setMax (const float max)
{
    // Value must be higher than the minimum.
    m_max = util::max (max, m_min);
    m_current = util::min (m_max, m_current);
    m_target = util::min (m_max, m_target);
}


void TimeTracker::setCurrent (const float current)
{
    // Value must be between min and max.
    m_current = util::clamp (current, m_min, m_max);
    m_isAscending = m_target > m_current;
}


void TimeTracker::setTarget (const float target)
{
    // Value must be between min and max.
    m_target = util::clamp (target, m_min, m_max);
    m_isAscending = m_target > m_current;
}


void TimeTracker::setNormalisedTarget (const float target)
{
    // Calculate the difference using a clamped target value.
    const float difference  { (m_max - m_min) * util::clamp (target, 0.f, 1.f) };

    // Set the target as normal.
    m_target = m_min + difference;
    m_isAscending = m_target > m_current;
}


void TimeTracker::setValues (const float min, const float max)
{
    setValues (min, max, m_target);
}


void TimeTracker::setValues (const float min, const float max, const float target)
{
    setValues (min, max, target, m_current);
}


void TimeTracker::setValues (const float min, const float max, const float target, const float current)
{
    // Ensure safety of all variables.
    m_min = util::max (0.f, min);
    m_max = util::max (0.f, max);

    if (m_min > m_max)
    {
        std::swap (m_min, m_max);
    }

    // m_min and m_max are now safe, clean up with m_target and m_current.
    m_target = util::clamp (target, m_min, m_max);
    m_current = util::clamp (current, m_min, m_max);
    m_isAscending = m_target > m_current;
}

#pragma endregion


#pragma region Tracking functionality

void TimeTracker::updateTime (float deltaTime)
{
    // Ensure the deltaTime value is positive.
    deltaTime = std::abs (deltaTime);

    // Calculate the current value with the added deltaTime. Ensure we don't go past the target.
    m_current = m_isAscending ? 
                                util::min (m_current + deltaTime, m_target) :
                                util::max (m_current - deltaTime, m_target);
}

#pragma endregion