#pragma once
#include "Dependencies/JSON/nlohmann/json.hpp"
class CHexWorld
{
public:
	CHexWorld();
	~CHexWorld();
	int getRows();
	int getCols();
	float getSize();
	bool isPointy();
};

