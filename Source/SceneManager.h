///////////////////////////////////////////////////////////////////////////////
// scenemanager.h
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "ShapeMeshes.h"

#include <string>
#include <vector>

/***********************************************************
 *  SceneManager
 *
 *  This class contains the code for preparing and rendering
 *  3D scenes, including the shader settings.
 ***********************************************************/
class SceneManager
{
public:
	// constructor
	SceneManager(ShaderManager* pShaderManager);

	// destructor
	~SceneManager();

	struct TEXTURE_INFO
	{
		std::string tag;
		uint32_t ID;
	};

	struct OBJECT_MATERIAL
	{
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
		float shininess;
		std::string tag;
	};

	// Structure used to store all information needed to render
	// a scene object. This replaces hard-coded rendering logic.
	struct SCENE_OBJECT
	{
		std::string meshType;

		glm::vec3 scale;
		glm::vec3 position;

		float rotationX;
		float rotationY;
		float rotationZ;

		std::string textureTag;
		std::string materialTag;

		glm::vec4 color;

		bool useTexture;

		float uvScaleU;
		float uvScaleV;
	};

private:

	// pointer to shader manager object
	ShaderManager* m_pShaderManager;

	// pointer to basic shapes object
	ShapeMeshes* m_basicMeshes;

	// total number of loaded textures
	int m_loadedTextures;

	// loaded textures info
	TEXTURE_INFO m_textureIDs[16];

	// defined object materials
	std::vector<OBJECT_MATERIAL> m_objectMaterials;

	// collection of all scene objects
	std::vector<SCENE_OBJECT> m_sceneObjects;

	// load texture images and convert to OpenGL texture data
	bool CreateGLTexture(
		const char* filename,
		std::string tag);

	// bind loaded OpenGL textures to slots in memory
	void BindGLTextures();

	// free loaded OpenGL textures
	void DestroyGLTextures();

	// find a loaded texture by tag
	int FindTextureID(
		std::string tag);

	int FindTextureSlot(
		std::string tag);

	// find a defined material by tag
	bool FindMaterial(
		std::string tag,
		OBJECT_MATERIAL& material);

	// set transformation values into shader
	void SetTransformations(
		glm::vec3 scaleXYZ,
		float XrotationDegrees,
		float YrotationDegrees,
		float ZrotationDegrees,
		glm::vec3 positionXYZ);

	// set color values into shader
	void SetShaderColor(
		float redColorValue,
		float greenColorValue,
		float blueColorValue,
		float alphaValue);

	// set texture data into shader
	void SetShaderTexture(
		std::string textureTag);

	// set UV scale for texture mapping
	void SetTextureUVScale(
		float u,
		float v);

	// set object material into shader
	void SetShaderMaterial(
		std::string materialTag);

	// define materials used by scene objects
	void DefineObjectMaterials();

	// configure scene lighting
	void SetupSceneLights();

	// create all objects used in the scene
	void BuildSceneObjects();

	// render a single scene object
	void RenderSceneObject(const SCENE_OBJECT& object);

public:

	// prepare textures, materials, meshes, and lights
	void PrepareScene();

	// render the complete scene
	void RenderScene();
};