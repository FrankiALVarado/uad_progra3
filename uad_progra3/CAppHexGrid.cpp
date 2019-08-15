#include "CAppHexGrid.h"

#include "../uad_progra3/stdafx.h"

#include <iostream>
using namespace std;

#include <assert.h>
#include "../uad_progra3/Include/CApp.h"
#include "../uad_progra3/Include/Globals.h"
#include "../uad_progra3/Include/CWideStringHelper.h"
#include "CGrid.h"


/* */
CAppHexGrid::CAppHexGrid() :
	CAppHexGrid(CGameWindow::DEFAULT_WINDOW_WIDTH, CGameWindow::DEFAULT_WINDOW_HEIGHT)
{
}

/* */
CAppHexGrid::CAppHexGrid(int window_width, int window_height) :
	CApp(window_width, window_height),
	m_currentDeltaTime{ 0.0 },
	m_objectRotation{ 0.0 },
	m_objectPosition{ -1.5f, 0.0f, 0.0f },
	m_rotationSpeed{ DEFAULT_ROTATION_SPEED },
	m_HexgridVertexArrayObject{ 0 },
	m_numFacesHexagon{ 0 },
	m_renderPolygonMode{ 0 }
	
{
	cout << "Constructor: CAppGeometricFigures(int window_width, int window_height)" << endl;

	// Initialize class member variables here
	// ======================================
	//
	// ======================================
}

/* */
CAppHexGrid::~CAppHexGrid()
{
	cout << "Destructor: ~CAppHexGrid()" << endl;

	// Free memory allocated in this class instance here
	// =================================================
	//
	if (m_textureID > 0)
	{
		getOpenGLRenderer()->deleteTexture(&m_textureID);
	}

	if (m_HexgridVertexArrayObject > 0)
	{
		getOpenGLRenderer()->freeGraphicsMemoryForObject(&m_HexgridVertexArrayObject);
	}
	// =================================================
}

/* */
void CAppHexGrid::initialize()
{
	// Initialize app-specific stuff here
	// ==================================
	//
	std::wstring wresourceFilenameVS, wresourceFilenameFS, wresourceFilenameTexture;
	std::string resourceFilenameVS, resourceFilenameFS, resourceFilenameTexture;

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
	CreateGridGeometry();
}

/* */
void CAppHexGrid::run()
{
	// Check if CGameWindow object AND Graphics API specific Window library have been initialized
	if (canRun())
	{
		// Create the Window 
		if (getGameWindow()->create("CAPP_HEXGRID"))
		{
			initialize();

			// Set initial clear screen color
			getOpenGLRenderer()->setClearScreenColor(0.25f, 0.0f, 0.75f);

			// Initialize window width/height in the renderer
			//getOpenGLRenderer()->setWindowWidth(getGameWindow()->getWidth());
			//getOpenGLRenderer()->setWindowHeight(getGameWindow()->getHeight());

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
void CAppHexGrid::update(double deltaTime)
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
	m_objectRotation += 0;//degreesToRotate;

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
void CAppHexGrid::render()
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
		MathHelper::Matrix4 modelMatrix = MathHelper::SimpleModelMatrixRotationTranslation((float)totalDegreesRotatedRadians, m_objectPosition);

		if (m_HexgridVertexArrayObject > 0 && m_numFacesHexagon > 0)
		{
			CGridCell cell;
			CGrid grid;
			CVector3 pos2 = m_objectPosition;
			for (int j = 0; j < m_row; j++) {
				if (m_pointy) {
					if (j % 2 == 0) {
							pos2 = CVector3(0.0f, 0.0f, m_height * j * 0.75f);
							cell.m_CenterPosition = pos2;
							grid.m_Cells.push_back(cell);
					}
					else {
							pos2 = CVector3(0.5f *m_width, 0.0f, m_height * j  *0.75f);
							cell.m_CenterPosition = pos2;
							grid.m_Cells.push_back(cell);
					}
				}
				else {
					pos2 = CVector3(0.0f, 0.0f, m_height* j);
				}
				for (int i = 0; i < m_column; i++) {
					if (m_pointy) {
						pos2 += CVector3(m_width, 0.0f, 0.0f);
					}
					else {
						if (i % 2 == 0) {
							pos2 += CVector3(m_width * 0.75f, 0.0f, 0.5f* m_height);
						}
						else {
							pos2 += CVector3(m_width * 0.75f, 0.0f, 0.5f* -m_height);
						}
					}
						cell.m_CenterPosition = pos2;
						grid.m_Cells.push_back(cell);
						MathHelper::Matrix4 modelMatrix2 = MathHelper::SimpleModelMatrixRotationTranslation((float)totalDegreesRotatedRadians, pos2);

						// Render pyramid in the first position, using the color shader
						getOpenGLRenderer()->renderObject(
							&m_texturedModelShaderId,
							&m_HexgridVertexArrayObject,
							&m_textureID,
							m_numFacesHexagon,
							color,
							&modelMatrix2,
							COpenGLRenderer::EPRIMITIVE_MODE::TRIANGLES,
							false
						);

						//// Render same pyramid (same vertex array object identifier), in a second position, but this time with a texture
						//getOpenGLRenderer()->renderObject(
						//	&m_texturedModelShaderId,
						//	&m_pyramidVertexArrayObject,
						//	&m_textureID,
						//	m_numFacesHexagon,
						//	color,
						//	&modelMatrix2,
						//	COpenGLRenderer::EPRIMITIVE_MODE::TRIANGLES,
						//	false
						//);
				}
			}
		}

		// =================================
	}
}

/* */
void CAppHexGrid::onMouseMove(float deltaX, float deltaY)
{
	// Update app-specific stuff when mouse moves here 
	// ===============================================
	//
	// ===============================================
	if (deltaX < 100.0f && deltaY < 100.0f)
	{
		float moveX = -deltaX * DEFAULT_CAMERA_MOVE_SPEED;
		float moveZ = -deltaY * DEFAULT_CAMERA_MOVE_SPEED;

		float currPos[3];
		m_objectPosition.getValues(currPos);
		currPos[0] += moveX;
		currPos[2] += moveZ;
		m_objectPosition.setValues(currPos);
	}
}

/* */
void CAppHexGrid::CreateGridGeometry() //TODO esto ya solo hace una fila * numero, ahora debemos de hacer que agarre por numero de columnas tambie
{
	bool loaded = false;

	 m_row = m_hexworld->getRows(); //4;   
	 m_column = m_hexworld->getCols();

	int num_vertices = 6; // 6 caras hexagono
	m_numFacesHexagon = 4;// 4 triangulos

	float v1[3], v2[3], v3[3], v1v2[3], v1v3[3], norm[3];
	m_size = m_hexworld->getSize();
	float CenterX = 0.0f;
	float CenterZ = 0.0f;
	m_pointy = false;
	//m_pointy = m_hexworld->isPointy();
	/*if (m_hexworld->isPointy() == "pointy") {
		m_pointy = true;
	}
	else {
		m_pointy = false;
	}*/

	float PI = 3.1415f;
	float * vData = new float[18];
	float angle_deg;
	float angle_rad;

	int vuelta = 0;
	if (m_pointy) {
		m_width = sqrtf(3) * m_size;

		m_height = 2.0f * m_size;
	}
	else {
		m_width = 2.0f * m_size;

		m_height = sqrtf(3) * m_size;
	}

	for (int i = 0; i < 18; i++) {
		if (m_pointy) {
			angle_deg = vuelta * 60 - 30;
		}
		else {
			angle_deg = vuelta * 60;
		}
		angle_rad = (PI/180) * angle_deg;

		vData[i] = CenterX + m_size * cosf(angle_rad);
		i++;

		vData[i] = 0.f;
		i++;
		
		vData[i] = CenterZ + m_size * sinf(angle_rad);

		vuelta++;
	}
 
	for (int i = 0; i < 18; i++) {
		cout << "vertices: " << i + 1 << endl;
		cout << vData[i] << endl;
		cout << vData[++i] << endl;
		cout << vData[++i] << endl;

	}

	unsigned short * tIndices = new unsigned short[12];
	if (m_pointy) {
		tIndices[0] = 4;
		tIndices[1] = 1;
		tIndices[2] = 3;

		//triangulo 2
		tIndices[3] = 4;
		tIndices[4] = 5;
		tIndices[5] = 0;

		//triangulo 3
		tIndices[6] = 3;
		tIndices[7] = 1;
		tIndices[8] = 2;

		//triangulo 4
		tIndices[9] = 1;
		tIndices[10] = 4;
		tIndices[11] = 0;
	}
	else {
		tIndices[0] = 4;

		tIndices[1] = 3;

		tIndices[2] = 2;


		//triangulo 2

		tIndices[3] = 5;

		tIndices[4] = 4;

		tIndices[5] = 2;



		//triangulo 3

		tIndices[6] = 5;

		tIndices[7] = 2;

		tIndices[8] = 1;


		//triangulo 4

		tIndices[9] = 5;

		tIndices[10] = 1;

		tIndices[11] = 0;
	}
	

	float  * vertexUVs = new float[12]{ 0 };
	float  * nData = new float[12]{ 0 };
	unsigned short * nIndices = new unsigned short[12]{ 0 };
	
	
	for (int i = 0; i < m_numFacesHexagon; i++)
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

		nData[i * 3] = norm[0];
		nData[(i * 3) + 1] = norm[1];
		nData[(i * 3) + 2] = norm[2];
	}

	// Allocate graphics memory for object
	loaded = getOpenGLRenderer()->allocateGraphicsMemoryForObject(
		&m_colorModelShaderId,
		&m_HexgridVertexArrayObject,
		vData,
		num_vertices,        // Numero de vertices, internamente el codigo multiplica sizeof(float) * numVertices * 3
		nData,
		m_numFacesHexagon,
		vertexUVs,
		num_vertices,
		tIndices,
		m_numFacesHexagon,        // Numero de indices, internamente el codigo multiplica sizeof(unsigned short) * numIndicesVert * 3
		nIndices,
		m_numFacesHexagon,
		tIndices,
		m_numFacesHexagon
	);

	if (!loaded)
	{
		m_numFacesHexagon = 0;

		if (m_HexgridVertexArrayObject > 0)
		{
			getOpenGLRenderer()->freeGraphicsMemoryForObject(&m_HexgridVertexArrayObject);
			m_HexgridVertexArrayObject = 0;
		}
	}
}

/* */
void CAppHexGrid::onF2(int mods)
{
}

/* */
void CAppHexGrid::onF3(int mods)
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
void CAppHexGrid::onArrowUp(int mods)
{
	getOpenGLRenderer()->moveCamera(0.3);
}
void CAppHexGrid::onArrowDown(int mods)
{
	getOpenGLRenderer()->moveCamera(-0.3);
}


/* */
void CAppHexGrid::normcrossprod(float v1[3], float v2[3], float out[3])
{
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];

	normalize(out);
}

/* */
void CAppHexGrid::normalize(float v[3])
{
	float d = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (d == 0.0f)
	{
		return;
	}
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}