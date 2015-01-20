#pragma once

#ifndef _CAMERA_
#define _CAMERA_


// STL headers.
#include <memory>


class Camera
{
    public:
	    Camera (const std::shared_ptr<Ogre::SceneNode>& sceneNode, const std::shared_ptr<Ogre::MovableObject>& camera);
	    ~Camera() = default;

	    void Update(OIS::Keyboard* keyboard, OIS::Mouse* mouse);

    private:

        std::shared_ptr<Ogre::SceneNode> m_sceneNode    { nullptr };
        std::shared_ptr<Ogre::MovableObject> m_camera   { nullptr };

        Ogre::Vector3 m_rotation                        { Ogre::Vector3::ZERO };
};

#endif