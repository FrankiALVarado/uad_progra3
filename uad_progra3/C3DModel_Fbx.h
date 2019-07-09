#pragma once

#include <string>

#include "../uad_progra3/Include/C3DModel.h"

class C3DModel_Fbx : public C3DModel
{
private:
	int m_Vertex, m_Normal, m_currentUV, m_aux = 0; //contadores y auxiliar usado en conteo de triangulos para el arreglo dinamico
	int m_BufferArray[4]{ 0 }; //arreglo usado para hacer triangulos de quads


	float *m_verticesRaw;                                             // Dynamically-allocated array of vertices (raw float values)
	float *m_normalsRaw;                                              // Dynamically-allocated array of normals (raw float values)
	float *m_uvCoordsRaw;                                             // Dynamically-allocated array of UV coords (raw float values)



public: 
	C3DModel_Fbx();
	~C3DModel_Fbx();
};

