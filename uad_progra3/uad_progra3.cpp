// uad_progra3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <fstream>
#include <iomanip>

#include <iostream>
using namespace std;

#include "Dependencies/JSON/nlohmann/json.hpp"
using json = nlohmann::json;

#include "Include/CGameWindow.h"
#include "Include/CApp.h"
#include "Include/CAppCubeTest.h"
#include "Include/CAppObjLoader.h"
#include "Include/CAppGeometricFigures.h"
#include "Include/CAppEmpty.h"
#include "Include/Csphere.h"
#include "Cesfera.h"
#include "CAppHexGrid.h"

void testJSon();

//csphere es el icosaedro
int main()
{
	//testJSon();
	CApp *app = NULL;                  // Pointer to BASE class CApp
	app = new CAppHexGrid(800, 600);  // Using pointer to base class, create a new object of DERIVED class
	app->run();                        // Run the app
	delete app;                        // Delete pointer
	app = NULL;                        // Set pointer to NULL

	//cin.ignore(2);
	return 0;
}

void testJSon()
{
	auto j = json::parse("{ \"happy\": true, \"pi\": 3.141}");

	string s = j.dump();

	cout << j.dump() << endl;;

	//read a json file
	ifstream i("C:\\Users\\frank\\OneDrive\\Documentos\\hexgrid_cfg.json");
	json j2;
	i >> j2;

	cout << "JSON configuration for CAppHexgrid:" << endl;
	cout << j2.dump(4) << endl;
 

}
