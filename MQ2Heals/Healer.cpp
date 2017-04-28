#include "Healer.h"



Healer::Healer()
{
}


Healer::~Healer()
{
}

bool Healer::BigHealReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, BigHeals, Distance);
}

bool Healer::HealNukeReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, HealNukes, Distance);
}

bool Healer::NukeHealReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, NukeHeals, Distance);
}

bool Healer::FastHealReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, FastHeals, Distance);
}

bool Healer::PatchHealReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, PatchHeals, Distance);
}

bool Healer::SlowHealReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, SlowHeals, Distance);
}

bool Healer::GroupHealReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, GroupHeals, Distance);
}

bool Healer::DelayedHealReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, DelayedHeals, Distance);
}

bool Healer::HotSpellReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, HotSpells, Distance);
}

bool Healer::GroupHotSpellReady(string &HealSpell, float Distance)
{
	return SpellTypeReady(HealSpell, GroupHotSpells, Distance);
}