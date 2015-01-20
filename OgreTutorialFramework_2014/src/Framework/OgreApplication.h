#pragma once

#ifndef _OGRE_APPLICATION_H_
#define _OGRE_APPLICATION_H_


#include <Framework/OgreWrapper.h>

using namespace std;

struct SceneNodeAndName
{
	Ogre::String Name;
	Ogre::SceneNode*  Node;
};

class OgreApplication
{

private:
	
	OgreWrapper	ogreWrapper; // Instance of our Ogre initialization wrapper

	Ogre::Root	*root; //! The ogre root
	Ogre::RenderWindow	*renderWindow;						
	Ogre::SceneManager	*sceneManager;						
	Ogre::SceneNode	 *rootSceneNode;				
	Ogre::Viewport	*viewport;	
	shared_ptr<Ogre::Timer>	timer;

	OIS::InputManager *inputManager;				
	std::shared_ptr<OIS::Mouse>	mouse;						
	std::shared_ptr<OIS::Keyboard> keyboard;					
																	
	void	CreateLights();													
	void	CreateEnviroment();
	   
public:
																
	OgreApplication(void);
	~OgreApplication(void);
   
	void RunOgreApplication();
	bool Initialize();
	void CleanUp();
											
	std::shared_ptr<Ogre::SceneNode> CreateCamera(Ogre::String  name);	
	Ogre::SceneNode* CreateEntityFromMesh(std::string mesh, std::string name);
 
	void SetEntityColour(Ogre::String      materialName,
		                 Ogre::String      resourceGroupName,
		                 Ogre::ColourValue diffuse,
		                 Ogre::ColourValue specular,
		                 Ogre::Real        shininess);
	

	void CreateParticles();                                            
	void CreateAnimations();											
	void CreateIOS(); 										
	void Run(bool updateOption=false, bool synchroOption=true); //The main run loop
	void DestroyNode(Ogre::SceneNode *nodeName);

	shared_ptr<Ogre::Timer> CreateTimer();
	void CreateScene();
	std::shared_ptr<Ogre::SceneNode> CreateEntity();

    const OgreWrapper&	GetOgreWrapper(){ return ogreWrapper; }
	Ogre::SceneManager* GetSceneManager(){return sceneManager ;}
									
	shared_ptr<OIS::Keyboard> GetKeyboard(){return keyboard;}
    shared_ptr<OIS::Mouse> GetMouse(){return mouse;}
	shared_ptr<Ogre::Timer> GetTimer(){return timer;}

};

#endif