#pragma once

#ifndef _OPENGL_RENDERER_H
#define _OPENGL_RENDERER_H

// include glad *before* glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "MathHelper.h"
#include "COpenGLShaderProgram.h"

#include <map>
#include <vector>
using namespace std;

#define BUFFER_OFFSET(a) ((void*)(a))
#define MIN_CAMERA_DISTANCE 5.0f
#define MAX_CAMERA_DISTANCE 1000.0f
#define MOVE_CAMERA_DELTA 1.5f

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* ONLY MODIFY THIS FILE IF YOU KNOW WHAT YOU'RE DOING...
* AND EVEN THEN... BE CAREFUL...
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// =========================================================================================================
// Class for a simple OpenGL renderer targeted for OpenGL 4.3
// UAD lab PCs support it
// *NOTE: This code has been tested to work fine on NVidia cards. Radeon cards seem to behave differently...
// =========================================================================================================

class COpenGLRenderer
{
public:
	enum EPRIMITIVE_MODE
	{
		POINTS = 0,
		LINES,
		LINE_STRIP,
		LINE_LOOP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN
	};

private:
	int m_frameBufferWidth;
	int m_frameBufferHeight;
	bool m_OpenGLError;

	std::map<int, COpenGLShaderProgram*> m_shaderProgramWrappers;
	std::vector<std::string> m_expectedUniformsInShader;
	std::vector<std::string> m_expectedAttributesInShader;

	float m_cameraDistance; // Distance from camera view point to target point, expressed in OpenGL units

	// TEST OBJECT VARS
	// When no 3D object is loaded, we display a test object (spinning cube)
	// ===========================
	GLuint m_testCubeShaderProgramID;
	GLuint m_testCubeVAOID;

	GLuint m_mCCubeShaderProgramID;
	GLuint m_mCCubeVAOID;
	// ===========================

	//
	void setCurrentVertexAttribPointer(int vertexShaderAttribPos, int numComponents, int type) const;

	// This method is internally called by allocateGraphicsMemoryForObject(), (that's why it's a private method).
	//
	// There's no easy way to render multi-indexed arrays (one index for vertex, one index for normal, one index for UV coords... but that's how the info
	// is saved in the .obj format), 
	// so this method duplicates some vertices in order to render correctly.
	// The geometry created by this method is used to allocate the graphics card memory.
	// The caller can discard the geometry if it's not going to be used for anything else...
	bool generateRenderGeometry(
		GLfloat *vertices, int numVertices,
		GLfloat *normals, int numNormals,
		GLfloat *UVcoords, int numUVCoords,
		int numIndicesVert,
		unsigned short *indicesVertices,
		unsigned short *indicesNormals,
		unsigned short *indicesUVCoords,
		GLfloat *finalVertices,
		GLfloat *finalNormals,
		GLfloat *finalUVCoords,
		int *numTriangles
	);

	//
	bool useShaderProgram(const unsigned int * const shaderProgramId) const;

	//
	GLuint generateVertexArrayObjectID() const;

	//
	GLuint generateBufferObjectID(GLfloat * const vertices, int numVertices) const;

	//
	void deleteBufferObject(GLuint *id);

	//
	void deleteVertexArrayObject(GLuint *id);

	//
	GLenum primitiveModeToGLEnum(EPRIMITIVE_MODE mode) const;

	//
	COpenGLShaderProgram* getShaderProgramWrapper(unsigned int id);

	//
	bool deleteShaderProgram(unsigned int *shaderProgramId);

public:
	// Constructor and Destructor
	COpenGLRenderer();
	~COpenGLRenderer();

	// =================================================================
	// Allocates graphics memory for a given 3D object 
	// Note: shader program must be already created
	// =================================================================
	bool allocateGraphicsMemoryForObject(
		unsigned int *shaderProgramId,
		unsigned int *vertexArrayObjectID,
		GLfloat *vertices, int numVertices,
		GLfloat *normals, int numNormals,
		GLfloat *UVcoords, int numUVCoords,
		unsigned short *indicesVertices, int numIndicesVert,
		unsigned short *indicesNormals, int numIndicesNormals,
		unsigned short *indicesUVCoords, int numIndicesUVCoords);

	// =================================================================
	// Allocates graphics memory for a given 3D object
	// Note: shader program must be already created
	// =================================================================
	bool allocateGraphicsMemoryForObject(
		const unsigned int * const shaderProgramId,
		unsigned int *vertexArrayObjectID,
		GLfloat *vertices, int numVertices,
		unsigned short *indicesVertices, int numIndicesVert);

	// =================================================================
	// Free graphics memory for a given 3D object 
	// =================================================================
	bool freeGraphicsMemoryForObject(
		//unsigned int *shaderProgramId, 
		unsigned int *vertexArrayObjectID);

	// =================================================================
	// Allocates graphics memory for a menu item (a quad)
	// We don't allocate the memory for all menu items at once because
	// we change the color (GLSL uniform) for the currently selected menu item
	// =================================================================
	bool allocateGraphicsMemoryForMenuItem(
		float topX, float topY, float menuItemHeight,
		float *uvCoords,
		unsigned int *shaderProgramId,
		unsigned int *vertexArrayObjectID
	);

	//
	bool createShaderProgram(
		unsigned int *shaderProgramId,
		const char *vertexShader,
		const char *fragmentShader);

	//
	bool createTextureObject(
		unsigned int *textureObjectId,
		unsigned char *textureData,
		int width,
		int height);

	// 
	bool renderWireframeObject(
		unsigned int *shaderProgramId,
		unsigned int *vertexArrayObjectId,
		int numFaces,
		GLfloat *objectColor,
		MathHelper::Matrix4 *objectTransformation);

	// 
	bool renderObject(
		unsigned int *shaderProgramId,
		unsigned int *vertexArrayObjectId,
		unsigned int *textureObjectId,
		int numFaces,
		GLfloat *objectColor,
		MathHelper::Matrix4 *objectTransformation = NULL,
		EPRIMITIVE_MODE mode = TRIANGLES,
		bool drawIndexedPrimitives = false);
	//
	bool renderObjectMultiMat(
		int scopeF,
		int scopeL,
		unsigned int *shaderProgramId,
		unsigned int *vertexArrayObjectId,
		unsigned int *textureObjMatId,
		int numFaces,
		GLfloat *objectColor,
		MathHelper::Matrix4 *objectTransformation = NULL,
		EPRIMITIVE_MODE mode = TRIANGLES,
		bool drawIndexedPrimitives = false
	);

	//
	bool renderMenuItem(
		unsigned int *shaderProgramId,
		unsigned int *textureObjectId,
		unsigned int *vertexArrayObjectId,
		GLfloat *menuItemColor);

	//
	void setFramebufferWidth(int width) { m_frameBufferWidth = width; }
	void setFramebufferHeight(int height) { m_frameBufferHeight = height; }
	int getFramebufferWidth() const { return m_frameBufferWidth; }
	int getFramebufferHeight() const { return m_frameBufferHeight; }

	//
	void renderColorCube(MathHelper::Matrix4 *objectTransformation = NULL);
	void renderColorCube(
		MathHelper::Matrix4 *modelMatrix,
		MathHelper::Matrix4 *viewMatrix,
		MathHelper::Matrix4 *projectionMatrix);
	void initializeColorCube();

	//
	void renderTexturedCube(unsigned int cubeTextureID, MathHelper::Matrix4 *objectTransformation = NULL);
	void renderTexturedCube(
		unsigned int cubeTextureID,
		MathHelper::Matrix4 * modelMatrix,
		MathHelper::Matrix4 *viewMatrix,
		MathHelper::Matrix4 *projectionMatrix);

	void initializeTexturedCube();

	//
	bool checkOpenGLError(char *operationAttempted);

	//
	void setClearScreenColor(float r, float g, float b) { glClearColor(r, g, b, 1.0f); }

	//
	void deleteTexture(unsigned int *id);

	//
	float getCameraDistance() const { return m_cameraDistance; }
	void setCameraDistance(float d) { m_cameraDistance = d; }

	//
	void moveCamera(float direction);

	//
	void enableDepthTest() { glEnable(GL_DEPTH_TEST); }

	//
	void setViewport(int width, int height) { glViewport(0, 0, width, height); }

	//
	const GLubyte * getOpenGLString(GLenum id) { return glGetString(id); }

	//
	void clearScreen() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	//
	void drawString(unsigned int *textureObjectId, std::string &text, float x, float y, CVector3 &color);

	//
	bool isDebugContextEnabled() const;

	//
	void activateOpenGLDebugging();

	// 
	void setWireframePolygonMode() { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }

	// 
	void setFillPolygonMode() { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

	//
	static void APIENTRY debugOutputCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar *message,
		const GLvoid *userParam);
};

#endif // !_OPENGL_RENDERER_H

// https://www.khronos.org/opengl/wiki/Common_Mistakes