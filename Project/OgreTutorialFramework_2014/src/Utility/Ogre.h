#pragma once

#ifndef _UTIL_OGRE_
#define _UTIL_OGRE_


namespace util
{
    /// <summary> Creates a new material based on the information given, this can then be used by entities. </summary>
    /// <param name="materialName"> The name the material should be referenced by. </param>
    /// <param name="textureName"> The name of the texture file to apply to the material. </param>
    void createMaterial (const Ogre::String& materialName, const Ogre::String& textureName);
}


#endif // _UTIL_OGRE_