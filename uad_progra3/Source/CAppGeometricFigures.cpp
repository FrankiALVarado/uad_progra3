#include "../stdafx.h"

#include <iostream>
using namespace std;

#include <assert.h>
#include "../Include/Globals.h"
#include "../Include/CAppGeometricFigures.h"
#include "../Include/CWideStringHelper.h"

/* */
CAppGeometricFigures::CAppGeometricFigures() :
	CAppGeometricFigures(CGameWindow::DEFAULT_WINDOW_WIDTH, CGameWindow::DEFAULT_WINDOW_HEIGHT)
{
}

/* */
CAppGeometricFigures::CAppGeometricFigures(int window_width, int window_height) :
	CApp(window_width, window_height),
	m_currentDeltaTime{ 0.0 },
	m_objectRotation{ 0.0 },
	m_objectPosition{ -1.5f, 0.0f, 0.0f },
	m_rotationSpeed{ DEFAULT_ROTATION_SPEED },
	m_pyramidVertexArrayObject{ 0 },
	m_numFacesPyramid{ 0 },
	m_renderPolygonMode{ 0 }
	
{
	cout << "Constructor: CAppGeometricFigures(int window_width, int window_height)" << endl;

	// Initialize class member variables here
	// ======================================
	//
	// ======================================
}

/* */
CAppGeometricFigures::~CAppGeometricFigures()
{
	cout << "Destructor: ~CAppGeometricFigures()" << endl;

	// Free memory allocated in this class instance here
	// =================================================
	//
	if (m_textureID > 0)
	{
		getOpenGLRenderer()->deleteTexture(&m_textureID);
	}

	if (m_pyramidVertexArrayObject > 0)
	{
		getOpenGLRenderer()->freeGraphicsMemoryForObject(&m_pyramidVertexArrayObject);
	}
	// =================================================
}

/* */
void CAppGeometricFigures::initialize()
{
	// Initialize app-specific stuff here
	// ==================================
	//
	std::wstring wresourceFilenameVS, wresourceFilenameFS, wresourceFilenameTexture;
	std::string resourceFilenameVS,resourceFilenameFS, resourceFilenameTexture;

	// Color Shader
	// Load shader file, create OpenGL Shader Object for it
	// -------------------------------------------------------------------------------------------------------------

	// Check shader for the color-only object exists
	if (!CWideStringHelper::GetResourceFullPath(VERTEX_SHADER_3D_OBJECT, wresourceFilenameVS, resourceFilenameVS) ||
		!CWideStringHelper::GetResourceFullPath(FRAGMENT_SHADER_3D_OBJECT, wresourceFilenameFS, resourceFilenameFS))
	{
		cout << "ERROR: Unable to find one or more resources: " << endl;
		cout << "  " << VERTEX_SHADER_3D_OBJECT << endl;
		cout << "  " << FRAGMENT_SHADER_3D_OBJECT << endl;

		return;
	}

	if (!getOpenGLRenderer()->createShaderProgram(
		&m_colorModelShaderId,
		resourceFilenameVS.c_str(),
		resourceFilenameFS.c_str()))
	{
		cout << "ERROR: Unable to load color shader" << endl;
		return;
	}

	// Texture Shader
	// Load shader file, create OpenGL Shader Object for it
	// -------------------------------------------------------------------------------------------------------------

	// Check shader for the textured object exists
	wresourceFilenameFS.clear();
	wresourceFilenameVS.clear();
	resourceFilenameFS.clear();
	resourceFilenameVS.clear();
	if (!CWideStringHelper::GetResourceFullPath(VERTEX_SHADER_TEXTURED_3D_OBJECT, wresourceFilenameVS, resourceFilenameVS) ||
		!CWideStringHelper::GetResourceFullPath(FRAGMENT_SHADER_TEXTURED_3D_OBJECT, wresourceFilenameFS, resourceFilenameFS))
	{
		cout << "ERROR: Unable to find one or more resources: " << endl;
		cout << "  " << VERTEX_SHADER_TEXTURED_3D_OBJECT << endl;
		cout << "  " << FRAGMENT_SHADER_TEXTURED_3D_OBJECT << endl;

		return;
	}

	if (!getOpenGLRenderer()->createShaderProgram(
		&m_texturedModelShaderId,
		resourceFilenameVS.c_str(),
		resourceFilenameFS.c_str()))
	{
		cout << "ERROR: Unable to load texture shader" << endl;
		return;
	}

	// Texture
	// Load texture file, create OpenGL Texture Object
	// -------------------------------------------------------------------------------------------------------------

	// Check texture file exists for the textured cube
	if (!CWideStringHelper::GetResourceFullPath(MC_LEAVES_TEXTURE, wresourceFilenameTexture, resourceFilenameTexture))
	{
		cout << "ERROR: Unable to find one or more resources: " << endl;
		cout << "  " << MC_LEAVES_TEXTURE << endl;
		return;
	}

	// Initialize the texture
	m_textureID = 0;
	if (!loadTexture(resourceFilenameTexture.c_str(), &m_textureID))
	{
		cout << "ERROR: Unable load texture:" << endl;
		cout << "  " << MC_LEAVES_TEXTURE << endl;
		return;
	}

	m_initialized = true;
	createPyramidGeometry();
}

/* */
void CAppGeometricFigures::run()
{
	// Check if CGameWindow object AND Graphics API specific Window library have been initialized
	if (canRun())
	{
		// Create the Window 
		if (getGameWindow()->create(CAPP_PROGRA3_GEOMETRIC_WINDOW_TITLE))
		{
			initialize();

			// Set initial clear screen color
			getOpenGLRenderer()->setClearScreenColor(0.25f, 0.0f, 0.75f);

			// Initialize window width/height in the renderer
			getOpenGLRenderer()->setWindowWidth(getGameWindow()->getWidth());
			getOpenGLRenderer()->setWindowHeight(getGameWindow()->getHeight());

			if (m_initialized)
			{
				getOpenGLRenderer()->setWireframePolygonMode();

				// Enter main loop
				cout << "Entering Main loop" << endl;
				getGameWindow()->mainLoop(this);
			}
		}
	}
}

/* */
void CAppGeometricFigures::update(double deltaTime)
{
	// Do not update if delta time is < 0
	if (deltaTime <= 0.0f)
	{
		return;
	}

	// Update app-specific stuff here
	// ===============================
	//
	double degreesToRotate = 0.0;

	// Save current delta time
	m_currentDeltaTime = deltaTime;

	// Calculate degrees to rotate
	// ----------------------------------------------------------------------------------------------------------------------------------------
	// degrees = rotation speed * delta time 
	// deltaTime is expressed in milliseconds, but our rotation speed is expressed in seconds (convert delta time from milliseconds to seconds)
	degreesToRotate = m_rotationSpeed * (deltaTime / 1000.0);
	// accumulate rotation degrees
	m_objectRotation += degreesToRotate;

	// Reset rotation if needed
	while (m_objectRotation > 360.0)
	{
		m_objectRotation -= 360.0;
	}
	if (m_objectRotation < 0.0)
	{
		m_objectRotation = 0.0;
	}
	// ===============================
}

/* */
void CAppGeometricFigures::render()
{
	CGameMenu *menu = getMenu();

	// If menu is active, render menu
	if (menu != NULL
		&& menu->isInitialized()
		&& menu->isActive())
	{
		//...
	}
	else // Otherwise, render app-specific stuff here...
	{
		// =================================
		//
		// White 
		// Colors are in the 0..1 range, if you want to use RGB, use (R/255, G/255, G/255)
		float color[3] = { 1.0f, 1.0f, 1.0f };
		unsigned int noTexture = 0;
	
		// convert total degrees rotated to radians;
		double totalDegreesRotatedRadians = m_objectRotation * 3.1459 / 180.0;

		// Get a matrix that has both the object rotation and translation
		MathHelper::Matrix4 modelMatrix = MathHelper::ModelMatrix((float)totalDegreesRotatedRadians, m_objectPosition);

		if (m_pyramidVertexArrayObject > 0 && m_numFacesPyramid > 0)
		{
			CVector3 pos2 = m_objectPosition;
			pos2 += CVector3(3.0f, 0.0f, 0.0f);
			MathHelper::Matrix4 modelMatrix2 = MathHelper::ModelMatrix((float)totalDegreesRotatedRadians, pos2);

			// Render pyramid in the first position, using the color shader
			getOpenGLRenderer()->renderObject(
				&m_colorModelShaderId,
				&m_pyramidVertexArrayObject,
				&noTexture,
				m_numFacesPyramid,
				color,
				&modelMatrix,
				COpenGLRenderer::EPRIMITIVE_MODE::TRIANGLES,
				false
			);

			// Render same pyramid (same vertex array object identifier), in a second position, but this time with a texture
			getOpenGLRenderer()->renderObject(
				&m_texturedModelShaderId,
				&m_pyramidVertexArrayObject,
				&m_textureID,
				m_numFacesPyramid,
				color,
				&modelMatrix2,
				COpenGLRenderer::EPRIMITIVE_MODE::TRIANGLES,
				false
			);
		}

		// =================================
	}
}

/* */
void CAppGeometricFigures::onMouseMove(float deltaX, float deltaY)
{
	// Update app-specific stuff when mouse moves here 
	// ===============================================
	//
	// ===============================================
}

/* */
void CAppGeometricFigures::createPyramidGeometry()
{
	float height = 2.25f;
	float sideHalfX = 0.75f;
	float sideHalfZ = 1.0f;
	bool loaded = false;
	float v1[3], v2[3], v3[3], v1v2[3], v1v3[3], norm[3];

	float vData[15] = {
		0.0, height, 0.0,                // TOP
		-sideHalfX,  0.0,   sideHalfZ,   // BOTTOM LEFT, FRONT
	 	 sideHalfX,  0.0,   sideHalfZ,   // BOTTOM RIGHT, FRONT
		-sideHalfX,  0.0,  -sideHalfZ,   // BOTTOM LEFT, BACK
		 sideHalfX,  0.0,  -sideHalfZ    // BOTTOM RIGHT, BACK
	};

	float vertexUVs[10] =
	{
		0.5f,  0.11f, // TOP 
		0.25f, 0.99f, // BOTTOM LEFT, FRONT
		0.75f, 0.99f, // BOTTOM RIGHT, FRONT
		0.11f, 0.40f, // BOTTOM LEFT, BACK
		0.99f, 0.40f  // BOTTOM RIGHT, BACK 
	};

	m_numFacesPyramid = 6;

	unsigned short tIndices[18] = {
		0, 1, 2,    // 0,1,2
		0, 2, 4,    // 0,2,4
		0, 4, 3,    // 0,4,3
		0, 3, 1,    // 0,3,1
		1, 3, 2,    // 1,3,2
		2, 3, 4     // 2,3,4
	};

	float nData[18] = {
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0
	};

	unsigned short nIndices[18] = {
		0, 0, 0,
		1, 1, 1,
		2, 2, 2,
		3, 3, 3,
		4, 4, 4,
		5, 5, 5
	};

	for (int i = 0; i < m_numFacesPyramid; i++)
	{
		// Vertex 1
		v1[0] = vData[tIndices[i * 3]];
		v1[1] = vData[tIndices[i * 3] + 1];
		v1[2] = vData[tIndices[i * 3] + 2];

		// Vertex 2
		v2[0] = vData[tIndices[(i * 3) + 1]];
		v2[1] = vData[tIndices[(i * 3) + 1] + 1];
		v2[2] = vData[tIndices[(i * 3) + 1] + 2];

		// Vertex 3
		v3[0] = vData[tIndices[(i * 3) + 2]];
		v3[1] = vData[tIndices[(i * 3) + 2] + 1];
		v3[2] = vData[tIndices[(i * 3) + 2] + 2];

		// Vector from v2 to v1
		v1v2[0] = v1[0] - v2[0];
		v1v2[1] = v1[1] - v2[1];
		v1v2[2] = v1[2] - v2[2];

		// Vector from v2 to v3
		v1v3[0] = v3[0] - v2[0];
		v1v3[1] = v3[1] - v2[1];
		v1v3[2] = v3[2] - v2[2];

		normcrossprod(v1v2, v1v3, norm);

		nData[i * 3]       = norm[0];
		nData[(i * 3) + 1] = norm[1];
		nData[(i * 3) + 2] = norm[2];
	}

	// Allocate graphics memory for object
	loaded = getOpenGLRenderer()->allocateGraphicsMemoryForObject(
		&m_colorModelShaderId,
		&m_pyramidVertexArrayObject,
		vData,
		5,        // Numero de vertices, internamente el codigo multiplica sizeof(float) * numVertices * 3
		nData,     
		6,
		vertexUVs,
		5,
		tIndices,
		6,        // Numero de indices, internamente el codigo multiplica sizeof(unsigned short) * numIndicesVert * 3
		nIndices,
		6,
		tIndices,
		6
	);

	if (!loaded)
	{
		m_numFacesPyramid = 0;

		if (m_pyramidVertexArrayObject > 0)
		{
			getOpenGLRenderer()->freeGraphicsMemoryForObject(&m_pyramidVertexArrayObject);
			m_pyramidVertexArrayObject = 0;
		}
	}
}

/* */
void CAppGeometricFigures::onF2(int mods)
{
}

/* */
void CAppGeometricFigures::onF3(int mods)
{
	if (m_renderPolygonMode == 0)
	{
		getOpenGLRenderer()->setFillPolygonMode();
		m_renderPolygonMode = 1;
	}
	else
	{
		getOpenGLRenderer()->setWireframePolygonMode();
		m_renderPolygonMode = 0;
	}
}

/* */
void CAppGeometricFigures::normcrossprod(float v1[3], float v2[3], float out[3])
{
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];

	normalize(out);
}

/* */
void CAppGeometricFigures::normalize(float v[3])
{
	float d = sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	if (d == 0.0f)
	{
		return;
	}
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}