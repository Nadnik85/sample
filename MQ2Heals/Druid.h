#pragma once

#include "..\MQ2Heals\Healer.h"

class CDruid : public Healer
{

public:
	CDruid();
	~CDruid();
	bool DichotomicNukeReady(string &NukeSpell, float Distance = 0);
	bool RemoteNukeReady(string &NukeSpell, float Distance = 0);
	bool StunNukeReady(string &NukeSpell, float Distance = 0);
	bool WrathNukeReady(string &NukeSpell, float Distance = 0);
	bool WinterNukeReady(string &NukeSpell, float Distance = 0);
	bool SkinDebuffReady(string &NukeSpell, float Distance = 0);
	bool FreeNukeReady(string &NukeSpell, float Distance = 0);

private:
	vector <string> GrowthSpells;
	vector <string> DichotomicSpells;
	vector <string> RemoteFireSpells;
	vector <string> StunNukeSpells;
	vector <string> FireWrathSpells;
	vector <string> WinterFireSpells;
	vector <string> SkinDebuffSpells;
	vector <string> FreeNukeSpells;
};