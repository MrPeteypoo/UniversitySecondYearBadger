#include "IActor.h"



// STL headers.
#include <exception>



// Engine headers.
#include <Framework/OgreApplication.h>



#pragma region Ogre creation

Ogre::Entity* IActor::constructEntity (OgreApplication* const ogre, const Ogre::String& mesh, const Ogre::String& material)
{
    // Create the entity.
    auto entity = ogre->GetSceneManager()->createEntity (mesh);
    entity->setCastShadows (false);

    // Create the material to be used.
    entity->setMaterialName (material);
    
    return entity;
}


Ogre::SceneNode* IActor::constructNode (Ogre::SceneNode* const root, const Ogre::String& name, Ogre::MovableObject* const attachable, const Ogre::Vector3& position, const Ogre::Quaternion& orientation, const Ogre::Vector3& scale)
{
    // Pre-condition: Valid root node given.
    if (!root)
    {
        throw std::invalid_argument ("IActor::constructNode(), required parameter 'root' is a nullptr.");
    }

    // Create the node.
    const auto node = root->createChildSceneNode (name);
    
    // Attach the object if possible.
    if (attachable)
    {
        node->attachObject (attachable);
    }

    // Set up the default node values.
    node->setPosition (position);
    node->setOrientation (orientation);
    node->setScale (scale);

    // We're done! Yay!
    return node;
}

#pragma endregion Ogre creation