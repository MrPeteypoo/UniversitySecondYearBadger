#pragma once

#ifndef _UTIL_MATHS_
#define _UTIL_MATHS_


// STL headers.
#include <type_traits>


namespace util
{
    #pragma region Miscellaneous

    /// <summary> Checks if two float values are relatively equal to each other. </summary>
    /// <param name="margin"> The absolute margin of error between the two floats. Must be a positive value. </param>
    inline bool roughlyEquals (const float lhs, const float rhs, const float margin = 0.1f)
    {
        // Test the upper and lower limits.
        return std::abs (lhs - rhs) <= margin;
    }


    /// <summary> Ogre is picky, it alternates between 0 and PI for yaw values so this function corrects them to be between -limit and limit. </summary>
    /// <param name="yaw"> The value to fix. </param>
    /// <param name="limit"> The range which the yaw value should be limited to. This value is expected to be positive. </param>
    float fixYaw (const float yaw, const float limit);

    #pragma endregion

    #pragma region Comparison functions

    /// <summary> Returns the minimum value, passed by value for arithmetic types. </summary>
    template <typename T> typename std::enable_if<std::is_arithmetic<T>::value, T>::type min (const T a, const T b)
    {
        return a < b ? a : b;
    }
    

    /// <summary> Returns the maximum value, passed by value for arithmetic types. </summary>
    template <typename T> typename std::enable_if<std::is_arithmetic<T>::value, T>::type max (const T a, const T b)
    {
        return a > b ? a : b;
    }


    /// <summary> Returns the minimum value, passed by reference for non-arithmetic types. </summary>
    template <typename T> typename std::enable_if<!std::is_arithmetic<T>::value, T>::type& min (const T& a, const T& b)
    {
        return a < b ? a : b;
    }


    /// <summary> Returns the maximum value, passed by reference for non-arithmetic types. </summary>
    template <typename T> typename std::enable_if<!std::is_arithmetic<T>::value, T>::type& max (const T& a, const T& b)
    {
        return a > b ? a : b;
    }
    

    /// <summary> Clamps a value between a given minimum and maximum value. Arithmetic types are passed by value. </summary>
    /// <param name="value"> The value to clamp. </param>
    template <typename T> typename std::enable_if<std::is_arithmetic<T>::value, T>::type clamp (const T value, const T min, const T max)
    {
        if (value < min)
        {
            return min;
        }

        if (value > max)
        {
            return max;
        }

        return value;
    }


    /// <summary> Clamps a value between a given minimum and maximum value. Non-arithmetic types are passed by reference. </summary>
    /// <param name="value"> The value to clamp. </param>
    template <typename T> typename std::enable_if<!std::is_arithmetic<T>::value, T>::type clamp (const T& value, const T& min, const T& max)
    {
        if (value < min)
        {
            return min;
        }

        if (value > max)
        {
            return max;
        }

        return value;
    }

    #pragma endregion

    #pragma region Rotation functions
    
    /// <summary>
    /// Creates a rotation matrix from the given orientation.
    /// <returns> Returns a matrix with the given orientation. </returns>
    /// </summary>
    /// <param name="orientation"> The X, Y and Z angles of the matrix. </param>
	Ogre::Matrix3 rotationMatrix (const Ogre::Vector3& orientation);


    /// <summary>
    /// Creates a rotation matrix from the given orientation.
    /// <returns> Returns a matrix with the given orientation. </returns>
    /// </summary>
    /// <param name="x"> The X angle of the matrix. </param>
    /// <param name="y"> The Y angle of the matrix. </param>
    /// <param name="z"> The Z angle of the matrix. </param>
	Ogre::Matrix3 rotationMatrix (const float x, const float y, const float z);


    /// <summary>
    /// Creates a rotation matrix for a given angle of the X axis.
    /// <returns> A rotation matrix. </returns>
    /// </summary>
    /// <param name="angle"> The angle which the matrix should represent. </param>
	Ogre::Matrix3 rotationX (const float angle);


    /// <summary>
    /// Creates a rotation matrix for a given angle of the Y axis.
    /// <returns> A rotation matrix. </returns>
    /// </summary>
    /// <param name="angle"> The angle which the matrix should represent. </param>
	Ogre::Matrix3 rotationY (const float angle);


    /// <summary>
    /// Creates a rotation matrix for a given angle of the Z axis.
    /// <returns> A rotation matrix. </returns>
    /// </summary>
    /// <param name="angle"> The angle which the matrix should represent. </param>
	Ogre::Matrix3 rotationZ (const float angle);

    #pragma endregion


}

#endif // _UTIL_MATHS_