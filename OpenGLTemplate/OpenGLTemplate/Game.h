// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file, heavily modified with custom logic.
// ---------------------------------------------------------
#pragma once

#include "Common.h"
#include "GameWindow.h"
#include "MatrixStack.h"

class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class CPyramid;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CCylinder;

// Main game class which acts as the central hub holding all of the objects and systems
class Game {
private:
	void Initialise();
	void Update();
	void Render();

	// Splits the update loop into smaller chunks so its easier to code and maintain
	void UpdatePhysicsAndInput();
	void UpdateCollisions();
	void UpdateParticles();
	void UpdateAudioState();

	// Splits the render loop into passes (shadows, water, particles, post processing)
	void RenderShadowMap(const glm::mat4& lightSpaceMatrix, const glm::mat4& carModel);
	void RenderWaterPass(const glm::mat4& viewMatrix, const glm::vec3& sunEyeDir, int cubeMapTextureUnit);
	void RenderParticlesPass(CShaderProgram* pMainProgram, glutil::MatrixStack& modelViewMatrixStack, const glm::mat4& viewMatrix, const glm::mat4& carModelBase);
	void RenderPostProcessingPass();

	// Checks if an object is actually in front of the camera before drawing it
	bool IsInFrustum(const glm::vec3& objPos, const glm::vec3& cullPos, const glm::vec3& cullDir);

	CSkybox* m_pSkybox;
	CCamera* m_pCamera;
	vector <CShaderProgram*>* m_pShaderPrograms;
	CPlane* m_pPlanarTerrain;
	CFreeTypeFont* m_pFtFont;

	// Shadow mapping framebuffers
	CShaderProgram* m_pShadowProgram;
	GLuint m_shadowMapFBO;
	GLuint m_shadowMapTexture;

	// Flat water plane setup
	CShaderProgram* m_pWaterProgram;
	GLuint m_vaoWater;
	GLuint m_vboWater;
	int m_waterVertexCount;
	float m_t;

	CShaderProgram* m_pInstancedProgram;
	CShaderProgram* m_pPyramidProgram;
	CShaderProgram* m_pEdgeMarkerProgram;
	CShaderProgram* m_pBrightFilterProgram;
	CShaderProgram* m_pBloomCompositeProgram;
	CShaderProgram* m_pBlurProgram;

	// 3d models
	COpenAssetImportMesh* m_pBarrelMesh;
	COpenAssetImportMesh* m_pHorseMesh;
	COpenAssetImportMesh* m_pCarMesh;
	COpenAssetImportMesh* m_pTreeMesh1;
	COpenAssetImportMesh* m_pTreeMesh2;

	CSphere* m_pSphere;
	CPyramid* m_pPyramid;
	CCylinder* m_pCylinder;

	CCatmullRom* m_pCatmullRom;

	CHighResolutionTimer* m_pHighResolutionTimer;
	CAudio* m_pAudio;

	// Post processing framebuffers (fbo = main scene, fbo2 = brighter spots, fbo3 = bloom combined)
	GLuint m_fbo;
	GLuint m_renderTexture;
	GLuint m_depthBuffer;

	GLuint m_fbo2;
	GLuint m_renderTexture2;

	GLuint m_fbo3;
	GLuint m_renderTexture3;

	GLuint m_vboQuad;

	// Item placements and hitboxes
	std::vector<glm::mat4> m_barrelTransforms;
	std::vector<glm::mat4> m_coneTransforms;

	struct BoostOrb {
		glm::vec3 originalPosition;
		float cooldownTimer;
		bool active;
	};
	std::vector<BoostOrb> m_boostOrbs;

	struct LogObstacle {
		glm::mat4 transform;
		glm::vec3 position;
		bool active;
	};
	std::vector<LogObstacle> m_logs;

	struct TreeInstance {
		glm::mat4 transform;
		int variant;
	};
	std::vector<TreeInstance> m_treeInstances;

	COpenAssetImportMesh* m_pGrassMesh;
	CShaderProgram* m_pGrassProgram;
	std::vector<glm::mat4> m_grassTransforms;

	// Leaf particle system properties
	struct LeafParticle {
		glm::vec3 position;
		glm::vec3 velocity;
		float rotation;
		float rotSpeed;
		float life;
		float scale;
	};
	std::vector<LeafParticle> m_leaves;

	// Dirt  particle system
	struct DirtParticle {
		glm::vec3 position;
		glm::vec3 velocity;
		float life;
		float scale;
		float rotation;
	};
	std::vector<DirtParticle> m_dirtParticles;

	struct SpeedParticle {
		glm::vec3 offset;
		float speedMultiplier;
	};
	std::vector<SpeedParticle> m_particles;
	GLuint m_vaoSpeedLine;

	// Player state
	glm::vec3 m_playerPosition;
	glm::mat4 m_playerOrientation;

	float m_collisionCooldown;
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;
	int m_cameraMode;
	float m_steerAngle;
	float m_currentDistance;
	float m_cameraSpeed;
	float m_lateralPosition;
	bool m_pickupActive;

	void RenderFullScreenQuad();

public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window, UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow& m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
};