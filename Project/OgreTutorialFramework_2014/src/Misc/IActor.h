#pragma once

#ifndef _IACTOR_
#define _IACTOR_


// STL headers.
#include <memory>


// Forward declarations.
class OgreApplication;


/// <summary>
/// A basic actor interface, should be used for all in-game actors.
/// </summary>
class IActor
{
    public:

        #pragma region Actor functionality

        /// <summary> Virtual destructor ensures no inheritance issues. </summary>
        virtual ~IActor() {}

        /// <summary> Causes an Actor to initialise itself. This should be called before using any Actor. </summary>
        /// <param name="ogre"> The OgreApplication used for creating entities. </param>
        /// <param name="root"> The SceneNode to attach the actor to. </param>
        /// <param name="name"> The name to instantiate the SceneNode with. </param>
        /// <returns> Returns whether the initialisation worked or not. </returns>
        virtual bool initialise (OgreApplication* const ogre = nullptr, Ogre::SceneNode* const root = nullptr, const Ogre::String& name = { }) = 0;

        /// <summary> Causes an Actor to update itself based on the delta-time value given. This should not update any simulation information. </summary>
        /// <param name="deltaTime"> The time in seconds since the last update. </param>
        virtual void update (const float deltaTime) {}

        /// <summary> Causes an Actor to perform an update of the simulation based on the delta-time value given. This is where all physics calculations should go. </summary>
        /// <param name="deltaTime"> The time in seconds since the last update. </param>
        virtual void updateSimulation (const float deltaTime) {}

        #pragma endregion

        #pragma region Getters and setters

        const std::unique_ptr<Ogre::SceneNode>& getNode() const     { return m_node; }

        const Ogre::Vector3& getPosition() const                    { return m_node->getPosition(); }
        const Ogre::Quaternion& getOrientation() const              { return m_node->getOrientation(); }
        const Ogre::Vector3& getScale() const                       { return m_node->getScale(); }

        void setPosition (const Ogre::Vector3& position)            { m_node->setPosition (position); }
        void setOrientation (const Ogre::Quaternion& orientation)   { m_node->setOrientation (orientation); }
        void setScale (const Ogre::Vector3& scale)                  { m_node->setScale (scale); }

        #pragma endregion

    protected:

        #pragma region Ogre creation

        /// <summary>
        /// Creates the visual entity for the IActor.
        /// </summary>
        /// <param name="ogre"> The OgreApplication used for creating entities. </param>
        /// <param name="mesh"> The filename for the mesh to be used. </param>
        /// <param name="material"> The material name to use for the entity. </param>
        /// <returns> Returns the completed entity. </returns>
        virtual Ogre::Entity* constructEntity (OgreApplication* const ogre, const Ogre::String& mesh, const Ogre::String& material = "green");

        /// <summary>
        /// Constructs a SceneNode for an IActor which will used in-game.
        /// </summary>
        /// <param name="root"> The root SceneNode to attach the IActor to. </param>
        /// <param name="name"> The unique name to give the SceneNode. </param>
        /// <param name="attachable"> A MovableObject to attach to the created SceneNode, can be a nullptr. </param>
        /// <param name="position"> The position to initialise the SceneNode to. </param>
        /// <param name="orientation"> The orientation to initialise the SceneNode to. </param>
        /// <param name="scale"> The scale to initialise the SceneNode to. </param>
        /// <returns> Returns the completed SceneNode. </returns>
        virtual Ogre::SceneNode* constructNode (Ogre::SceneNode* const root, const Ogre::String& name, Ogre::MovableObject* const attachable = nullptr, const Ogre::Vector3& position = { }, const Ogre::Quaternion& orientation = Ogre::Quaternion(), const Ogre::Vector3& scale = { 1.f, 1.f, 1.f });

        #pragma endregion

        #pragma region Implementation data

        std::unique_ptr<Ogre::SceneNode>    m_node  { nullptr };    //!< The in-game SceneNode used to represent the actor.

        #pragma endregion
};


#endif // _IACTOR_