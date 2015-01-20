#include "OgreWrapper.h"
#include <OgreConfigFile.h>

OgreWrapper::OgreWrapper():
	root(NULL), 
	window(NULL)
{
}

OgreWrapper::~OgreWrapper()
{
	UnLoadResources();
}

bool OgreWrapper::Initialize()
{
	Ogre::String configFileName = "resources.cfg";
	Ogre::String pluginsFileName = "plugins.cfg";
	Ogre::String logFileName = "Ogre.log";

	// STEP 1/  create the Ogre::Root object.
	root = new Ogre::Root(configFileName, pluginsFileName, logFileName);

	// STEP 2/  Load plugins & resources. At least a 'RenderSystem' plugin to drive opengl or directx.
	std::vector<Ogre::String> plugins;	

	plugins.push_back("RenderSystem_GL");
	plugins.push_back("RenderSystem_Direct3D9");
	plugins.push_back("Plugin_ParticleFX");

	for(std::vector<Ogre::String>::iterator it = plugins.begin(); it != plugins.end(); ++it)
	{
		Ogre::String& pluginName = (*it);

#ifdef _DEBUG
		pluginName.append("_d");
#endif
		root->loadPlugin(pluginName);
	}
		
	// Load resource paths from config file
#ifdef _DEBUG
	configFileName = "resources_d.cfg";
#endif

	Ogre::ConfigFile cf;
	cf.load(configFileName);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator secit = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (secit.hasMoreElements())
	{
		secName = secit.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = secit.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
		
		
	// STEP 3/ get the list of render drivers available	
	const Ogre::RenderSystemList& renderSystemList = root->getAvailableRenderers();
	if( renderSystemList.size() == 0 )
	{
		return false;
	}
	// Just select the first render driver (should be OpenGL)
	Ogre::RenderSystem *renderSystem = renderSystemList.at(0);
	root->showConfigDialog();
	
	// STEP 4/ initialise the Root, only when a rendersystem has been selected.
	root->initialise(false, "", "");
	
    // Setup some window parameters
		
	Ogre::String windowTitle = "Animation and Simulation Tutorial Framework";
	unsigned int width  = 1280;
	unsigned int height = 720;
	bool fullscreen = false; 
	
	Ogre::NameValuePairList windowParameters;
	windowParameters["full screen antialiasing"] = "0"; 
	// vertical synchronisation will prevent some image-tearing.
	windowParameters["vsync"] = "true";
	window = root->createRenderWindow(windowTitle, width, height, fullscreen, &windowParameters);
		
	return true;
}

//\brief Unload resources
void OgreWrapper::UnLoadResources()
{
	// unload the map so we don't interfere with subsequent samples
	Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
	rgm.unloadResourceGroup(rgm.getWorldResourceGroupName());
}

