#include "Ogre.h"



namespace util
{
    void createMaterial (const Ogre::String& materialName, const Ogre::String& textureName)
    {
        // Cache the MaterialManager.
        auto& manager = Ogre::MaterialManager::getSingleton();

        // Determine if we need to create the material.
        if (manager.getByName (materialName).isNull())
        {
            // Create the material.
            const auto material = manager.create (materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

            // Set up the texture unit state for the entity.
            auto textureUnit = material->getTechnique (0)->getPass (0)->createTextureUnitState();
            textureUnit->setTextureName (textureName, Ogre::TEX_TYPE_2D);
            textureUnit->setTextureCoordSet (0);            
        }
    }
}