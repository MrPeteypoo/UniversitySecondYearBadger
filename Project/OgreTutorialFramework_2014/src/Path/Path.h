#pragma once

#ifndef _PATH_
#define _PATH_


// STL headers.
#include <memory>


// Forward declarations
namespace pugi { class xml_node; }
class OgreApplication;


/// <summary>
/// An enumeration to represent which derivative is desired from a curve point on the path.
/// </summary>
enum class Derivative : int
{
    None    = 0,
    First   = 1,
    Second  = 2
};


/// <summary>
/// A management class which creates a path based on cubic bezier curve segments.
/// </summary>
class Path final
{
    private:

        // Forward declarations.
        class Waypoint;

    public:

        // Forward declarations.
        class Segment;

        #pragma region Constructors and destructor

        Path();

        Path (Path&& move);
        Path& operator= (Path&& move);

        ~Path();

        Path (const Path& copy)             = delete;
        Path& operator= (const Path& copy)  = delete;
        
        #pragma endregion

        #pragma region Getters
        
        /// <summary> Gets a bezier curve segment from the given distance value, this is more expensive than by index. </summary>
        /// <param name="distance"> The distance to check for. If this is larger than the length of the path then the distance will cycle. </param>
        /// <returns> The segment which the distance resides in. A nullptr if the distance has not been calculated. </returns>
        const std::shared_ptr<const Segment> segmentByDistance (float distance) const;

        /// <summary> Gets a bezier curve segment from a given index value. </summary>
        /// <param name="index"> The segment number. </param>
        /// <returns> The desired segment, if an invalid index is given a nullptr is returned. </returns>
        const std::shared_ptr<const Segment> getSegment (const unsigned int index) const;

        /// <summary> Gets the current number of segments that exist. </summary>
        size_t getSegmentCount() const                  { return m_segments.size(); }

        /// <summary> Gets the scale applied to waypoints.
        const Ogre::Vector3& getWaypointScale() const   { return m_waypointScale; }

        /// <summary> Gets the most recently calculated arc length for the path. </summary>
        /// <returns> The total arc length of the path, -1.f if the path has not been initialised. </returns>
        float getLength() const                         { return m_length; }

        /// <summary> Sets the scale which is applied to waypoints. This can also be used to update the current waypoints. </summary>
        /// <param name="scale"> The scale to apply to waypoints. </param>
        /// <param name="updateCurrent"> Indicates whether the function should update the scale of currently created waypoints. </param>
        void setWaypointScale (const Ogre::Vector3& scale, const bool updateCurrent = false);

        #pragma endregion

        #pragma region Initialisation

        /// <summary> Loads the entire path from an XML file. Throws exceptions if an error occurs. </summary>
        /// <param name="fileLocation"> The location of the .xml file to load. </param>
        /// <param name="ogre"> The application used
        /// <returns> Whether the loading was successful or not. </returns>
        bool loadFromXML (const std::string& fileLocation, OgreApplication* const ogre, Ogre::SceneNode* const root);

        #pragma endregion

        #pragma region Core functionality

        /// <summary> Samples each segment to calculate the approximate length of the path. This value is accessible via getLength(). </summary>
        /// <param name="samplesPerSegment"> The number of samples to use per segment, the higher the more accurate. 100+ will often provide visually accurate results. </param>
        /// <returns> The calculated length, also accessible from getLength(). </returns>
        float calculateLength (const unsigned int samplesPerSegment);

        /// <summary> Calculates the point on the bezier curve of the given segment. </summary>
        /// <param name="index"> The segment number to access. </param>
        /// <param name="delta"> The delta value between 0.f and 1.f for the curve point on the chosen segment. </param>
        /// <param name="derivative"> Which derivative to calculate, none will be the curve point itself, first is a tangent vector and second is a curviture vector. </param>
        /// <returns> The calculated curve point according to the given derivative. 0, 0, 0 if index is invalid. </returns>
        Ogre::Vector3 curvePoint (const unsigned int index, const float delta, const Derivative derivative = Derivative::None) const;

        #pragma endregion

    private:

        #pragma region Helper functions

        /// <summary> Parses through an XML node attempting to construct a point for a given segment. </summary>
        /// <param name="segment"> The segment to add the generated point to. </param>
        /// <param name="pointNode"> The node to parse. </param>
        /// <param name="pointNumber"> The index to use when adding it to the segment. </param>
        void addPointFromXML (Segment& segment, const pugi::xml_node& pointNode, const size_t pointIndex);
        
        /// <summary> Creates a waypoint in the scene at the given position </summary>
        /// <param name="ogre"> Used to initialse the waypoint entity. </param>
        /// <param name="root"> The node to attach the waypoint to. </param>
        /// <param name="name"> The unique name to call the waypoing. </param>
        /// <param name="position"> Where to place the waypoint. </param>
        void createWaypoint (OgreApplication* const ogre, Ogre::SceneNode* const root, const std::string& name, const Ogre::Vector3& position);

        #pragma endregion

        #pragma region Implementation data

        std::vector<std::shared_ptr<Segment>>   m_segments      {  };               //!< A vector of segments, used to represent an entire bezier curve segment of the total path.
        std::vector<std::unique_ptr<Waypoint>>  m_waypoints     {  };               //!< A vector of waypoints used to visually represent the track.

        Ogre::Vector3                           m_waypointScale { 1.f, 1.f, 1.f };  //!< The scale vector used for waypoints. This should be controlled externally.
        
        float                                   m_length        { -1.f };           //!< The total calculated length of the path.

        #pragma endregion

};

#endif // _PATH_