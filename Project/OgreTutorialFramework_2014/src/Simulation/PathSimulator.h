#pragma once

#ifndef _PATH_SIMULATOR_
#define _PATH_SIMULATOR_


// STL headers.
#include <memory>


// Engine headers.
#include <Simulation/ISimulator.h>
#include <Path/Path.h>


// Forward declarations.
class Badger;


/// <summary>
/// A simulator which manages a badger which follows a bezier path.
/// </summary>
class PathSimulator final : public ISimulator
{
    public:

        #pragma region Constructors and destructor

        PathSimulator();

        PathSimulator (PathSimulator&& move);
        PathSimulator& operator= (PathSimulator&& move);

        ~PathSimulator() override final;

        PathSimulator (const PathSimulator& copy)             = delete;
        PathSimulator& operator= (const PathSimulator& copy)  = delete;

        #pragma endregion

        #pragma region Getters and settes

        /// <summary> Gets the time to complete a full path simulation in seconds. </summary>
        float getTimeToComplete() const { return m_timeToComplete; }

        /// <summary> Sets how long it should take in seconds for the path simulation to complete a cycle. </summary>
        /// <param name="time"> Negative values will be made absolute. 0.f is ignored. </param>
        void setTimeToComplete (const float time);

        /// <summary> An alternative to setTimeToComplete. This sets the completion time based on the movement speed given. </summary>
        /// <param name="distancePerSecond"> The distance to be travelled per second, this will not work if the simulator is not initialised. 0.f is ignored. </param>
        void movementSpeed (const float distancePerSecond);

        #pragma endregion

        #pragma region ISimulator functionality

        /// <summary> Initialises the path and badger ready for simulation. </summary>
        /// <param name="ogre"> The OgreApplication which contains the SceneManager required to function. </param>
        bool initialise (OgreApplication* const ogre) override final;

        /// <summary> Resets the PathSimulator to initial values, completely restarting the PathSimulator. </summary>
        void reset() override final;

        /// <summary> Updates the entire PathSimulator. </summary>
        /// <param name="deltaTime"> The value in seconds to update the PathSimulator with. </param>
        void update (const float deltaTime) override final;

        #pragma endregion

    private:

        #pragma region Initialisation

        /// <summary> Attempts to load the badger ready for the simulation. </summary>
        /// <param name="ogre"> The OgreApplication required to initialise the path object. </param>
        /// <param name="root"> The SceneNode to attach the badger to. </param>
        void loadBadger (OgreApplication* const ogre, Ogre::SceneNode* const root);

        /// <summary> Causes the simulator to initialise the path with an XML file. </summary>
        /// <param name="ogre"> The OgreApplication required to initialise the path object. </param>
        /// <param name="root"> The SceneNode to attach the waypoints to. </param>
        void loadPath (OgreApplication* const ogre, Ogre::SceneNode* const root);

        /// <summary> Obtains user input to determine the location of the XML file to use. </summary>
        /// <returns> The valid file location. </returns>
        std::string obtainFileLocation() const;

        #pragma endregion

        #pragma region Simulation management

        /// <summary> Obtains a segment at the specified index </summary>
        void obtainSegment (const size_t segment);

        /// <summary> Calculates the inverse of the speed of the previous tangent vector, creating a step to increase time by. </summary>
        /// <returns> The inverse of the speed, this can be manipulated by deltaTime to create a normalised speed. </returns>
        float timeIncrement() const;

        #pragma endregion

        #pragma region Implmentation data
        
        std::unique_ptr<Badger>                 m_badger            { nullptr };    //!< The badger vehicle used to demonstrate the bezier curve path.
        std::unique_ptr<Path>                   m_path              { nullptr };    //!< The path which the badger will follow.

        std::shared_ptr<const Path::Segment>    m_segment           { nullptr };    //!< The current segment. Allows for quicker curve calculations.

        Ogre::Vector3                           m_previousTangent   {  };           //!< The tangent of the previous curve point, avoids calculating it again.

        unsigned int                            m_segmentIndex      { 0 };          //!< The current index of the current segment.

        float                                   m_timeToComplete    { 20.f };        //!< How long it should take run through the entire path.

        float                                   m_time              { 0.f };        //!< The current time value used to track where on the curve we should be.
        float                                   m_timeForSegment    { 0.f };        //!< Keeps track of how long each segment has taken to complete in seconds.

        #pragma endregion

};

#endif // _PATH_SIMULATOR_