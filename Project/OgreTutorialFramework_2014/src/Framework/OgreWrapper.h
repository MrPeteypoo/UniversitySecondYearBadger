#pragma once

#ifndef _OGREWRAPPER_H_
#define _OGREWRAPPER_H_


#include <memory>


/// \brief This class contains a function to initialise Ogre3d in one go.
class OgreWrapper
{
private:
	Ogre::Root *root;					
	Ogre::RenderWindow *window;					

private:
	// These functions will prevent the user and the class to creates copies of the original object.
	OgreWrapper(const OgreWrapper& source);						

	OgreWrapper& operator=(const OgreWrapper& source);			

	void UnLoadResources();			
public:
	
	OgreWrapper();
	~OgreWrapper();

	bool Initialize();
									
	Ogre::Root *GetRoot() const {return root;}					
    Ogre::RenderWindow *GetWindow() const { return window; }
									
};

#endif

