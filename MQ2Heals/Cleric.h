#pragma once

#include "Healer.h"
#include "MQ2Heals.h"

class CCleric : public Healer
{
public:
	CCleric();
	~CCleric();
	bool WardSpellReady(string &HealSpell, float Range=0);
	bool YaulpSpellReady(string &HealSpell);

private:
};
