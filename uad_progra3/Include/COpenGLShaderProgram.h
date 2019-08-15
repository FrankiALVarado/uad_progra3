#pragma once

#ifndef _OPENGL_SHADER_PROGRAM_
#define _OPENGL_SHADER_PROGRAM_

#include <map>
#include <string>
using namespace std;

#define UNIFORM_MODEL_MATRIX		"uModelMatrix"
#define UNIFORM_VIEW_MATRIX			"uViewMatrix"
#define UNIFORM_PROJECTION_MATRIX	"uProjMatrix"
#define UNIFORM_COLOR				"uColor"
#define UNIFORM_TEXTURE_SAMPLER		"textureSampler"

#define ATTRIBUTE_POSITION			"attPosition"
#define ATTRIBUTE_NORMAL			"attNormal"
#define ATTRIBUTE_UV				"attUV"
#define ATTRIBUTE_COLOR				"attColor"

class COpenGLShaderProgram
{
private:

	// Shader program ID
	unsigned int m_shaderProgramID;

	// Uniform locations (Uniforms change per-object) (model, view, and projection matrix. color. texture sampler)
	std::map<std::string, int> m_uniformLocations;

	// Attributes locations (Attributes change per-vertex) (position, normal, UV, and color)
	std::map<std::string, int> m_attributeLocations;

public:
	COpenGLShaderProgram();
	~COpenGLShaderProgram();

	// Load shader

	// Setters / Getters
	//

	// Shader Program ID
	unsigned int getShaderProgramID() const { return m_shaderProgramID; }
	void setShaderProgramID(unsigned int id) { m_shaderProgramID = id; }

	// Uniforms
	int getUniformLocation(std::string uniformName) const;
	void setUniformLocation(std::string uniformName, int id);

	int getModelMatrixUniformLocation() const { return getUniformLocation(UNIFORM_MODEL_MATRIX); }
	int getViewMatrixUniformLocation() const { return getUniformLocation(UNIFORM_VIEW_MATRIX); }
	int getProjectionMatrixUniformLocation() const { return getUniformLocation(UNIFORM_PROJECTION_MATRIX); }
	int getColorUniformLocation() const { return getUniformLocation(UNIFORM_COLOR); }
	int getTextureSamplerUniformLocation() const { return getUniformLocation(UNIFORM_TEXTURE_SAMPLER); }

	// Attributes
	int getAttributeLocation(std::string attributeName) const;
	void setAttributeLocation(std::string attribName, int id);

	int getPositionAttributeLocation() const { return getAttributeLocation(ATTRIBUTE_POSITION); }
	int getNormalAttributeLocation() const { return getAttributeLocation(ATTRIBUTE_NORMAL); }
	int getUVAttributeLocation() const { return getAttributeLocation(ATTRIBUTE_UV); }
	int getColorAttributeLocation() const { return getAttributeLocation(ATTRIBUTE_COLOR); }
};


#endif // !_OPENGL_SHADER_PROGRAM_
