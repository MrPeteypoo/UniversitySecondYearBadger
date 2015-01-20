#include "Camera.h"


Camera::Camera (const std::shared_ptr<Ogre::SceneNode>& sceneNode, const std::shared_ptr<Ogre::MovableObject>& camera) : m_sceneNode (sceneNode), m_camera (camera)
{
}


void Camera::Update(OIS::Keyboard* keyboard, OIS::Mouse* mouse)
{
	Ogre::Vector3 currentPosition = m_sceneNode->getPosition();
	Ogre::Quaternion currentRotation = m_sceneNode->getOrientation();

	Ogre::Vector3 right = Ogre::Vector3::UNIT_X;
	Ogre::Vector3 up = Ogre::Vector3::UNIT_Y;
	Ogre::Vector3 look = Ogre::Vector3::UNIT_Z;

	right = currentRotation * right;
	up = currentRotation * up;
	look = currentRotation * look;

	float forwardScale = 0.0f;
	float sideScale = 0.0f;
	float verticalScale = 0.0f;

	if (keyboard->isKeyDown(OIS::KC_W))
		forwardScale = 1.0f;
	else if (keyboard->isKeyDown(OIS::KC_S))
		forwardScale = -1.0f;

	if (keyboard->isKeyDown(OIS::KC_D))
		sideScale = -1.0f;
	else if (keyboard->isKeyDown(OIS::KC_A))
		sideScale = 1.0f;

	if (keyboard->isKeyDown(OIS::KC_Q) || keyboard->isKeyDown(OIS::KC_HOME))
		verticalScale = 1.0f;
	else if (keyboard->isKeyDown(OIS::KC_Z) || keyboard->isKeyDown(OIS::KC_END))
		verticalScale = -1.0f;

	currentPosition += (look * forwardScale);
	currentPosition += (up * verticalScale);
	currentPosition += (right * sideScale);

	m_sceneNode->setPosition(currentPosition);
	std::static_pointer_cast<Ogre::Camera>(m_camera)->lookAt(currentPosition + look);

	if (mouse->getMouseState().X.abs > mouse->getMouseState().width)
		return;

	if (mouse->getMouseState().Y.abs > mouse->getMouseState().height)
		return;

	const int mouseX = mouse->getMouseState().X.rel;
	const int mouseY = mouse->getMouseState().Y.rel;

	m_rotation.x += mouseX;
	m_rotation.y += mouseY;

	Ogre::Quaternion newRotationX, newRotationY;
	newRotationX.FromAngleAxis((Ogre::Radian)(m_rotation.x * -0.0017f), Ogre::Vector3::UNIT_Y);
	newRotationY.FromAngleAxis((Ogre::Radian)(m_rotation.y *  0.0017f), Ogre::Vector3::UNIT_X);

	m_sceneNode->setOrientation(newRotationX * newRotationY);
}