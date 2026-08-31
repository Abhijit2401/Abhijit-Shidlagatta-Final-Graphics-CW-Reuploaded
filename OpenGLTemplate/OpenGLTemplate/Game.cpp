/*
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting,
 different camera controls, different shaders, etc.

 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk)

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

 version 6.1a 13/02/2022 - Sorted out Release mode and a few small compiler warnings
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

*/


// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file, heavily modified with custom logic.
// Major logic written/edited by me:
// - HDR Bloom FBOs & Multi-pass Post-Processing Pipeline
// - PCF Shadow Mapping Pipeline
// - Procedural Object Spawning (Trees, Grass, Obstacles, Boosts)
// - Particle Systems (Dirt, Leaves, Speed lines) & Alpha Blending
// - Mathematical Frustum Culling
// - Water Rendering Pass (with depth-masking)
// - Decoupled Game Loop (Physics, Collisions, Audio states)
// ---------------------------------------------------------
// ---------------------------------------------------------
#include "game.h"
#include "HighResolutionTimer.h"
#include "GameWindow.h"
#include <fstream>
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "Pyramid.h"
#include "Track.h"
#include "CCatmullRom.h"
#include <cstdlib>
#include "CCylinder.h"

// Map settings preset
const glm::vec3 SUN_DIRECTION = glm::normalize(glm::vec3(-1.0f, 0.15f, -0.4f));
const glm::vec3 SUN_COLOR = glm::vec3(1.0f, 0.55f, 0.5f);
const float     WATER_HEIGHT = 28.5f;
const float     BEACH_TREE_LEVEL = 50.0f;
const int       TREE_ATTEMPTS = 7000;
const int       GRASS_ATTEMPTS = 1500;
const float     TRACK_CLEARANCE = 35.0f;
const float     TRACK_MAX_LENGTH = 10000.0f;
const int       BARREL_COUNT = 12;
const float     BARREL_SPACING = 600.0f;
const float     BARREL_HITBOX = 4.0f;
const int       LOG_COUNT = 10;
const float     LOG_START_DIST = 400.0f;
const float     LOG_SPACING = 1000.0f;
const float     LOG_RADIUS = 0.5f;
const float     LOG_LENGTH = 1.4f;
const int       BOOST_COUNT = 10;
const float     BOOST_SPACING = 500.0f;
const float     BOOST_HITBOX = 12.0f;
const float     CONE_SPACING = 20.0f;

Game::Game() : m_gameWindow(GameWindow::GetInstance())
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pShadowProgram = NULL;
	m_pWaterProgram = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_pCarMesh = NULL;
	m_pTreeMesh1 = NULL;
	m_pTreeMesh2 = NULL;
	m_pSphere = NULL;
	m_pPyramid = NULL;
	m_pCylinder = NULL;
	m_pPyramidProgram = NULL;
	m_pBrightFilterProgram = NULL;
	m_pBloomCompositeProgram = NULL;
	m_pBlurProgram = NULL;
	m_pInstancedProgram = NULL;
	m_pCatmullRom = NULL;
	m_pTrack = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;

	m_t = 0.0f;
	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_cameraMode = 2;
	m_steerAngle = 0.0f;
	m_currentDistance = 0.0f;
	m_cameraSpeed = 0.0f;
	m_lateralPosition = 0.0f;
	m_collisionCooldown = 0.0f;
}

Game::~Game()
{
	// Deletes all the dynamically allocated objects to prevent memory leaks
	if (m_pCamera) { delete m_pCamera; m_pCamera = NULL; }
	if (m_pSkybox) { delete m_pSkybox; m_pSkybox = NULL; }
	if (m_pPlanarTerrain) { delete m_pPlanarTerrain; m_pPlanarTerrain = NULL; }
	if (m_pFtFont) { delete m_pFtFont; m_pFtFont = NULL; }
	if (m_pShadowProgram) { delete m_pShadowProgram; m_pShadowProgram = NULL; }
	if (m_pWaterProgram) { delete m_pWaterProgram; m_pWaterProgram = NULL; }
	if (m_pBarrelMesh) { delete m_pBarrelMesh; m_pBarrelMesh = NULL; }
	if (m_pHorseMesh) { delete m_pHorseMesh; m_pHorseMesh = NULL; }
	if (m_pCarMesh) { delete m_pCarMesh; m_pCarMesh = NULL; }
	if (m_pTreeMesh1) { delete m_pTreeMesh1; m_pTreeMesh1 = NULL; }
	if (m_pTreeMesh2) { delete m_pTreeMesh2; m_pTreeMesh2 = NULL; }
	if (m_pSphere) { delete m_pSphere; m_pSphere = NULL; }
	if (m_pPyramid) { delete m_pPyramid; m_pPyramid = NULL; }
	if (m_pCylinder) { delete m_pCylinder; m_pCylinder = NULL; }
	if (m_pPyramidProgram) { delete m_pPyramidProgram; m_pPyramidProgram = NULL; }
	if (m_pBrightFilterProgram) { delete m_pBrightFilterProgram; m_pBrightFilterProgram = NULL; }
	if (m_pBloomCompositeProgram) { delete m_pBloomCompositeProgram; m_pBloomCompositeProgram = NULL; }
	if (m_pBlurProgram) { delete m_pBlurProgram; m_pBlurProgram = NULL; }
	if (m_pInstancedProgram) { delete m_pInstancedProgram; m_pInstancedProgram = NULL; }
	if (m_pCatmullRom) { delete m_pCatmullRom; m_pCatmullRom = NULL; }
	if (m_pTrack) { delete m_pTrack; m_pTrack = NULL; }
	if (m_pAudio) { delete m_pAudio; m_pAudio = NULL; }

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
		delete m_pShaderPrograms;
		m_pShaderPrograms = NULL;
	}

	if (m_pHighResolutionTimer) { delete m_pHighResolutionTimer; m_pHighResolutionTimer = NULL; }
}

void Game::Initialise()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram*>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pPyramid = new CPyramid;
	m_pCatmullRom = new CCatmullRom;
	m_pGrassMesh = new COpenAssetImportMesh;
	// sets up the track and its offset curves for collision detection and rendering
	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateTrack("resources\\textures\\", "grass_floor.png", "dirtpile01.jpg");

	m_pHorseMesh = new COpenAssetImportMesh;
	m_pCarMesh = new COpenAssetImportMesh;
	m_pTreeMesh1 = new COpenAssetImportMesh;
	m_pTreeMesh2 = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;

	// Pyramid shader set up
	CShader pyrVertShader;
	pyrVertShader.LoadShader("resources\\shaders\\pyramidShader.vert.txt", GL_VERTEX_SHADER);
	CShader pyrFragShader;
	pyrFragShader.LoadShader("resources\\shaders\\pyramidShader.frag.txt", GL_FRAGMENT_SHADER);
	m_pPyramidProgram = new CShaderProgram;
	m_pPyramidProgram->CreateProgram();
	m_pPyramidProgram->AddShaderToProgram(&pyrVertShader);
	m_pPyramidProgram->AddShaderToProgram(&pyrFragShader);
	m_pPyramidProgram->LinkProgram();

	// Setup shadow mapping
	CShader shadowVert, shadowFrag;
	shadowVert.LoadShader("resources\\shaders\\shadow.vert", GL_VERTEX_SHADER);
	shadowFrag.LoadShader("resources\\shaders\\shadow.frag", GL_FRAGMENT_SHADER);
	m_pShadowProgram = new CShaderProgram;
	m_pShadowProgram->CreateProgram();
	m_pShadowProgram->AddShaderToProgram(&shadowVert);
	m_pShadowProgram->AddShaderToProgram(&shadowFrag);
	m_pShadowProgram->LinkProgram();

	// Binds the shadow map texture to a framebuffer object (FBO) for rendering the depth information from the light's perspective
	glGenFramebuffers(1, &m_shadowMapFBO);
	glGenTextures(1, &m_shadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Sets up the framebuffers and textures for post processing effects like bloom and motion blur
	RECT clientRect;
	GetClientRect(m_gameWindow.Hwnd(), &clientRect);
	int fbWidth = clientRect.right - clientRect.left;
	int fbHeight = clientRect.bottom - clientRect.top;

	// FBO 1 Main scene
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_renderTexture);
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, fbWidth, fbHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTexture, 0);

	glGenRenderbuffers(1, &m_depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbWidth, fbHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

	// FBO 2 Bright Filter
	glGenFramebuffers(1, &m_fbo2);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo2);
	glGenTextures(1, &m_renderTexture2);
	glBindTexture(GL_TEXTURE_2D, m_renderTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, fbWidth, fbHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTexture2, 0);

	// FBO 3 Bloom composite
	glGenFramebuffers(1, &m_fbo3);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo3);
	glGenTextures(1, &m_renderTexture3);
	glBindTexture(GL_TEXTURE_2D, m_renderTexture3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, fbWidth, fbHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTexture3, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Sets up the water shader for rendering the water surface with reflections and refractions
	CShader waterVert, waterFrag;
	waterVert.LoadShader("resources\\shaders\\waterShader.vert", GL_VERTEX_SHADER);
	waterFrag.LoadShader("resources\\shaders\\waterShader.frag", GL_FRAGMENT_SHADER);
	m_pWaterProgram = new CShaderProgram;
	m_pWaterProgram->CreateProgram();
	m_pWaterProgram->AddShaderToProgram(&waterVert);
	m_pWaterProgram->AddShaderToProgram(&waterFrag);
	m_pWaterProgram->LinkProgram();

	// Sets up the grass shader for rendering grass patches with instancing
	CShader grassVert, grassFrag;
	grassVert.LoadShader("resources\\shaders\\grassShader.vert", GL_VERTEX_SHADER);
	grassFrag.LoadShader("resources\\shaders\\grassShader.frag", GL_FRAGMENT_SHADER);
	m_pGrassProgram = new CShaderProgram;
	m_pGrassProgram->CreateProgram();
	m_pGrassProgram->AddShaderToProgram(&grassVert);
	m_pGrassProgram->AddShaderToProgram(&grassFrag);
	m_pGrassProgram->LinkProgram();

	m_pGrassMesh->Load("resources\\models\\Grass\\grass.obj");

	// Sets up the water plane geometry for rendering the water surface
	glGenVertexArrays(1, &m_vaoWater);
	glBindVertexArray(m_vaoWater);

	std::vector<float> waterVerts;
	int wRes = 250;
	float wSize = 7000.0f;
	float wStart = -wSize / 2.0f;
	float wStep = wSize / wRes;
	for (int z = 0; z < wRes; z++) {
		for (int x = 0; x < wRes; x++) {
			float x0 = wStart + x * wStep; float z0 = wStart + z * wStep;
			float x1 = wStart + (x + 1) * wStep; float z1 = wStart + (z + 1) * wStep;
			float pts[] = {
				x0, 0.0f, z0, 0.f, 0.f, 0.f, 1.f, 0.f,
				x0, 0.0f, z1, 0.f, 1.f, 0.f, 1.f, 0.f,
				x1, 0.0f, z0, 1.f, 0.f, 0.f, 1.f, 0.f,
				x1, 0.0f, z0, 1.f, 0.f, 0.f, 1.f, 0.f,
				x0, 0.0f, z1, 0.f, 1.f, 0.f, 1.f, 0.f,
				x1, 0.0f, z1, 1.f, 1.f, 0.f, 1.f, 0.f
			};
			waterVerts.insert(waterVerts.end(), pts, pts + 48);
		}
	}
	m_waterVertexCount = (int)(waterVerts.size() / 8);

	glGenBuffers(1, &m_vboWater);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboWater);
	glBufferData(GL_ARRAY_BUFFER, waterVerts.size() * sizeof(float), waterVerts.data(), GL_STATIC_DRAW);
	GLsizei strideW = 8 * sizeof(float);
	glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideW, 0);
	glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideW, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2); glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, strideW, (void*)(5 * sizeof(float)));

	// Sets up the camera's projection matrices based on the window dimensions
	RECT dimensions = m_gameWindow.GetDimensions();
	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	m_pCamera->SetOrthographicProjectionMatrix(width, height);
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float)width / (float)height, 0.5f, 5000.0f);
	// Sets up the shaders for rendering the main scene and text, including vertex and fragment shaders for each
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");

	for (int i = 0; i < (int)sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int)sShaderFileNames[i].size() - 4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\" + sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Sets up the vertex array object (VAO) for rendering speed lines which are used to create a visual effect of when the car is moving at high speeds
	glGenVertexArrays(1, &m_vaoSpeedLine);
	glBindVertexArray(m_vaoSpeedLine);
	float lineVerts[] = {
		0.0f, 0.0f, 0.0f,     0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,    1.0f, 1.0f,  0.0f, 1.0f, 0.0f
	};
	GLuint vboLine;
	glGenBuffers(1, &vboLine);
	glBindBuffer(GL_ARRAY_BUFFER, vboLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVerts), lineVerts, GL_STATIC_DRAW);
	GLsizei stride = 8 * sizeof(float);
	glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2); glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));

	for (int i = 0; i < 15; i++) {
		SpeedParticle p;
		p.offset = glm::vec3(
			((rand() % 10000) / 10.0f),
			((rand() % 500) - 100) / 10.0f,
			((rand() % 1000) - 500) / 10.0f
		);
		p.speedMultiplier = 1.0f + ((rand() % 50) / 100.0f);
		m_particles.push_back(p);
	}
	// Sets up the main shader program for rendering the 3D scene including the vertex and fragment shaders for lighting, texturing and other visual effects
	CShaderProgram* pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);
	// Sets up the font shader program for rendering text on the screen using FreeType library
	CShaderProgram* pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Sets up the reflection shader for rendering reflective surfaces like water
	CShader refVertShader;
	refVertShader.LoadShader("resources\\shaders\\reflect.vert", GL_VERTEX_SHADER);
	CShader refFragShader;
	refFragShader.LoadShader("resources\\shaders\\reflect.frag", GL_FRAGMENT_SHADER);
	CShaderProgram* pReflectProgram = new CShaderProgram;
	pReflectProgram->CreateProgram();
	pReflectProgram->AddShaderToProgram(&refVertShader);
	pReflectProgram->AddShaderToProgram(&refFragShader);
	pReflectProgram->LinkProgram();
	m_pShaderPrograms->push_back(pReflectProgram);

	// Sets up the motion blur shader for rendering motion blur effect when the car is moving at high speeds
	CShader blurVertShader;
	blurVertShader.LoadShader("resources\\shaders\\motionBlur.vert", GL_VERTEX_SHADER);
	CShader blurFragShader;
	blurFragShader.LoadShader("resources\\shaders\\motionBlur.frag", GL_FRAGMENT_SHADER);
	m_pBlurProgram = new CShaderProgram;
	m_pBlurProgram->CreateProgram();
	m_pBlurProgram->AddShaderToProgram(&blurVertShader);
	m_pBlurProgram->AddShaderToProgram(&blurFragShader);
	m_pBlurProgram->LinkProgram();

	// Sets up the shaders for bloom effect which enhances the brightness of certain areas in the scene to create a glowing effect
	CShader brightFrag, compositeFrag;
	brightFrag.LoadShader("resources\\shaders\\brightFilter.frag", GL_FRAGMENT_SHADER);
	compositeFrag.LoadShader("resources\\shaders\\bloomComposite.frag", GL_FRAGMENT_SHADER);
	// Creates a shader program for the bright filter which extracts the bright areas of the scene for bloom effect
	m_pBrightFilterProgram = new CShaderProgram;
	m_pBrightFilterProgram->CreateProgram();
	m_pBrightFilterProgram->AddShaderToProgram(&blurVertShader);
	m_pBrightFilterProgram->AddShaderToProgram(&brightFrag);
	m_pBrightFilterProgram->LinkProgram();
	// Creates a shader program for the bloom composite which combines the original scene with the bright areas to create the final bloom effect
	m_pBloomCompositeProgram = new CShaderProgram;
	m_pBloomCompositeProgram->CreateProgram();
	m_pBloomCompositeProgram->AddShaderToProgram(&blurVertShader);
	m_pBloomCompositeProgram->AddShaderToProgram(&compositeFrag);
	m_pBloomCompositeProgram->LinkProgram();
	// Sets up the instanced shader for rendering multiple instances of the same object efficiently
	m_pSkybox->Create(2500.0f);
	m_pPlanarTerrain->Create("resources\\textures\\", "grass_floor.png", "stone.png", "snow.png", "sand.png", 3000.0f, 3000.0f, 60.0f);
	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Loads the 3D models for the car, barrel, horse(not used)) and trees from the specified file paths
	m_pCarMesh->Load("resources\\models\\Car\\car.obj");
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");
	m_pTreeMesh1->Load("resources\\models\\Trees\\tree.obj");
	m_pTreeMesh2->Load("resources\\models\\Trees\\tree_snow.obj");

	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);
	glEnable(GL_CULL_FACE);

	m_pPyramid->Create("resources\\textures\\", "dirtpile01.jpg", 20.0f);
	m_pCylinder = new CCylinder;
	m_pCylinder->Create("resources\\textures\\", "dirtpile01.jpg", 5.0f, 20.0f, 30);

	// Sets up the audio system and loads various sound effects and music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");
	m_pAudio->LoadBoostSound("resources\\Audio\\boost.wav");
	m_pAudio->LoadAmbientSound("resources\\Audio\\wind.wav");
	m_pAudio->LoadOceanSound("resources\\Audio\\ocean.wav");
	m_pAudio->LoadEngineSound("resources\\Audio\\engine_loop.wav");
	m_pAudio->LoadMusicStream("resources\\Audio\\music.mp3");

	m_pAudio->PlayAmbientSound();
	m_pAudio->PlayEngineSound();
	m_pAudio->PlayMusicStream();

	// Prepares the game for rendering by setting up the track points, spawning trees and grass patches procedurally based on terrain height and distance from the track
	std::vector<glm::vec3> trackPoints;
	for (float d = 0; d < TRACK_MAX_LENGTH; d += 50.0f) {
		glm::vec3 p;
		if (m_pCatmullRom->Sample(d, p)) {
			trackPoints.push_back(p);
		}
	}

	// Spawn logic
	for (int i = 0; i < TREE_ATTEMPTS; i++) {
		float randomX = ((rand() % 3000) - 1500.0f);
		float randomZ = ((rand() % 3000) - 1500.0f);
		float terrainY = GetTerrainHeight(randomX, randomZ);

		if (terrainY > BEACH_TREE_LEVEL && terrainY < 320.0f) {
			float hL = GetTerrainHeight(randomX - 1.0f, randomZ);
			float hR = GetTerrainHeight(randomX + 1.0f, randomZ);
			float hD = GetTerrainHeight(randomX, randomZ - 1.0f);
			float hU = GetTerrainHeight(randomX, randomZ + 1.0f);
			glm::vec3 surfaceNormal = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));

			// Only spawns trees on relatively flat terrain to avoid placing them on steep slopes
			if (surfaceNormal.y > 0.8f) {
				bool safeFromTrack = true;
				for (const auto& tp : trackPoints) {
					float distSq = (tp.x - randomX) * (tp.x - randomX) + (tp.z - randomZ) * (tp.z - randomZ);
					if (distSq < (TRACK_CLEARANCE * TRACK_CLEARANCE)) {
						safeFromTrack = false;
						break;
					}
				}
				// Only spawns trees if they are a safe distance away from the track to prevent them from obstructing the camera's view
				if (safeFromTrack) {
					float spawnY = terrainY;
					spawnY -= 7.0f;

					glm::mat4 tModel = glm::mat4(1.0f);
					tModel = glm::translate(tModel, glm::vec3(randomX, spawnY, randomZ));

					float tScale = ((rand() % 500) / 100.0f) + 8.0f;
					tModel = glm::scale(tModel, glm::vec3(tScale));
					tModel = glm::rotate(tModel, (float)(rand() % 360), glm::vec3(0, 1, 0));
					TreeInstance tree;
					tree.transform = tModel;
					tree.variant = 0;
					m_treeInstances.push_back(tree);
				}
			}
		}
	}

	// Spawn grass patch models along the track with some randomness in their placement and orientation
	for (float d = 0; d < TRACK_MAX_LENGTH; d += 2.0f) {
		glm::vec3 centerPos, nextPos;

		if (m_pCatmullRom->Sample(d, centerPos) && m_pCatmullRom->Sample(d + 1.0f, nextPos)) {
			glm::vec3 T = glm::normalize(nextPos - centerPos);
			glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));

			int patchesPerStep = 4 + (rand() % 4);
			for (int p = 0; p < patchesPerStep; p++) {
				float distFromCenter = 35.0f + ((rand() % 1450) / 10.0f);
				int side = (rand() % 2 == 0) ? 1 : -1;

				glm::vec3 grassPos = centerPos + (N * (distFromCenter * side));
				// Adds some random jitter to the grass position to make it look more natural and less uniform
				float jitterForward = ((rand() % 200) / 10.0f) - 10.0f;
				grassPos += T * jitterForward;
				grassPos.y = GetTerrainHeight(grassPos.x, grassPos.z);
				// Only spawns grass patches on  flat terrain and within a certain height range to avoid placing them on steep slopes or underwater
				if (grassPos.y > BEACH_TREE_LEVEL && grassPos.y < 250.0f) {
					float hL = GetTerrainHeight(grassPos.x - 1.0f, grassPos.z);
					float hR = GetTerrainHeight(grassPos.x + 1.0f, grassPos.z);
					float hD = GetTerrainHeight(grassPos.x, grassPos.z - 1.0f);
					float hU = GetTerrainHeight(grassPos.x, grassPos.z + 1.0f);
					glm::vec3 surfaceNormal = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));
					//
					if (surfaceNormal.y > 0.85f) {
						bool safeFromTrack = true;
						for (const auto& tp : trackPoints) {
							float distSq = (tp.x - grassPos.x) * (tp.x - grassPos.x) + (tp.z - grassPos.z) * (tp.z - grassPos.z);
							if (distSq < (30.0f * 30.0f)) {
								safeFromTrack = false;
								break;
							}
						}

						if (safeFromTrack) {
							glm::mat4 gModel = glm::mat4(1.0f);
							gModel = glm::translate(gModel, grassPos);

							float gScale = ((rand() % 500) / 100.0f) + 5.0f;
							gModel = glm::scale(gModel, glm::vec3(gScale));
							gModel = glm::rotate(gModel, (float)(rand() % 360), glm::vec3(0, 1, 0));
							// Adds the grass patch transform to the list of grass transforms for rendering
							m_grassTransforms.push_back(gModel);
						}
					}
				}
			}
		}
	}

	// Places barrels along the track at regular intervals with some randomness in their lateral position to create obstacles
	for (int i = 0; i < BARREL_COUNT; i++) {
		float distance = 500.0f + (i * BARREL_SPACING);
		glm::vec3 centerPos, centerUp, nextPos, dummyUp;
		// Samples the track at the current distance and a point slightly ahead to determine the orientation of the barrel
		if (m_pCatmullRom->Sample(distance, centerPos, centerUp) && m_pCatmullRom->Sample(distance + 1.0f, nextPos, dummyUp)) {
			glm::vec3 T = glm::normalize(nextPos - centerPos);
			glm::vec3 N = glm::normalize(glm::cross(T, centerUp));
			glm::vec3 B = glm::normalize(glm::cross(N, T));

			float randomOffset = ((rand() % 300) / 10.0f) - 15.0f;
			glm::vec3 barrelPos = centerPos + (N * randomOffset);

			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, barrelPos + (B * 1.5f));
			modelMatrix *= glm::mat4(glm::mat3(T, B, N));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f));

			m_barrelTransforms.push_back(modelMatrix);
		}
	}

	// Places boost orbs along the track at regular intervals with some randomness in their distance to create powerups for the player
	for (int i = 0; i < BOOST_COUNT; i++) {
		float distance = (i * BOOST_SPACING) + (float)(rand() % 200);
		glm::vec3 p, up;
		if (m_pCatmullRom->Sample(distance, p, up)) {
			// Offsets the boost orb position slightly above the track
			p = p + (up * 4.0f);
			BoostOrb orb;
			orb.originalPosition = p;
			orb.active = true;
			orb.cooldownTimer = 0.0f;
			// Adds the boost orb to the list of boost orbs for rendering and collision detection
			m_boostOrbs.push_back(orb);
		}
	}

	// Places cones along the start of hte track to create a visual effect of a starting line for the player
	glm::vec3 cPos, cUp, nPos, dummyUp;
	if (m_pCatmullRom->Sample(0.0f, cPos, cUp) && m_pCatmullRom->Sample(1.0f, nPos, dummyUp)) {
		glm::vec3 T = glm::normalize(nPos - cPos);
		glm::vec3 N = glm::normalize(glm::cross(T, cUp));
		glm::vec3 B = glm::normalize(glm::cross(N, T));

		for (int i = -2; i <= 2; i++) {
			glm::vec3 conePos = cPos + (N * (8.5f * i));
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, conePos + (B * 1.5f));
			model *= glm::mat4(glm::mat3(T, B, N));
			model = glm::scale(model, glm::vec3(0.15f, 0.45f, 0.15f));
			m_coneTransforms.push_back(model);
		}
	}

	// Places log obstacles along the track at regular intervals with some randomness in their lateral position to create another set of obstacles for the player
	for (int i = 0; i < LOG_COUNT; i++) {
		float dist = LOG_START_DIST + (i * LOG_SPACING);
		glm::vec3 centerPos, centerUp, nextPos, dummyUp;
		// Samples the track at the current distance and a point slightly ahead to determine the orientation of the log obstacle
		if (m_pCatmullRom->Sample(dist, centerPos, centerUp) && m_pCatmullRom->Sample(dist + 1.0f, nextPos, dummyUp)) {
			glm::vec3 T = glm::normalize(nextPos - centerPos);
			glm::vec3 N = glm::normalize(glm::cross(T, centerUp));
			glm::vec3 B = glm::normalize(glm::cross(N, T));
			int side = (rand() % 2 == 0) ? 1 : -1;
			glm::vec3 logPos = centerPos + (N * (17.0f * side));
			LogObstacle log;
			log.position = logPos;
			log.active = true;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, logPos + (B * 1.0f));
			model *= glm::mat4(glm::mat3(T, B, N));
			model = glm::rotate(model, 1.5708f, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(LOG_RADIUS, LOG_LENGTH, LOG_RADIUS));
			log.transform = model;
			// Adds the log obstacle to the list of log obstacles for rendering and collision detection
			m_logs.push_back(log);
		}
	}
}

WPARAM Game::Execute()
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if (!m_gameWindow.Hdc()) return 1;

	Initialise();
	m_pHighResolutionTimer->Start();

	MSG msg;
	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (m_appActive) {
			GameLoop();
		}
		else Sleep(200);
	}

	m_gameWindow.Deinit();
	return(msg.wParam);
}

void Game::GameLoop()
{
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
}

void Game::Update()
{
	m_t += (float)m_dt * 0.001f;

	// Updates the collision cooldown timer to prevent immediate repeated collisions after a crash so that it doesnt bug
	if (m_collisionCooldown > 0.0f) {
		m_collisionCooldown -= (float)m_dt;
	}

	UpdatePhysicsAndInput();
	UpdateCollisions();
	UpdateParticles();
	UpdateAudioState();
}

void Game::UpdatePhysicsAndInput()
{
	float normalMaxSpeed = 0.08f;
	float maxReverse = -0.05f;

	// Disables camera speed updates when the player is in free cam mode (mode 4) to allow for free movement without affecting the car and its movement
	if (m_cameraMode != 4) {
		// Updates the camera speed based on user input for forward and backward movement by applying acceleration and deceleration
		if (GetKeyState('W') & 0x80) {
			if (m_cameraSpeed < normalMaxSpeed) {
				// Accelerates the camera speed when the 'W' key is pressed
				m_cameraSpeed += 0.00003f * (float)m_dt;
				if (m_cameraSpeed > normalMaxSpeed) m_cameraSpeed = normalMaxSpeed;
			}
		}
		if (GetKeyState('S') & 0x80) {
			m_cameraSpeed -= 0.00005f * (float)m_dt;
		}
	}

	// Slowly reduces the camera speed when neither forward or backward keys are pressed to act like friction and deceleration
	if (!(GetKeyState('W') & 0x80) && !(GetKeyState('S') & 0x80)) {
		if (m_cameraSpeed > 0.0f) {
			m_cameraSpeed -= 0.00005f * (float)m_dt;
			if (m_cameraSpeed < 0.0f) m_cameraSpeed = 0.0f;
		}
		else if (m_cameraSpeed < 0.0f) {
			m_cameraSpeed += 0.00005f * (float)m_dt;
			if (m_cameraSpeed > 0.0f) m_cameraSpeed = 0.0f;
		}
	}

	// Ensures that the camera speed does not exceed the maximum forward speed or go below the maximum reverse speed
	if (m_cameraSpeed > normalMaxSpeed) {
		// Applies deceleration when the camera speed exceeds the maximum forward speed
		m_cameraSpeed -= 0.00008f * (float)m_dt;
	}
	if (m_cameraSpeed < maxReverse) m_cameraSpeed = maxReverse;

	// Updates the steering angle based on user input for left and right movement by applying a turn speed that is proportional to the passed time
	float turnSpeed = 0.001f * (float)m_dt;
	bool isSteering = false;

	// sisables steering when the player is in free cam (mode 4)
	if (m_cameraMode != 4 && abs(m_cameraSpeed) > 0.001f) {
		float actualTurnSpeed = (m_cameraSpeed >= 0.0f) ? turnSpeed : -turnSpeed;
		if (GetKeyState('A') & 0x80) { m_steerAngle += actualTurnSpeed; isSteering = true; }
		if (GetKeyState('D') & 0x80) { m_steerAngle -= actualTurnSpeed; isSteering = true; }
	}

	// Gradually reduces the steering angle
	if (!isSteering) m_steerAngle *= 0.90f;
	if (m_steerAngle > 0.8f) m_steerAngle = 0.8f;
	if (m_steerAngle < -0.8f) m_steerAngle = -0.8f;

	float forwardSpeed = m_cameraSpeed * cos(m_steerAngle);
	float lateralSpeed = m_cameraSpeed * sin(m_steerAngle);

	m_currentDistance += forwardSpeed * (float)m_dt;
	if (m_currentDistance < 0.0f) {
		m_currentDistance = 0.0f;
		m_cameraSpeed = 0.0f;
	}

	// Clamps the lateral position of the car to prevent it from going too far off the track and reduces the steering angle when it hits the boundaries
	m_lateralPosition -= lateralSpeed * (float)m_dt * 3.0f;
	if (m_lateralPosition < -15.0f) { m_lateralPosition = -15.0f; m_steerAngle *= 0.5f; }
	if (m_lateralPosition > 15.0f) { m_lateralPosition = 15.0f;  m_steerAngle *= 0.5f; }
	// Locks the car to the track if it goes too far off the track
	glm::vec3 centerPosition, centerUp, nextPos, dummyUp;
	m_pCatmullRom->Sample(m_currentDistance, centerPosition, centerUp);
	m_pCatmullRom->Sample(m_currentDistance + 1.0f, nextPos, dummyUp);

	glm::vec3 T = glm::normalize(nextPos - centerPosition);
	glm::vec3 N = glm::normalize(glm::cross(T, centerUp));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	m_playerPosition = centerPosition + (N * m_lateralPosition);
	glm::mat4 baseOrientation = glm::mat4(glm::mat3(T, B, N));
	m_playerOrientation = glm::rotate(baseOrientation, m_steerAngle, glm::vec3(0.0f, 1.0f, 0.0f));

	// Camera shake
	glm::vec3 shakeOffset = glm::vec3(0.0f);
	if (m_collisionCooldown > 0.0f) {
		float intensity = (m_collisionCooldown / 1000.0f) * 25.0f;

		shakeOffset = glm::vec3(
			((rand() % 100) / 100.0f - 0.5f) * intensity,
			((rand() % 100) / 100.0f - 0.5f) * intensity,
			((rand() % 100) / 100.0f - 0.5f) * intensity
		);
	}

	// Updates the camera position and orientation based on currrent camera mode
	glm::vec3 cameraPosition, viewTarget, upVector;
	if (m_cameraMode == 1) {
		cameraPosition = m_playerPosition + (B * 4.0f) + (T * 2.0f) + shakeOffset;
		viewTarget = cameraPosition + (T * 10.0f) + (shakeOffset * 0.5f);
		upVector = B;
		m_pCamera->Set(cameraPosition, viewTarget, upVector);
	}
	else if (m_cameraMode == 2) {
		glm::vec3 idealPos = m_playerPosition + (-T * 15.0f) + (glm::vec3(0.0f, 1.0f, 0.0f) * 7.0f) + shakeOffset;
		glm::vec3 idealView = m_playerPosition + (T * 10.0f) + (shakeOffset * 0.5f);

		float smoothSpeed = 4.0f * (float)m_dt * 0.001f;
		if (smoothSpeed > 1.0f) smoothSpeed = 1.0f;

		cameraPosition = m_pCamera->GetPosition() + (idealPos - m_pCamera->GetPosition()) * smoothSpeed;
		viewTarget = m_pCamera->GetView() + (idealView - m_pCamera->GetView()) * smoothSpeed;
		upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		m_pCamera->Set(cameraPosition, viewTarget, upVector);
	}
	else if (m_cameraMode == 3) {
		cameraPosition = m_playerPosition + (B * 80.0f);
		viewTarget = m_playerPosition;
		upVector = T;
		m_pCamera->Set(cameraPosition, viewTarget, upVector);
	}
	else if (m_cameraMode == 4) {
		m_pCamera->Update(m_dt * 5.0);
	}
}

void Game::UpdateCollisions()
{
	// Checks for collisions with barrels and logs only if the collision cooldown timer has expired to prevent immediate repeated collisions
	if (m_collisionCooldown <= 0.0f) {
		for (int i = 0; i < m_barrelTransforms.size(); i++) {
			if (glm::distance(m_playerPosition, glm::vec3(m_barrelTransforms[i][3])) < BARREL_HITBOX) {
				if (m_cameraSpeed >= 0.0f) { m_currentDistance -= 2.0f; m_cameraSpeed = -0.05f; }
				else { m_currentDistance += 2.0f; m_cameraSpeed = 0.05f; }
				m_lateralPosition -= 1.0f;
				m_pAudio->PlayEventSound();
				m_collisionCooldown = 1000.0f;
				break;
			}
		}

		for (auto& log : m_logs) {
			if (log.active && glm::distance(m_playerPosition, log.position) < 7.5f) {
				log.active = false;
				if (m_cameraSpeed >= 0.0f) { m_currentDistance -= 2.0f; m_cameraSpeed = -0.05f; }
				else { m_currentDistance += 2.0f; m_cameraSpeed = 0.05f; }
				m_lateralPosition -= 1.0f;
				m_pAudio->PlayEventSound();
				m_collisionCooldown = 1000.0f;
				break;
			}
		}
	}

	// Boosts can be picked up by the player when they are within a certain distance of the boost orb's position and they will respawn after a cooldown
	for (auto& orb : m_boostOrbs) {
		if (!orb.active) {
			orb.cooldownTimer -= (float)m_dt;
			if (orb.cooldownTimer <= 0.0f) orb.active = true;
		}
		else if (glm::distance(m_playerPosition, orb.originalPosition) < BOOST_HITBOX) {
			m_cameraSpeed += (m_cameraSpeed >= 0.0f) ? 0.1f : -0.1f;
			m_pAudio->PlayBoostSound();
			orb.active = false;
			orb.cooldownTimer = 5000.0f; // Respawns in 5 seconds
		}
	}
}

void Game::UpdateParticles()
{
	// Updates the position of the background particles based on the camera speed and resets their position when they go past teh screen to create a continuous particle effect
	for (auto& p : m_particles) {
		p.offset.x -= m_cameraSpeed * p.speedMultiplier * 90.0f * (float)m_dt;
		if (p.offset.x < -40.0f) {
			p.offset.x += 2200.0f;
			p.offset.y = ((rand() % 500) - 100) / 10.0f;
			p.offset.z = ((rand() % 1000) - 500) / 10.0f;
		}
	}

	// Generates dirt particles behind the car when it is moving to create a dust effect and to animate the car's movement
	if (abs(m_cameraSpeed) > 0.01f) {
		glm::vec3 cPos, cUp, nPos, dUp;
		m_pCatmullRom->Sample(m_currentDistance, cPos, cUp);
		m_pCatmullRom->Sample(m_currentDistance + 1.0f, nPos, dUp);
		glm::vec3 T = glm::normalize(nPos - cPos);
		glm::vec3 N = glm::normalize(glm::cross(T, cUp));

		int numParticles = (int)(abs(m_cameraSpeed) * 15.0f);
		if (numParticles < 1 && (rand() % 100 < 30)) numParticles = 1;

		for (int p = 0; p < numParticles; p++) {
			DirtParticle dirt;
			int side = (rand() % 2 == 0) ? 1 : -1;
			glm::vec3 rearTirePos = m_playerPosition - (T * 3.5f) + (N * (2.2f * side));

			dirt.position = rearTirePos + glm::vec3(((rand() % 100) - 50) / 100.0f, 0.2f, ((rand() % 100) - 50) / 100.0f);
			dirt.velocity = (-T * (abs(m_cameraSpeed) * 0.4f)) + glm::vec3(
				((rand() % 100) - 50) / 1000.0f,
				0.03f + ((rand() % 50) / 1000.0f),
				((rand() % 100) - 50) / 1000.0f
			);
			dirt.life = 1.0f;
			dirt.scale = 0.03f + ((rand() % 5) / 100.0f);
			dirt.rotation = (float)(rand() % 360);
			m_dirtParticles.push_back(dirt);
		}
	}

	for (int i = 0; i < m_dirtParticles.size(); i++) {
		m_dirtParticles[i].velocity.y -= 0.002f * (float)m_dt;
		m_dirtParticles[i].position += m_dirtParticles[i].velocity * (float)m_dt;
		m_dirtParticles[i].rotation += 0.8f * (float)m_dt;
		m_dirtParticles[i].life -= 0.002f * (float)m_dt;

		float groundY = GetTerrainHeight(m_dirtParticles[i].position.x, m_dirtParticles[i].position.z);
		if (m_dirtParticles[i].life <= 0.0f || m_dirtParticles[i].position.y <= groundY) {
			m_dirtParticles.erase(m_dirtParticles.begin() + i);
			i--;
		}
	}

	// Generates leaf particles that fall from the trees to create a dynamic environment
	if (rand() % 100 < 17) {
		LeafParticle leaf;
		leaf.position = m_playerPosition + glm::vec3(
			((rand() % 800) - 400) / 10.0f,
			15.0f + ((rand() % 200) / 10.0f),
			((rand() % 800) - 400) / 10.0f
		);
		leaf.velocity = glm::vec3(
			((rand() % 100) - 50) / 4000.0f,
			-0.01f - ((rand() % 10) / 1000.0f),
			((rand() % 100) - 50) / 4000.0f
		);
		leaf.rotation = (float)(rand() % 360);
		leaf.rotSpeed = 0.0f;
		leaf.life = 1.0f;
		leaf.scale = 0.02f + ((rand() % 5) / 100.0f);
		m_leaves.push_back(leaf);
	}

	for (int i = 0; i < m_leaves.size(); i++) {
		m_leaves[i].position += m_leaves[i].velocity * (float)m_dt;
		m_leaves[i].position.x += sin(m_t * 1.2f + m_leaves[i].life * 10.0f) * 0.005f * (float)m_dt;
		m_leaves[i].position.z += cos(m_t * 0.8f + m_leaves[i].life * 10.0f) * 0.005f * (float)m_dt;
		m_leaves[i].life -= 0.00005f * (float)m_dt;

		float groundY = GetTerrainHeight(m_leaves[i].position.x, m_leaves[i].position.z);
		if (m_leaves[i].life <= 0.0f || m_leaves[i].position.y <= groundY + 0.5f) {
			m_leaves.erase(m_leaves.begin() + i);
			i--;
		}
	}
}

void Game::UpdateAudioState()
{
	static bool isOceanPlaying = false;

	// Switches between ambient and ocean sounds based on the player's height to create a more immersive audio experience
	if (m_playerPosition.y < 100.0f) {
		if (!isOceanPlaying) {
			m_pAudio->StopAmbientSound();
			m_pAudio->PlayOceanSound();
			isOceanPlaying = true;
		}
	}
	else {
		if (isOceanPlaying) {
			m_pAudio->StopOceanSound();
			m_pAudio->PlayAmbientSound();
			isOceanPlaying = false;
		}
	}

	m_pAudio->UpdateEnginePitch(m_cameraSpeed);
	m_pAudio->Update();
}

bool Game::IsInFrustum(const glm::vec3& objPos, const glm::vec3& cullPos, const glm::vec3& cullDir) {
	glm::vec3 toObj = objPos - cullPos;
	float dist = glm::length(toObj);
	if (dist > 1500.0f) return false;
	return glm::dot(cullDir, toObj / dist) > -0.2f;
}

void Game::RenderShadowMap(const glm::mat4& lightSpaceMatrix, const glm::mat4& carModel)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
	glViewport(0, 0, 4096, 4096);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); // Culls front faces

	m_pShadowProgram->UseProgram();
	m_pShadowProgram->SetUniform("lightSpaceMatrix", lightSpaceMatrix);

	m_pShadowProgram->SetUniform("modelMatrix", glm::mat4(1.0f));
	m_pPlanarTerrain->Render();
	m_pCatmullRom->RenderTrack();

	for (int i = 0; i < m_treeInstances.size(); i++) {
		if (glm::distance(m_playerPosition, glm::vec3(m_treeInstances[i].transform[3])) < 450.0f) {
			m_pShadowProgram->SetUniform("modelMatrix", m_treeInstances[i].transform);
			if (m_treeInstances[i].variant == 0) m_pTreeMesh1->Render();
			else m_pTreeMesh2->Render();
		}
	}
	for (int i = 0; i < m_barrelTransforms.size(); i++) {
		if (glm::distance(m_playerPosition, glm::vec3(m_barrelTransforms[i][3])) < 450.0f) {
			m_pShadowProgram->SetUniform("modelMatrix", m_barrelTransforms[i]);
			m_pBarrelMesh->Render();
		}
	}
	for (auto& log : m_logs) {
		if (log.active && glm::distance(m_playerPosition, log.position) < 450.0f) {
			m_pShadowProgram->SetUniform("modelMatrix", log.transform);
			m_pCylinder->Render();
		}
	}

	m_pShadowProgram->SetUniform("modelMatrix", carModel);
	m_pCarMesh->Render();

	glCullFace(GL_BACK);
}

void Game::RenderWaterPass(const glm::mat4& viewMatrix, const glm::vec3& sunEyeDir, int cubeMapTextureUnit)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // Disables depth writing for water transparency
	glDisable(GL_CULL_FACE);

	m_pWaterProgram->UseProgram();
	m_pWaterProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	m_pWaterProgram->SetUniform("viewMatrix", viewMatrix);
	m_pWaterProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	m_pWaterProgram->SetUniform("t", m_t);
	m_pWaterProgram->SetUniform("sunDirection", sunEyeDir);
	m_pWaterProgram->SetUniform("sunColor", SUN_COLOR);

	glutil::MatrixStack mvs;
	mvs.SetIdentity();
	mvs.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	mvs.Translate(glm::vec3(0.0f, WATER_HEIGHT, 0.0f));

	m_pWaterProgram->SetUniform("matrices.modelViewMatrix", mvs.Top());
	m_pWaterProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(mvs.Top()));

	glBindVertexArray(m_vaoWater);
	glDrawArrays(GL_TRIANGLES, 0, m_waterVertexCount);
	glBindVertexArray(0);
	// Resets state after rendering water
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void Game::RenderParticlesPass(CShaderProgram* pMainProgram, glutil::MatrixStack& modelViewMatrixStack, const glm::mat4& viewMatrix, const glm::mat4& carModelBase)
{
	if (m_dirtParticles.size() > 0) {
		pMainProgram->UseProgram();
		pMainProgram->SetUniform("bUseTexture", false);
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.25f, 0.15f, 0.08f));
		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.35f, 0.20f, 0.12f));
		glDisable(GL_CULL_FACE);

		for (const auto& dirt : m_dirtParticles) {
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(dirt.position);
			modelViewMatrixStack.Rotate(glm::vec3(1.0f, 1.0f, 1.0f), dirt.rotation);
			modelViewMatrixStack.Scale(dirt.scale * dirt.life); // Scale the dirt particle based on its life to make it shrink as it fades away

			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pPyramid->Render();
			modelViewMatrixStack.Pop();
		}
		glEnable(GL_CULL_FACE);
	}

	if (m_leaves.size() > 0) {
		pMainProgram->UseProgram();
		pMainProgram->SetUniform("bUseTexture", false);
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f, 0.0f, 0.0f));
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.05f, 0.25f, 0.05f));
		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.15f, 0.65f, 0.15f));
		glDisable(GL_CULL_FACE);

		for (const auto& leaf : m_leaves) {
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(leaf.position);
			modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), leaf.rotation);

			modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), sin(m_t * 2.0f + leaf.life * 20.0f) * 0.3f);
			modelViewMatrixStack.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), cos(m_t * 1.7f + leaf.life * 20.0f) * 0.3f);

			modelViewMatrixStack.Scale(glm::vec3(leaf.scale, 0.05f, leaf.scale * 1.5f));

			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pPyramid->Render();
			modelViewMatrixStack.Pop();
		}
		glEnable(GL_CULL_FACE);
	}

	if (abs(m_cameraSpeed) > 0.08f) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		pMainProgram->UseProgram();
		pMainProgram->SetUniform("bUseTexture", false);
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f, 0.0f, 0.0f));
		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f, 0.5f, 0.5f)); // Increases the ambient color to make the particles appear brighter and more visible
		pMainProgram->SetUniform("objectAlpha", 0.6f);

		for (auto& p : m_particles) {
			modelViewMatrixStack.Push();
			glm::mat4 particleModel = carModelBase;
			particleModel = glm::translate(particleModel, p.offset);
			particleModel = glm::scale(particleModel, glm::vec3(5.0f + (abs(m_cameraSpeed) * 300.0f), 0.02f, 0.02f));

			glm::mat4 mV = viewMatrix * particleModel;
			pMainProgram->SetUniform("matrices.modelViewMatrix", mV);
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(mV));
			m_pPyramid->Render();
			modelViewMatrixStack.Pop();
		}
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	//	Renders the boost orbs that the player can collect to increase their speed and adds a glowing effect to them
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("objectAlpha", 1.0f);
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));
}

void Game::RenderPostProcessingPass()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	// Extract FBO 2 for bright areas (BLOOM PASS)
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo2);
	glClear(GL_COLOR_BUFFER_BIT);
	m_pBrightFilterProgram->UseProgram();
	m_pBrightFilterProgram->SetUniform("sceneTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	glBindSampler(0, 0);
	RenderFullScreenQuad();

	// Composite bloom with original scene (BLOOM COMPOSITE PASS)
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo3);
	glClear(GL_COLOR_BUFFER_BIT);
	m_pBloomCompositeProgram->UseProgram();
	m_pBloomCompositeProgram->SetUniform("baseTexture", 0);
	m_pBloomCompositeProgram->SetUniform("brightTexture", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	glBindSampler(0, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_renderTexture2);
	glBindSampler(1, 0);

	RenderFullScreenQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_pBlurProgram->UseProgram();
	m_pBlurProgram->SetUniform("blurAmount", abs(m_cameraSpeed) * 0.05f);
	m_pBlurProgram->SetUniform("sampler0", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_renderTexture3);
	glBindSampler(0, 0);

	RenderFullScreenQuad();
}

void Game::Render()
{
	glm::vec3 sunWorldDir = SUN_DIRECTION;
	float shadowOrtho = 400.0f;
	glm::mat4 lightProjection = glm::ortho(-shadowOrtho, shadowOrtho, -shadowOrtho, shadowOrtho, 1.0f, 3000.0f);
	glm::vec3 lightPos = m_playerPosition + (sunWorldDir * 1000.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, m_playerPosition, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glm::mat4 carModel = glm::mat4(1.0f);
	carModel = glm::translate(carModel, m_playerPosition);
	carModel *= m_playerOrientation;
	glm::mat4 carModelBase = carModel;
	carModel = glm::translate(carModel, glm::vec3(0.0f, 1.7f, 0.0f));
	carModel = glm::rotate(carModel, 9.4f, glm::vec3(0.0f, 1.0f, 0.0f));
	carModel = glm::scale(carModel, glm::vec3(8.0f));

	RenderShadowMap(lightSpaceMatrix, carModel);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	RECT dimensions = m_gameWindow.GetDimensions();
	glViewport(0, 0, dimensions.right - dimensions.left, dimensions.bottom - dimensions.top);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();

	CShaderProgram* pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("lightSpaceMatrix", lightSpaceMatrix);
	pMainProgram->SetUniform("inverseViewMatrix", glm::inverse(viewMatrix));

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	pMainProgram->SetUniform("shadowMap", 12);
	glActiveTexture(GL_TEXTURE0);

	int cubeMapTextureUnit = 10;
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	glm::vec3 carForward = glm::vec3(m_playerOrientation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	glm::vec4 headlightWorldPos = glm::vec4(m_playerPosition + (carForward * 2.0f), 1.0f);
	headlightWorldPos.y += 1.0f;
	pMainProgram->SetUniform("light1.position", viewMatrix * headlightWorldPos);
	pMainProgram->SetUniform("light1.direction", glm::normalize(glm::mat3(viewMatrix) * carForward));
	pMainProgram->SetUniform("light1.cutoff", 15.0f);
	pMainProgram->SetUniform("light1.exponent", 50.0f);
	pMainProgram->SetUniform("light1.La", glm::vec3(0.15f, 0.1f, 0.2f));
	pMainProgram->SetUniform("light1.Ld", SUN_COLOR * 2.5f);
	pMainProgram->SetUniform("light1.Ls", SUN_COLOR * 2.5f);
	pMainProgram->SetUniform("sunDirection", glm::mat3(viewMatrix) * sunWorldDir);
	pMainProgram->SetUniform("sunColor", SUN_COLOR);

	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("renderSkybox", true);
	modelViewMatrixStack.Translate(m_pCamera->GetPosition());
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render(cubeMapTextureUnit);
	pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.05f));
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pMainProgram->SetUniform("bTerrainMode", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("sampler1", 1);
	pMainProgram->SetUniform("sampler2", 2);
	pMainProgram->SetUniform("sampler3", 3);
	m_pPlanarTerrain->Render();
	pMainProgram->SetUniform("bTerrainMode", false);
	modelViewMatrixStack.Pop();

	RenderWaterPass(viewMatrix, glm::mat3(viewMatrix) * sunWorldDir, cubeMapTextureUnit);

	pMainProgram->UseProgram();
	glActiveTexture(GL_TEXTURE0);
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	glm::vec3 cullPos = m_playerPosition;
	glm::vec3 cullDir = glm::normalize(carForward);

	pMainProgram->SetUniform("bIsTree", true);
	pMainProgram->SetUniform("t", m_t);
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));
	for (int i = 0; i < m_treeInstances.size(); i++) {
		if (!IsInFrustum(glm::vec3(m_treeInstances[i].transform[3]), cullPos, cullDir)) continue;
		pMainProgram->SetUniform("treePhase", m_treeInstances[i].transform[3][0] + m_treeInstances[i].transform[3][2]);
		glm::mat4 modelView = modelViewMatrixStack.Top() * m_treeInstances[i].transform;
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelView);
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelView));
		if (m_treeInstances[i].variant == 0) m_pTreeMesh1->Render();
		else m_pTreeMesh2->Render();
	}
	pMainProgram->SetUniform("bIsTree", false);

	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.8f));
	for (int i = 0; i < m_barrelTransforms.size(); i++) {
		if (!IsInFrustum(glm::vec3(m_barrelTransforms[i][3]), cullPos, cullDir)) continue;
		glm::mat4 modelView = modelViewMatrixStack.Top() * m_barrelTransforms[i];
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelView);
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	}

	pMainProgram->SetUniform("bUseTexture", false);
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f, 0.4f, 0.0f));
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f, 0.4f, 0.0f));
	for (int i = 0; i < m_coneTransforms.size(); i++) {
		if (!IsInFrustum(glm::vec3(m_coneTransforms[i][3]), cullPos, cullDir)) continue;
		glm::mat4 modelView = modelViewMatrixStack.Top() * m_coneTransforms[i];
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelView);
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelView));
		m_pPyramid->Render();
	}
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));

	CShaderProgram* pReflectProgram = (*m_pShaderPrograms)[2];
	pReflectProgram->UseProgram();
	pReflectProgram->SetUniform("projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	pReflectProgram->SetUniform("viewMatrix", viewMatrix);
	pReflectProgram->SetUniform("cameraPos", m_pCamera->GetPosition());
	pReflectProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	pReflectProgram->SetUniform("sampler0", 0);
	pReflectProgram->SetUniform("modelMatrix", carModel);
	m_pCarMesh->Render();

	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", false);
	pMainProgram->SetUniform("t", m_t);

	float pulse = (sin(m_t * 5.0f) * 0.5f) + 1.5f;
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.3f, 0.0f, 0.3f) * pulse);
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.3f, 0.0f, 0.3f) * pulse);
	for (const auto& orb : m_boostOrbs) {
		if (!orb.active || !IsInFrustum(orb.originalPosition, cullPos, cullDir)) continue;
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(orb.originalPosition);
		modelViewMatrixStack.Scale(1.8f + (sin(m_t * 3.0f) * 0.4f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), (float)m_elapsedTime * 0.005f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSphere->Render();
		modelViewMatrixStack.Pop();
	}
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));
	// 
	static GLuint cubeVAO = 0;
	static GLuint cubeVBO = 0;
	// Uses the cube VAO and VBO for rendering the boost orbs and dirt particles
	if (cubeVAO == 0) {
		float cv[] = {
			-1.f,-1.f, 1.f, 0.f,0.f, 0.f,0.f,1.f,   1.f,-1.f, 1.f, 1.f,0.f, 0.f,0.f,1.f,   1.f, 1.f, 1.f, 1.f,1.f, 0.f,0.f,1.f,
			 1.f, 1.f, 1.f, 1.f,1.f, 0.f,0.f,1.f,  -1.f, 1.f, 1.f, 0.f,1.f, 0.f,0.f,1.f,  -1.f,-1.f, 1.f, 0.f,0.f, 0.f,0.f,1.f,
			-1.f,-1.f,-1.f, 0.f,0.f, 0.f,0.f,-1.f, -1.f, 1.f,-1.f, 0.f,1.f, 0.f,0.f,-1.f,  1.f, 1.f,-1.f, 1.f,1.f, 0.f,0.f,-1.f,
			 1.f, 1.f,-1.f, 1.f,1.f, 0.f,0.f,-1.f,  1.f,-1.f,-1.f, 1.f,0.f, 0.f,0.f,-1.f, -1.f,-1.f,-1.f, 0.f,0.f, 0.f,0.f,-1.f,
			-1.f, 1.f, 1.f, 1.f,1.f,-1.f,0.f,0.f,  -1.f, 1.f,-1.f, 0.f,1.f,-1.f,0.f,0.f,  -1.f,-1.f,-1.f, 0.f,0.f,-1.f,0.f,0.f,
			-1.f,-1.f,-1.f, 0.f,0.f,-1.f,0.f,0.f,  -1.f,-1.f, 1.f, 1.f,0.f,-1.f,0.f,0.f,  -1.f, 1.f, 1.f, 1.f,1.f,-1.f,0.f,0.f,
			 1.f, 1.f, 1.f, 0.f,1.f, 1.f,0.f,0.f,   1.f,-1.f, 1.f, 0.f,0.f, 1.f,0.f,0.f,   1.f,-1.f,-1.f, 1.f,0.f, 1.f,0.f,0.f,
			 1.f,-1.f,-1.f, 1.f,0.f, 1.f,0.f,0.f,   1.f, 1.f,-1.f, 1.f,1.f, 1.f,0.f,0.f,   1.f, 1.f, 1.f, 0.f,1.f, 1.f,0.f,0.f,
			-1.f, 1.f,-1.f, 0.f,1.f, 0.f,1.f,0.f,  -1.f, 1.f, 1.f, 0.f,0.f, 0.f,1.f,0.f,   1.f, 1.f, 1.f, 1.f,0.f, 0.f,1.f,0.f,
			 1.f, 1.f, 1.f, 1.f,0.f, 0.f,1.f,0.f,   1.f, 1.f,-1.f, 1.f,1.f, 0.f,1.f,0.f,  -1.f, 1.f,-1.f, 0.f,1.f, 0.f,1.f,0.f,
			-1.f,-1.f,-1.f, 0.f,0.f, 0.f,-1.f,0.f,  1.f,-1.f,-1.f, 1.f,0.f, 0.f,-1.f,0.f,  1.f,-1.f, 1.f, 1.f,1.f, 0.f,-1.f,0.f,
			 1.f,-1.f, 1.f, 1.f,1.f, 0.f,-1.f,0.f, -1.f,-1.f, 1.f, 0.f,1.f, 0.f,-1.f,0.f, -1.f,-1.f,-1.f, 0.f,0.f, 0.f,-1.f,0.f
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cv), cv, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2); glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glBindVertexArray(0);
	}
	glm::vec3 sPos, sUp, sNext, sDUp;
	if (m_pCatmullRom->Sample(0.0f, sPos, sUp) && m_pCatmullRom->Sample(1.0f, sNext, sDUp)) {
		glm::vec3 sT = glm::normalize(sNext - sPos);
		glm::vec3 sN = glm::normalize(glm::cross(sT, sUp));
		glm::vec3 sB = glm::normalize(glm::cross(sN, sT));

		glm::mat4 sModel = glm::mat4(1.0f);
		sModel = glm::translate(sModel, sPos + (sB * 4.0f) - (sT * 30.0f));
		sModel *= glm::mat4(glm::mat3(sT, sB, sN));
		sModel = glm::scale(sModel, glm::vec3(1.0f, 4.0f, 18.0f));

		if (IsInFrustum(glm::vec3(sModel[3]), cullPos, cullDir)) {
			glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glDepthMask(GL_FALSE); glDisable(GL_CULL_FACE);
			pMainProgram->SetUniform("bUseTexture", false);
			pMainProgram->SetUniform("objectAlpha", 0.4f);
			pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f, 1.0f, 0.8f));
			pMainProgram->SetUniform("material1.Ma", glm::vec3(0.0f, 1.0f, 0.8f));

			glm::mat4 sMV = modelViewMatrixStack.Top() * sModel;
			pMainProgram->SetUniform("matrices.modelViewMatrix", sMV);
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(sMV));

			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			pMainProgram->SetUniform("objectAlpha", 1.0f);
			pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));
			pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));
			glEnable(GL_CULL_FACE); glDepthMask(GL_TRUE); glDisable(GL_BLEND);
		}
	}

	pMainProgram->SetUniform("bUseTexture", true);

	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pMainProgram->SetUniform("bMultiTexture", true);
	pMainProgram->SetUniform("sampler1", 1);
	m_pCatmullRom->RenderCentreline();
	m_pCatmullRom->RenderOffsetCurves();
	m_pCatmullRom->RenderTrack();
	pMainProgram->SetUniform("bMultiTexture", false);
	modelViewMatrixStack.Pop();

	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.6f, 0.35f, 0.15f));
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.6f, 0.35f, 0.15f));
	for (auto& log : m_logs) {
		if (log.active && IsInFrustum(log.position, cullPos, cullDir)) {
			glm::mat4 modelView = modelViewMatrixStack.Top() * log.transform;
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelView);
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelView));
			m_pCylinder->Render();
		}
	}
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));

	m_pGrassProgram->UseProgram();
	m_pGrassProgram->SetUniform("projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	m_pGrassProgram->SetUniform("viewMatrix", viewMatrix);
	m_pGrassProgram->SetUniform("t", m_t);
	m_pGrassProgram->SetUniform("sunColor", SUN_COLOR);
	m_pGrassProgram->SetUniform("sampler0", 0);
	glDisable(GL_CULL_FACE);
	for (int i = 0; i < m_grassTransforms.size(); i++) {
		glm::vec3 grassPos = glm::vec3(m_grassTransforms[i][3]);
		if (glm::distance(cullPos, grassPos) > 400.0f) continue;
		if (!IsInFrustum(grassPos, cullPos, cullDir)) continue;
		m_pGrassProgram->SetUniform("modelMatrix", m_grassTransforms[i]);
		m_pGrassMesh->Render();
	}
	glEnable(GL_CULL_FACE);

	RenderParticlesPass(pMainProgram, modelViewMatrixStack, viewMatrix, carModelBase);
	RenderPostProcessingPass();

	DisplayFrameRate();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	SwapBuffers(m_gameWindow.Hdc());
}

void Game::RenderFullScreenQuad()
{
	static GLuint quadVAO = 0;
	static GLuint quadVBO;
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		};
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Game::DisplayFrameRate()
{
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];
	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	m_elapsedTime += m_dt;
	m_frameCount++;

	if (m_elapsedTime > 1000)
	{
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;
		m_frameCount = 0;
	}

	if (m_framesPerSecond > 0) {
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1.0f));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
		int displaySpeed = (int)(abs(m_cameraSpeed) * 1000.0f);
		m_pFtFont->Render(20, height - 50, 20, "Speed: %d km/h", displaySpeed);
		int currentLap = m_pCatmullRom->CurrentLap(m_currentDistance);
		m_pFtFont->Render(20, height - 80, 20, "Current Lap: %d", currentLap);
	}
}
// Processes Windows messages and events
LRESULT Game::ProcessEvents(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	LRESULT result = 0;
	switch (message) {
	case WM_ACTIVATE:
	{
		switch (LOWORD(w_param))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			m_appActive = true;
			m_pHighResolutionTimer->Start();
			break;
		case WA_INACTIVE:
			m_appActive = false;
			break;
		}
		break;
	}
	case WM_SIZE:
		RECT dimensions;
		GetClientRect(window, &dimensions);
		m_gameWindow.SetDimensions(dimensions);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;
	case WM_KEYDOWN:
		switch (w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1': m_cameraMode = 1; break;
		case '2': m_cameraMode = 2; break;
		case '3': m_cameraMode = 3; break;
		case '4':
			m_cameraMode = 4;
			m_pCamera->Set(m_playerPosition + glm::vec3(0.0f, 10.0f, 30.0f), m_playerPosition, glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case VK_F1: m_pAudio->PlayEventSound(); break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}
	return result;
}

Game& Game::GetInstance() { static Game instance; return instance; }
void Game::SetHinstance(HINSTANCE hinstance) { m_hInstance = hinstance; }

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) {
	Game& game = Game::GetInstance();
	game.SetHinstance(hinstance);
	return int(game.Execute());
}