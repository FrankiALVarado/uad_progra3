#pragma once
#include <vector>
#include "CGridCell.h"
class CGrid
{
	//Vector de gridcells
public:
	CGrid();
	~CGrid();

	std::vector<CGridCell> m_Cells; //centros de las celdas
};

