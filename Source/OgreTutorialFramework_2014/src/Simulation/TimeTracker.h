#pragma once

#ifndef _TIME_TRACKER_
#define _TIME_TRACKER_


/// <summary>
/// A little helper class which can be used for keeping track of time. This can be very helpful for animation.
/// </summary>
class TimeTracker final
{
    public:

        #pragma region Constructors and destructor
        
        /// <summary> 
        /// Allows the specifying of minimum and maximum values for the tracker. Values will be swapped if min is more than max. 
        /// </summary>
        /// <param name="min"> The lowest accepted minimum is zero. </param>
        /// <param
        TimeTracker (const float min, const float max);

        TimeTracker (TimeTracker&& move);
        TimeTracker& operator= (TimeTracker&& move);
        
        TimeTracker()                                       = default;
        TimeTracker (const TimeTracker& copy)               = default;
        TimeTracker& operator= (const TimeTracker& copy)    = default;
        ~TimeTracker()                                      = default;

        #pragma endregion

        #pragma region Getters and setters

        /// <summary> Indicates whether the tracker is reducing or increasing the current time value. </summary>
        bool isAscending() const    { return m_isAscending; }

        /// <summary> Indicates whether the tracker has reached the target time. </summary>
        bool isFinished() const     { return m_current == m_target; }
        
        float getMin() const        { return m_min; }
        float getMax() const        { return m_max; }
        float getCurrent() const    { return m_current; }
        float getTarget() const     { return m_target; }

        void setMin (const float min);
        void setMax (const float max);
        void setCurrent (const float current);
        void setTarget (const float target);
        void setValues (const float min, const float max);
        void setValues (const float min, const float max, const float target);
        void setValues (const float min, const float max, const float target, const float current);

        /// <summary> Sets the target to a point between the minimum and maximum values. </summary>
        /// <param name="target"> The point between minimum and maximum, 0.f to 1.f. </param>
        void setNormalisedTarget (const float target);

        /// <summary> Sets the current value to the neutral position (in-between min and max). </summary>
        void setToNeutral()         { m_current = neutral(); }

        /// <summary> Sets the current value to the minimum value. </summary>
        void setToMin()             { m_current = m_min; }

        /// <summary> Sets the current value to the maximum value. </summary>
        void setToMax()             { m_current = m_max;}

        /// <summary> Sets the current value to the target value. </summary>
        void setToTarget()          { m_current = m_target; }

        #pragma endregion

        #pragma region Tracking functionality

        /// <summary> Calculates the point in-between the minimum and maximum values. </summary>
        float neutral() const           { return (m_min + m_max) / 2.f; }

        /// <summary> Calculates the current progress of the TimeTracker as a percentage. </summary>
        float normalisedCurrent() const { return (m_current - m_min) / (m_max - m_min); }

        /// <summary> Calculates the target of the TimeTracker as a percentage of the min and max values given. </summary>
        float normalisedTarget() const  { return (m_target - m_min) / (m_max - m_min); }

        /// <summary> Updates the current time value based on the given deltaTime value. </summary>
        void updateTime (float deltaTime);

        #pragma endregion

    private:

        float   m_min           { 0.f };    //!< The minimum time value.
        float   m_max           { 0.f };    //!< The maximum time value.

        float   m_current       { 0.f };    //!< The current time value.
        float   m_target        { 0.f };    //!< The target point for the TimeTracker to aim for.

        bool    m_isAscending   { true };   //!< A flag to indicate whether the TimeTracker should be increasing or decreasing.
};


#endif // _TIME_TRACKER_