///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials,
// lighting for the articulated desk lamp scene
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>
#include <iostream>
#include <string>

namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
	m_loadedTextures = 0;
}

SceneManager::~SceneManager()
{
	DestroyGLTextures();

	m_pShaderManager = NULL;

	if (m_basicMeshes != NULL)
	{
		delete m_basicMeshes;
		m_basicMeshes = NULL;
	}

	m_objectMaterials.clear();
}

bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* image = stbi_load(filename, &width, &height, &colorChannels, 0);

	if (image)
	{
		std::cout << "Successfully loaded image: " << filename << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (colorChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0,
				GL_RGB, GL_UNSIGNED_BYTE, image);
		}
		else if (colorChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, image);
		}
		else
		{
			stbi_image_free(image);
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image: " << filename << std::endl;
	return false;
}

void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glDeleteTextures(1, &m_textureIDs[i].ID);
	}

	m_loadedTextures = 0;
}

int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
		{
			index++;
		}
	}

	return textureID;
}

int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
		{
			index++;
		}
	}

	return textureSlot;
}

bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return false;
	}

	int index = 0;
	bool bFound = false;

	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;

			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return bFound;
}

void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	scale = glm::scale(scaleXYZ);

	rotationX = glm::rotate(
		glm::radians(XrotationDegrees),
		glm::vec3(1.0f, 0.0f, 0.0f));

	rotationY = glm::rotate(
		glm::radians(YrotationDegrees),
		glm::vec3(0.0f, 1.0f, 0.0f));

	rotationZ = glm::rotate(
		glm::radians(ZrotationDegrees),
		glm::vec3(0.0f, 0.0f, 1.0f));

	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

void SceneManager::SetShaderTexture(std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = FindTextureSlot(textureTag);

		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

void SceneManager::SetShaderMaterial(std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;

		bool bReturn = FindMaterial(materialTag, material);

		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

void SceneManager::DefineObjectMaterials()
{
	OBJECT_MATERIAL tableMaterial;
	tableMaterial.diffuseColor = glm::vec3(0.55f, 0.35f, 0.20f);
	tableMaterial.specularColor = glm::vec3(0.45f, 0.35f, 0.25f);
	tableMaterial.shininess = 32.0f;
	tableMaterial.tag = "tableMaterial";
	m_objectMaterials.push_back(tableMaterial);

	OBJECT_MATERIAL metalMaterial;
	metalMaterial.diffuseColor = glm::vec3(0.90f, 0.84f, 0.65f);
	metalMaterial.specularColor = glm::vec3(0.80f, 0.75f, 0.55f);
	metalMaterial.shininess = 48.0f;
	metalMaterial.tag = "metalMaterial";
	m_objectMaterials.push_back(metalMaterial);

	OBJECT_MATERIAL darkMaterial;
	darkMaterial.diffuseColor = glm::vec3(0.05f, 0.05f, 0.05f);
	darkMaterial.specularColor = glm::vec3(0.35f, 0.35f, 0.35f);
	darkMaterial.shininess = 64.0f;
	darkMaterial.tag = "darkMaterial";
	m_objectMaterials.push_back(darkMaterial);

	OBJECT_MATERIAL shadeMaterial;
	shadeMaterial.diffuseColor = glm::vec3(0.22f, 0.24f, 0.26f);
	shadeMaterial.specularColor = glm::vec3(0.45f, 0.45f, 0.45f);
	shadeMaterial.shininess = 40.0f;
	shadeMaterial.tag = "shadeMaterial";
	m_objectMaterials.push_back(shadeMaterial);

	OBJECT_MATERIAL bulbMaterial;
	bulbMaterial.diffuseColor = glm::vec3(1.0f, 0.85f, 0.30f);
	bulbMaterial.specularColor = glm::vec3(1.0f, 0.95f, 0.70f);
	bulbMaterial.shininess = 96.0f;
	bulbMaterial.tag = "bulbMaterial";
	m_objectMaterials.push_back(bulbMaterial);
}

void SceneManager::SetupSceneLights()
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseLightingName, true);

		// Turn off unused spotlight first.
		m_pShaderManager->setBoolValue("spotLight.bActive", false);

		for (int i = 0; i < 5; i++)
		{
			std::string lightName = "pointLights[" + std::to_string(i) + "]";
			m_pShaderManager->setBoolValue(lightName + ".bActive", false);
		}

		// Soft directional fill light to brighten the overall scene.
		m_pShaderManager->setBoolValue("directionalLight.bActive", true);
		m_pShaderManager->setVec3Value("directionalLight.direction", glm::vec3(-0.3f, -1.0f, -0.4f));
		m_pShaderManager->setVec3Value("directionalLight.ambient", glm::vec3(0.20f, 0.20f, 0.22f));
		m_pShaderManager->setVec3Value("directionalLight.diffuse", glm::vec3(0.35f, 0.35f, 0.38f));
		m_pShaderManager->setVec3Value("directionalLight.specular", glm::vec3(0.25f, 0.25f, 0.28f));

		// Main warm point light placed near the visible bulb.
		m_pShaderManager->setBoolValue("pointLights[0].bActive", true);
		m_pShaderManager->setVec3Value("pointLights[0].position", glm::vec3(1.92f, 3.92f, 0.0f));
		m_pShaderManager->setVec3Value("pointLights[0].ambient", glm::vec3(0.40f, 0.34f, 0.22f));
		m_pShaderManager->setVec3Value("pointLights[0].diffuse", glm::vec3(1.0f, 0.85f, 0.45f));
		m_pShaderManager->setVec3Value("pointLights[0].specular", glm::vec3(1.0f, 0.92f, 0.60f));

		// Secondary cool fill point light so no object is completely hidden in shadow.
		m_pShaderManager->setBoolValue("pointLights[1].bActive", true);
		m_pShaderManager->setVec3Value("pointLights[1].position", glm::vec3(-3.5f, 5.0f, 4.0f));
		m_pShaderManager->setVec3Value("pointLights[1].ambient", glm::vec3(0.30f, 0.30f, 0.35f));
		m_pShaderManager->setVec3Value("pointLights[1].diffuse", glm::vec3(0.48f, 0.50f, 0.65f));
		m_pShaderManager->setVec3Value("pointLights[1].specular", glm::vec3(0.42f, 0.42f, 0.52f));
	}
}

void SceneManager::PrepareScene()
{
	// Load texture images before rendering.
	CreateGLTexture("CS 330 Refrence image.jpeg", "lampTexture");
	CreateGLTexture("textures/darkwood.jpg", "tableTexture");

	BindGLTextures();

	// Define Phong materials and lights for Milestone Five.
	DefineObjectMaterials();
	SetupSceneLights();

	// Load meshes used to create the desk lamp scene.
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadBoxMesh();

	// Build the collection of scene objects.
	BuildSceneObjects();
}

void SceneManager::RenderScene()
{
	for (const SCENE_OBJECT& object : m_sceneObjects)
	{
		RenderSceneObject(object);
	}
}

void SceneManager::BuildSceneObjects()
{
	m_sceneObjects.clear();

	// Helper function used to create and store each scene object.
	auto addObject = [this](
		const std::string& meshType,
		const glm::vec3& scale,
		const glm::vec3& position,
		float rotationX,
		float rotationY,
		float rotationZ,
		const std::string& textureTag,
		const std::string& materialTag,
		bool useTexture,
		float uvScaleU,
		float uvScaleV,
		const glm::vec4& color)
	{
		SCENE_OBJECT object;

		object.meshType = meshType;
		object.scale = scale;
		object.position = position;

		object.rotationX = rotationX;
		object.rotationY = rotationY;
		object.rotationZ = rotationZ;

		object.textureTag = textureTag;
		object.materialTag = materialTag;
		object.useTexture = useTexture;

		object.uvScaleU = uvScaleU;
		object.uvScaleV = uvScaleV;
		object.color = color;

		m_sceneObjects.push_back(object);
	};

	// Table surface
	addObject(
		"Plane",
		glm::vec3(12.0f, 1.0f, 8.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		0.0f, 0.0f, 0.0f,
		"tableTexture",
		"tableMaterial",
		true,
		4.0f, 4.0f,
		glm::vec4(1.0f));

	// Lamp base
	addObject(
		"Cylinder",
		glm::vec3(1.50f, 0.22f, 1.50f),
		glm::vec3(-2.5f, 0.15f, 0.0f),
		0.0f, 0.0f, 0.0f,
		"lampTexture",
		"metalMaterial",
		true,
		1.0f, 1.0f,
		glm::vec4(1.0f));

	// Top rounded base
	addObject(
		"Sphere",
		glm::vec3(1.15f, 0.15f, 1.15f),
		glm::vec3(-2.5f, 0.28f, 0.0f),
		0.0f, 0.0f, 0.0f,
		"lampTexture",
		"metalMaterial",
		true,
		1.0f, 1.0f,
		glm::vec4(1.0f));

	// Base connector
	addObject(
		"Box",
		glm::vec3(0.30f, 0.45f, 0.30f),
		glm::vec3(-2.3f, 0.52f, 0.0f),
		0.0f, 0.0f, -20.0f,
		"",
		"darkMaterial",
		false,
		1.0f, 1.0f,
		glm::vec4(0.02f, 0.02f, 0.02f, 1.0f));

	// Lower arm hinge
	addObject(
		"Sphere",
		glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(-2.10f, 0.82f, 0.0f),
		0.0f, 0.0f, 0.0f,
		"",
		"darkMaterial",
		false,
		1.0f, 1.0f,
		glm::vec4(0.02f, 0.02f, 0.02f, 1.0f));

	// Lower support rod 1
	addObject(
		"Cylinder",
		glm::vec3(0.06f, 2.30f, 0.06f),
		glm::vec3(-1.60f, 1.85f, -0.12f),
		0.0f, 0.0f, -28.0f,
		"lampTexture",
		"metalMaterial",
		true,
		3.0f, 3.0f,
		glm::vec4(1.0f));

	// Lower support rod 2
	addObject(
		"Cylinder",
		glm::vec3(0.06f, 2.30f, 0.06f),
		glm::vec3(-1.40f, 1.85f, 0.12f),
		0.0f, 0.0f, -28.0f,
		"lampTexture",
		"metalMaterial",
		true,
		3.0f, 3.0f,
		glm::vec4(1.0f));

	// Lower arm plate 1
	addObject(
		"Box",
		glm::vec3(0.30f, 0.12f, 0.08f),
		glm::vec3(-1.95f, 1.15f, 0.0f),
		0.0f, 0.0f, -28.0f,
		"",
		"darkMaterial",
		false,
		1.0f, 1.0f,
		glm::vec4(0.03f, 0.03f, 0.03f, 1.0f));

	// Lower arm plate 2
	addObject(
		"Box",
		glm::vec3(0.30f, 0.12f, 0.08f),
		glm::vec3(-1.05f, 2.60f, 0.0f),
		0.0f, 0.0f, -28.0f,
		"",
		"darkMaterial",
		false,
		1.0f, 1.0f,
		glm::vec4(0.03f, 0.03f, 0.03f, 1.0f));

	// Middle hinge
	addObject(
		"Sphere",
		glm::vec3(0.28f, 0.28f, 0.28f),
		glm::vec3(-0.85f, 2.95f, 0.0f),
		0.0f, 0.0f, 0.0f,
		"",
		"darkMaterial",
		false,
		1.0f, 1.0f,
		glm::vec4(0.02f, 0.02f, 0.02f, 1.0f));

	// Upper support rod 1
	addObject(
		"Cylinder",
		glm::vec3(0.06f, 2.10f, 0.06f),
		glm::vec3(-0.25f, 3.95f, -0.12f),
		0.0f, 0.0f, -25.0f,
		"lampTexture",
		"metalMaterial",
		true,
		3.0f, 3.0f,
		glm::vec4(1.0f));

	// Upper support rod 2
	addObject(
		"Cylinder",
		glm::vec3(0.06f, 2.10f, 0.06f),
		glm::vec3(-0.05f, 3.95f, 0.12f),
		0.0f, 0.0f, -25.0f,
		"lampTexture",
		"metalMaterial",
		true,
		3.0f, 3.0f,
		glm::vec4(1.0f));

	// Upper hinge
	addObject(
		"Sphere",
		glm::vec3(0.28f, 0.28f, 0.28f),
		glm::vec3(0.55f, 4.95f, 0.0f),
		0.0f, 0.0f, 0.0f,
		"",
		"darkMaterial",
		false,
		1.0f, 1.0f,
		glm::vec4(0.02f, 0.02f, 0.02f, 1.0f));

	// Lamp neck
	addObject(
		"Cylinder",
		glm::vec3(0.16f, 0.70f, 0.16f),
		glm::vec3(0.95f, 4.70f, 0.0f),
		0.0f, 0.0f, -55.0f,
		"lampTexture",
		"metalMaterial",
		true,
		1.0f, 1.0f,
		glm::vec4(1.0f));

	// Lamp head
	addObject(
		"Sphere",
		glm::vec3(0.55f, 0.45f, 0.55f),
		glm::vec3(1.30f, 4.50f, 0.0f),
		0.0f, 0.0f, -55.0f,
		"lampTexture",
		"shadeMaterial",
		true,
		1.0f, 1.0f,
		glm::vec4(1.0f));

	// Main lampshade
	addObject(
		"Cone",
		glm::vec3(0.90f, 0.75f, 0.90f),
		glm::vec3(1.72f, 4.20f, 0.0f),
		0.0f, 0.0f, -65.0f,
		"lampTexture",
		"shadeMaterial",
		true,
		1.0f, 1.0f,
		glm::vec4(1.0f));

	// Bright underside opening
	addObject(
		"Cylinder",
		glm::vec3(0.78f, 0.06f, 0.78f),
		glm::vec3(2.08f, 3.72f, 0.0f),
		0.0f, 0.0f, -65.0f,
		"lampTexture",
		"bulbMaterial",
		true,
		2.0f, 2.0f,
		glm::vec4(1.0f));

	// Light bulb
	addObject(
		"Sphere",
		glm::vec3(0.22f, 0.22f, 0.22f),
		glm::vec3(1.92f, 3.92f, 0.0f),
		0.0f, 0.0f, 0.0f,
		"",
		"bulbMaterial",
		false,
		1.0f, 1.0f,
		glm::vec4(1.0f, 0.85f, 0.30f, 1.0f));

	// Soft shadow accent on the table
	addObject(
		"Sphere",
		glm::vec3(1.20f, 0.02f, 0.40f),
		glm::vec3(1.70f, 0.03f, -0.40f),
		0.0f, 0.0f, -15.0f,
		"",
		"darkMaterial",
		false,
		1.0f, 1.0f,
		glm::vec4(0.01f, 0.01f, 0.01f, 1.0f));
}

void SceneManager::RenderSceneObject(const SCENE_OBJECT& object)
{
    SetTransformations(
        object.scale,
        object.rotationX,
        object.rotationY,
        object.rotationZ,
        object.position);

    if (object.useTexture)
    {
        SetShaderTexture(object.textureTag);
        SetTextureUVScale(object.uvScaleU, object.uvScaleV);
    }
    else
    {
        SetShaderColor(
            object.color.r,
            object.color.g,
            object.color.b,
            object.color.a);
    }

    SetShaderMaterial(object.materialTag);

    if (object.meshType == "Plane")
        m_basicMeshes->DrawPlaneMesh();
    else if (object.meshType == "Cylinder")
        m_basicMeshes->DrawCylinderMesh();
    else if (object.meshType == "Cone")
        m_basicMeshes->DrawConeMesh();
    else if (object.meshType == "Sphere")
        m_basicMeshes->DrawSphereMesh();
    else if (object.meshType == "Box")
        m_basicMeshes->DrawBoxMesh();
}
