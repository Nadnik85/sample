#include "Beastlord.h"

CBeastlord::CBeastlord()
{
	BiteSpells.push_back("Kreig's Bite");
	BiteSpells.push_back("Poantaar's Bite");
	BiteSpells.push_back("Rotsil's Bite");
	BiteSpells.push_back("Sarsez' Bite");
	BiteSpells.push_back("Bite of the Vitrik");
	BiteSpells.push_back("Bite of the Borrower");
	BiteSpells.push_back("Bite of the Empress");
	BiteSpells.push_back("Diregriffon's Bite");

	FeralgiaSpells.push_back("Krenk's Feralgia");
	FeralgiaSpells.push_back("Kesar's Feralgia");
	FeralgiaSpells.push_back("Yahnoa's Feralgia");
	FeralgiaSpells.push_back("Tuzil's Feralgia");
	FeralgiaSpells.push_back("Haergen's Feralgia");

	MaelstromSpells.push_back("Visoracius' Maelstrom");
	MaelstromSpells.push_back("Nak's Maelstrom");
	MaelstromSpells.push_back("Bale's Maelstrom");
	MaelstromSpells.push_back("Kron's Maelstrom");

	LanceSpells.push_back("Kromtus Lance");
	LanceSpells.push_back("Kromrif Lance");
	LanceSpells.push_back("Frostrif Lance");
	LanceSpells.push_back("Glacial Lance");
	LanceSpells.push_back("Frigid Lance");

	FrozenSpells.push_back("Frozen Miasma");
	FrozenSpells.push_back("Frozen Carbomate");
	FrozenSpells.push_back("Frozen Cyanin");
	FrozenSpells.push_back("Frozen Venin");
	FrozenSpells.push_back("Frozen Venom");
}

CBeastlord::~CBeastlord()
{
}

bool CBeastlord::BiteSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, BiteSpells, Distance);
}

bool CBeastlord::FeralgiaSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, FeralgiaSpells, Distance);
}

bool CBeastlord::MaelstromSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, MaelstromSpells, Distance);
}

bool CBeastlord::LanceSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, LanceSpells, Distance);
}

bool CBeastlord::FrozenSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, FrozenSpells, Distance);
}