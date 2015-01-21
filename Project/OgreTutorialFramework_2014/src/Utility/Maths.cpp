#include "Maths.h"



namespace util
{

    #pragma region Miscellaneous

    float fixYaw (const float yaw, const float limit)
    {
        // Obtain a copy of PI.
        const float pi = Ogre::Math::PI;

        // Create a temporary result.
        float result = yaw;

        if (yaw >= limit)
        {
            result = -pi + yaw;
        }

        else if (yaw <= -limit)
        {
            result = pi + yaw;
        }

        // Return the calculated value.
        return clamp (result, -limit, limit);
    }

    #pragma endregion


    #pragma region Rotation functions

    Ogre::Matrix3 rotationMatrix (const Ogre::Vector3& orientation)
    {		
        return  rotationZ (orientation.z) * 
                rotationY (orientation.y) * 
                rotationX (orientation.x);
    }


    Ogre::Matrix3 rotationMatrix (const float x, const float y, const float z)
    {
        return  rotationZ (z) *
                rotationY (y) *
                rotationX (x);
    }


    Ogre::Matrix3 rotationX (const float angle)
    {
        const float radians = angle * Ogre::Math::fDeg2Rad;
        return
        {
            1.f,    0.f,                0.f,
            0.f,    std::cos (radians), -std::sin (radians),
            0.f,    std::sin (radians), std::cos (radians)
        };
    }


    Ogre::Matrix3 rotationY (const float angle)
    {
        const float radians = angle * Ogre::Math::fDeg2Rad;
        return 
        {
            std::cos (radians),     0.f,    std::sin (radians),
            0.f,                    1.f,    0.f,
            -std::sin (radians),    0.f,    std::cos (radians)
        };
    }


    Ogre::Matrix3 rotationZ (const float angle)
    {
        const float radians = angle * Ogre::Math::fDeg2Rad;
        return
        {
            std::cos (radians), -std::sin (radians),    0.f,
            std::sin (radians), std::cos (radians),     0.f,
            0.f,                0.f,                    1.f
        };
    }

    #pragma endregion Rotation functions

}