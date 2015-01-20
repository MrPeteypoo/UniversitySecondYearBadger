#include "OgreApplication.h"
#include <Framework/Camera.h>
#include <Simulation/Simulation.h>
#include <iostream>

static double STEP_LENGTH = 0.03;

using namespace std;

OgreApplication::OgreApplication(void):root(nullptr), renderWindow( nullptr), sceneManager ( nullptr), rootSceneNode ( nullptr), viewport( nullptr),
	inputManager( nullptr), mouse( nullptr), keyboard( nullptr), timer( nullptr)
{
}


OgreApplication::~OgreApplication(void)
{
	this->CleanUp();

	root = nullptr;
	renderWindow = nullptr;
	sceneManager = nullptr;
	rootSceneNode = nullptr;
	viewport = nullptr;
    inputManager = nullptr;
	timer = nullptr;

}

// Setup and run ogre based application
void OgreApplication::RunOgreApplication()
{

	Ogre::String cameraName = "MainCamera";
	auto cameraNode = this->CreateCamera(cameraName);
	cameraNode->setPosition(Ogre::Vector3(-80.0f, 60.0f, -80.0f));
	static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"))->lookAt(0.0f, 0.0f, 0.0f);
	shared_ptr<Ogre::MovableObject> temp;
	temp.reset(cameraNode->getAttachedObject("MainCamera"));
	std::shared_ptr<Camera> camera = make_shared<Camera>(cameraNode, temp);

	//Create the simulation world
	auto simulation = make_shared<Simulation>();
	simulation->initialiseScene(this);
	
	this->CreateIOS();
	auto keyboard = this->GetKeyboard();
	auto mouse = this->GetMouse();
	auto timer = this->CreateTimer();
	timer->reset();

    simulation->setKeyboard (keyboard);

	unsigned long lastTime = timer->getMilliseconds();
	unsigned long deltaTime = 0;
	float deltaTime_s = 0.0f;

	this->GetOgreWrapper().GetRoot()->clearEventTimes();

	double timeToUpdate = 0.0;
    unsigned int index = 0;
    float elapsedTime = 0.0f;


	bool animationState = false;
	//Game loop
	while (!this->GetOgreWrapper().GetWindow()->isClosed())
	{

		//Evaluate the current time and the time elapsed since last frame 
		//Prepare the next iteration. 	
		unsigned long currentTime = timer->getMilliseconds();
		deltaTime = currentTime - lastTime;
		if(deltaTime == 0)  continue;

		lastTime = currentTime;
		deltaTime_s = 0.001f * float(deltaTime);

		keyboard->capture();
		mouse->capture();
		camera->Update(keyboard.get(), mouse.get());

		float coeff = 200.0f * deltaTime_s;
		Ogre::Vector3 translation(Ogre::Vector3::ZERO);

		if (keyboard->isKeyDown(OIS::KC_ESCAPE))
		{
			break;
		}
		else if (keyboard->isKeyDown(OIS::KC_V)){
			auto camera = static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"));
			camera->setPolygonMode(Ogre::PM_WIREFRAME);
		}
		else if (keyboard->isKeyDown(OIS::KC_B))
		{
			auto camera = static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"));
			camera->setPolygonMode(Ogre::PM_POINTS);
		}
		else if (keyboard->isKeyDown(OIS::KC_N))
		{
			auto camera = static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"));
			camera->setPolygonMode(Ogre::PM_SOLID);
		}
		if (keyboard->isKeyDown(OIS::KC_R))
		{
			simulation->reset();
		}
		
		
		const OIS::MouseState& mouseState = mouse->getMouseState();

		
		elapsedTime += deltaTime;
		
		if (elapsedTime > 100)
		{
			// do something

			elapsedTime = 0;
		}

		timeToUpdate += deltaTime_s;
		unsigned int numOfUpdates = 0;

		while (timeToUpdate > STEP_LENGTH && numOfUpdates < 60)
		{
			timeToUpdate -= STEP_LENGTH;
			simulation->update (deltaTime_s);
			//simulation->Update(STEP_LENGTH);
			numOfUpdates++;
		}
		
		// Update window false
		bool updateWin = false;
		bool verticalSynchro = true;
		this->Run(updateWin, verticalSynchro);

		Ogre::WindowEventUtilities::messagePump();
	}

	this->CleanUp();

	return;
}

void OgreApplication::CleanUp()
{
  renderWindow->removeAllViewports();
  sceneManager ->destroyAllLights();
  sceneManager ->destroyAllManualObjects();
  sceneManager ->destroyAllEntities();
	
}

//\ brief Setup and initialize
bool OgreApplication::Initialize()
{
	
	if(!ogreWrapper.Initialize())
	{
		std::cout << __FILE__ << " (" << __LINE__ << ") - " << "Failed to initialise the application" << std::endl; 
		return false;
	}
	
	// Create  Root, RenderWindow, SceneManager, and SceneNode 
	root = ogreWrapper.GetRoot();
	renderWindow = ogreWrapper.GetWindow();
	sceneManager  = root->createSceneManager(Ogre::ST_GENERIC, "MySceneManager");
	rootSceneNode  = sceneManager ->getRootSceneNode();

	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	return true;
}

//Create a camera and attach it to a scene node
shared_ptr<Ogre::SceneNode>  OgreApplication::CreateCamera(Ogre::String  name)
{

	// Create a camera using SceneManager. 
	Ogre::Camera* camera = sceneManager->createCamera(name);

	// Create a child scene node of the RootSceneNode,
	// Attach the camera to the child SceneNode. It will be easier then to move it in the scene.
	shared_ptr<Ogre::SceneNode> cameraNode;
	cameraNode.reset(rootSceneNode ->createChildSceneNode(name.append("Node")));
	cameraNode->attachObject(camera);

	// Setup viewport parameters 
	// Add viewport to the window with the camera 
	float viewportWidth = 1.0f;
	float viewportHeight = 1.0f;
	float viewportLeft	= (1.0f - viewportWidth) * 0.5f;
	float viewportTop = (1.0f - viewportHeight) * 0.5f;
	unsigned short viewportZOrder = 100;
	
	viewport = renderWindow->addViewport(camera, viewportZOrder, viewportLeft, viewportTop, viewportWidth, viewportHeight);

	// Set the viewport to draw the scene automatically
	// whenever the window is updated.
	viewport->setAutoUpdated(true);

	viewport->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.3f));

	// Setup the visual ratio of the camera. To make it looks real, same as the viewport.
	float ratio = static_cast<float>(viewport->getActualWidth()) / static_cast<float>(viewport->getActualHeight());
	camera->setAspectRatio(ratio);
	camera->setNearClipDistance(0.2f);
	camera->setFarClipDistance(1000.0f);
	
	// Set the window to be active
	renderWindow->setActive(true);

	// Update the content of the window not automatically.
	renderWindow->setAutoUpdated(false);

	root->clearEventTimes();

	return cameraNode;
}

//Create light in the scene
void OgreApplication::CreateLights()
{
	
	// create a spot light
	Ogre::String tempName = "SpotLight";
    Ogre::Light* spotLight = sceneManager ->createLight(tempName);
	spotLight->setType(Ogre::Light::LT_POINT);

	//Set light colour 
	spotLight->setDiffuseColour(0.8f, 0.8f, 0.8f); 
	spotLight->setSpecularColour(0.5f, 0.5f, 0.5f);
	//set the spot light corn beam width
	spotLight->setSpotlightRange(Ogre::Degree(20), Ogre::Degree(50));

	//Create a childSceneNode of the RootSceneNode
	//Attach the light to it
	Ogre::SceneNode* lightNode = rootSceneNode ->createChildSceneNode(tempName.append("Node"));
	lightNode->attachObject(spotLight);
	lightNode->setDirection(1.0f, 1.0f, 0.0f);
	lightNode->setPosition(Ogre::Vector3(80.0, 80.0, 0.0));

	Ogre::ColourValue ambientColour(0.2f, 0.2f, 0.2f, 1.0f);
	sceneManager ->setAmbientLight(ambientColour);

}
/*
shared_ptr<Ogre::SceneNode> OgreApplication::CreateEntityFromMesh( std::string mesh, std::string name)
{
	// add the second object
	Ogre::String meshName = mesh.substr(0, mesh.find(".mesh")); 

	Ogre::Entity* meshEntity =  nullptr;
	try 
	{
		meshEntity = sceneManager->getEntity(meshName);
		meshEntity = meshEntity->clone(meshName + "-" + name);
	}
	catch(Ogre::ItemIdentityException ex) 
	{
		try
		{
			meshEntity = sceneManager->createEntity(meshName, mesh);
		}
		catch (Ogre::Exception ex)
		{
			return  nullptr;
		}
	}

	// create a scene node to the entity scene node for our character
	shared_ptr<Ogre::SceneNode>  node;
    node.reset( rootSceneNode->createChildSceneNode(name));
	
	//attach the mesh
	try
	{
		node->attachObject(meshEntity);
		node->setPosition(0.0f, 0.0f, 0.0f);
	}
	catch (Ogre::InvalidParametersException ex)
	{
		return  nullptr;
	}

	return node;
}
*/

Ogre::SceneNode* OgreApplication::CreateEntityFromMesh(std::string mesh, std::string name)
{
	// add the second object
	Ogre::String meshName = mesh.substr(0, mesh.find(".mesh"));

	Ogre::Entity* meshEntity = nullptr;
	try
	{
		meshEntity = sceneManager->getEntity(meshName);
		meshEntity = meshEntity->clone(meshName + "-" + name);
	}
	catch (Ogre::ItemIdentityException ex)
	{
		try
		{
			meshEntity = sceneManager->createEntity(meshName, mesh);
		}
		catch (Ogre::Exception ex)
		{
			return  nullptr;
		}
	}

	// create a scene node to the entity scene node for our character
	Ogre::SceneNode *const node = rootSceneNode->createChildSceneNode(name);

	//attach the mesh
	try
	{
		node->attachObject(meshEntity);
		node->setPosition(0.0f, 0.0f, 0.0f);
	}
	catch (Ogre::InvalidParametersException ex)
	{
		return  nullptr;
	}

	return node;
}
//Create an entity with a predefined mesh name and add a new node to our scene 
shared_ptr<Ogre::SceneNode> OgreApplication::CreateEntity()
{

	//	After the previous setup for the scene, we can now load the object and lit it with AmbientLight
	// load a character mesh
	Ogre::String  tempName = "Head"; 
	Ogre::Entity* headEnt = sceneManager ->createEntity(tempName, "ogrehead.mesh");
    headEnt->setCastShadows(true);

	// create a scene node to hold all the mesh entities that we will compute physics later.
	shared_ptr<Ogre::SceneNode> node;
	node.reset(rootSceneNode ->createChildSceneNode(tempName.append("Node")));
	
	// create a scene node to the entity scene node for our character
	//attach the mesh
	node->attachObject(headEnt);
	
	return node;
}

//Set surface material property for an entity
void OgreApplication::SetEntityColour(Ogre::String  materialName, 
		                              Ogre::String  resourceGroupName,
									  Ogre::ColourValue diffuse,
									  Ogre::ColourValue specular,
									  Ogre::Real shininess)
{
	
	Ogre::MaterialPtr lMaterial = Ogre::MaterialManager::getSingleton().create(materialName, resourceGroupName);
	Ogre::Technique* lFirstTechnique = lMaterial->getTechnique(0);
	Ogre::Pass* lFirstPass = lFirstTechnique->getPass(0);

	// Lighting is allowed on this pass.
	lFirstPass->setLightingEnabled(true);

	// diffuse color is the traditional color of the object surface.
	lFirstPass->setDiffuse(diffuse);

	// Generating specular highlight on the surface
	lFirstPass->setSpecular(specular);

	lFirstPass->setShininess(shininess);
}

//Create scene 
void OgreApplication::CreateScene()
{
	CreateLights();	
	CreateEnviroment();
}

// Create a ogre world environment with a predefined geometry and a texture
void OgreApplication::CreateEnviroment()
{
	
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 400, 400, 200, 200, true, 5, 5, 5, Ogre::Vector3::UNIT_Z);

	Ogre::String tempName = "Ground";
	Ogre::Entity* ground = this->sceneManager->createEntity("Ground", "ground");
	ground->setCastShadows(false);

	Ogre::SceneNode* groundNode = this->sceneManager->getRootSceneNode()->createChildSceneNode(tempName.append("Node"));
	groundNode->attachObject(ground);
	groundNode->setPosition(0.0f, 0.0f, 0.0f);

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("GroundTexture", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Technique* firstTechnique = material->getTechnique(0);
	Ogre::Pass* firstPass = firstTechnique->getPass(0);

	Ogre::TextureUnitState* textureUnit = firstPass->createTextureUnitState();
	textureUnit->setTextureName("checker.png", Ogre::TEX_TYPE_2D);  // BeachStones.jpg
	textureUnit->setTextureCoordSet(0);

	const Ogre::String& materialName = "GroundTexture";
	ground->setMaterialName(materialName);
	

}


void OgreApplication::Run(bool updateOption, bool synchroOption)
{
	renderWindow->update(updateOption);
    renderWindow->swapBuffers();
    root->renderOneFrame();
}

void OgreApplication::CreateIOS()
{

	//The input/output system needs an handle to a window as input, in a string format. 
	//1/ Create a size_t object that we need to use to retrieve a window handle from our render window 
	size_t windowHandle = 0;
	renderWindow->getCustomAttribute("WINDOW", &windowHandle);

	//2/convert it into a string
	std::string windowsHandleAsString="";
	{
		std::ostringstream windowHndStr;
		windowHndStr << windowHandle;
		windowsHandleAsString = windowHndStr.str();
	}

	// 3/ we translate it into a format accepted by OIS (the input library). 

	OIS::ParamList lSpecialParameters;
	lSpecialParameters.insert(std::make_pair(std::string("WINDOW"), windowsHandleAsString));

	//4/ we create the input/output system itself.
	inputManager = OIS::InputManager::createInputSystem( lSpecialParameters );
	
	//the events can be stored or not in a buffer 
	bool lBufferedKeys = false;
	bool lBufferedMouse = false;

	//creation of the keyboard-representing object 
	keyboard.reset(static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, lBufferedKeys)));
	//creation of the mouse-representing object 
	mouse.reset(static_cast<OIS::Mouse*>(inputManager->createInputObject( OIS::OISMouse, lBufferedMouse)));
	//then must tell the mouse how much it is allowed to move. 
	unsigned int width, height, colourDepth;
    int top, left;
	renderWindow->getMetrics(width, height, colourDepth, left, top);
	const OIS::MouseState &infoSouris = mouse->getMouseState();
	infoSouris.width = width; 
	infoSouris.height = height; 
	
}

shared_ptr<Ogre::Timer> OgreApplication::CreateTimer()
{
	timer = make_shared<Ogre::Timer>(*root->getTimer());

	return timer;
}

void OgreApplication::DestroyNode(Ogre::SceneNode *node)
{
	if (node != nullptr)
	{
		node->detachAllObjects();
		node->removeAndDestroyAllChildren();
		rootSceneNode->removeAndDestroyChild(node->getName());
	}
}