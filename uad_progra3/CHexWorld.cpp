#include "CHexWorld.h"
#include <fstream>
using std::ifstream;
using json = nlohmann::json;


CHexWorld::CHexWorld()
{
}


CHexWorld::~CHexWorld()
{
}

int CHexWorld::getRows()
{
	ifstream file("C:\\Users\\frank\\OneDrive\\Documentos\\hexgrid_cfg.json");
	json j;
	j << file;
	return j["HexGrid"]["numRows"];
}

int CHexWorld::getCols()
{
	ifstream file("C:\\Users\\frank\\OneDrive\\Documentos\\hexgrid_cfg.json");
	json j;
	j << file;
	return j["HexGrid"]["numCols"];
}

float CHexWorld::getSize()
{
	ifstream file("C:\\Users\\frank\\OneDrive\\Documentos\\hexgrid_cfg.json");
	json j;
	j << file;
	return j["HexGrid"]["cellSize"];
}

bool  CHexWorld::isPointy()
{
	ifstream file("C:\\Users\\frank\\OneDrive\\Documentos\\hexgrid_cfg.json");
	json j;
	j << file;
	if (j["HexGrid"]["orientation"] == "pointy") {
		return true;
	}
	else {
		return false;
	}
}
		
