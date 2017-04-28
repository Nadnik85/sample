#pragma once

#include "Healer.h"

class CShaman : public Healer
{

public:
	CShaman();
	~CShaman();

private:
	vector <string> GrowthSpells;
};