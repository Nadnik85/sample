#include "Magician.h"

CMagician::CMagician()
{
	SpearSpells.push_back("Spear of Molten Shieldstone");
	SpearSpells.push_back("Spear of Blistersteel");
	SpearSpells.push_back("Spear of Molten Steel");
	SpearSpells.push_back("Spear of Magma");

	BoltSpells.push_back("Meteoric Bolt");
	BoltSpells.push_back("Bolt of Molten Shieldstone");
	BoltSpells.push_back("Iron Bolt");
	BoltSpells.push_back("Bolt of Molten Magma");
	BoltSpells.push_back("Bolt of Molten Steel");
	BoltSpells.push_back("Bolt of Rhyolite");
	BoltSpells.push_back("Bolt of Molten Scoria");
	BoltSpells.push_back("Bolt of Molten Slag");
	BoltSpells.push_back("Bolt of Jerikor");
	BoltSpells.push_back("Firebolt of Tallon");
	BoltSpells.push_back("Lava Bolt");
	BoltSpells.push_back("Cinder Bolt");
	BoltSpells.push_back("Bolt of Flame");
	BoltSpells.push_back("Flame Bolt");

	ServantSpells.push_back("Remorseless Servant");
	ServantSpells.push_back("Relentless Servant");
	ServantSpells.push_back("Ruthless Servant");
	ServantSpells.push_back("Ruinous Servant");
	ServantSpells.push_back("Rumbling Servant");
	ServantSpells.push_back("Rancorous Servant");
	ServantSpells.push_back("Rampaging Servant");
	ServantSpells.push_back("Raging Servant");

	FireRainSpells.push_back("Coronal Rain");
	FireRainSpells.push_back("Rain of Blistersteel");
	FireRainSpells.push_back("Rain of Molten Steel");
	FireRainSpells.push_back("Rain of Rhyolite");
	FireRainSpells.push_back("Rain of Molten Scoria");
	FireRainSpells.push_back("Rain of Molten Dross");
	FireRainSpells.push_back("Rain of Molten Slag");
	FireRainSpells.push_back("Rain of Jerikor");
	FireRainSpells.push_back("Sun Storm");
	FireRainSpells.push_back("Rain of Lava");
	FireRainSpells.push_back("Rain of Fire");

	MagicRainSpells.push_back("Rain of Cutlasses");
	MagicRainSpells.push_back("Rain of Scythes");
	MagicRainSpells.push_back("Rain of Razors");
	MagicRainSpells.push_back("Rain of Daggers");
	MagicRainSpells.push_back("Deluge of Thunderbolts");
	MagicRainSpells.push_back("Torrent of Thunderbolts");
	MagicRainSpells.push_back("Rain of Thunderbolts");
	MagicRainSpells.push_back("Manastorm");
	MagicRainSpells.push_back("Rain of Swords");
	MagicRainSpells.push_back("Rain of Spikes");
	MagicRainSpells.push_back("Rain of Blades");

	FickleSpells.push_back("Fickle Conflagration");
	FickleSpells.push_back("Fickle Magma");
	FickleSpells.push_back("Fickle Flames");
	FickleSpells.push_back("Fickle Flare");
	FickleSpells.push_back("Fickle Blaze");
	FickleSpells.push_back("Fickle Pyroclasm");
	FickleSpells.push_back("Fickle Inferno");
	FickleSpells.push_back("Fickle Fire");

	ThaumatizeSpells.push_back("Thaumatize Pet");

	FreeNukeAAs.push_back("Virulent Talon");
	FreeNukeAAs.push_back("Force of Elements");
	FreeNukeAAs.push_back("Banestrike");

	ItemClickies.push_back("Balefire Gem");

	ItemClickies.push_back("Ether-Fused Shard");
	ItemClickies.push_back("Coronal Orb");
	ItemClickies.push_back("Summoned: Icebound Shard");
	ItemClickies.push_back("Summoned: Icebound Fragment");
	ItemClickies.push_back("Summoned: Icebound Sliver");
	ItemClickies.push_back("Blazing Orb");
	ItemClickies.push_back("Summoned: Frostrift Shard");
	ItemClickies.push_back("Summoned: Frostrift Fragment");
	ItemClickies.push_back("Summoned: Frostrift Sliver");
	ItemClickies.push_back("Summoned: Spectral Ice Shard");
	ItemClickies.push_back("Summoned: Spectral Ice Fragment");
	ItemClickies.push_back("Summoned: Spectral Ice Sliver");
	ItemClickies.push_back("Summoned: Elemental Ice Shard");
	ItemClickies.push_back("Summoned: Elemental Ice Fragment");
	ItemClickies.push_back("Summoned: Elemental Ice Sliver");
	ItemClickies.push_back("Void Shard");
	ItemClickies.push_back("Elemental Shard");
	ItemClickies.push_back("Molten Orb");
	ItemClickies.push_back("Lava Orb");

	DichotomicSpells.push_back("Dichotomic Companion");

	OfManySpells.push_back("Storm of Many");
	OfManySpells.push_back("Salvo of Many");
	OfManySpells.push_back("Strike of Many");
	OfManySpells.push_back("Clash of Many");
	OfManySpells.push_back("Jolt of Many");
	OfManySpells.push_back("Shock of Many");
}

bool CMagician::OfManySpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, OfManySpells, Distance);
}

bool CMagician::DichotomicSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, DichotomicSpells, Distance);
}

bool CMagician::NextItemClicky(string &Spell, float Distance)
{
	return ItemTypeReady(Spell, ItemClickies);
}

bool CMagician::FreeNukeReady(string &Spell, float Distance)
{
	return AATypeReady(Spell, FreeNukeAAs);
}

bool CMagician::ServantSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, ServantSpells, Distance);
}

bool CMagician::SpearSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, SpearSpells, Distance);
}

bool CMagician::BoltSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, BoltSpells, Distance);
}

bool CMagician::FickleSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, FickleSpells, Distance);
}

bool CMagician::FireRainSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, FireRainSpells, Distance);
}

bool CMagician::MagicRainSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, MagicRainSpells, Distance);
}

bool CMagician::ThaumatizeSpellReady(string &Spell, float Distance)
{
	return SpellTypeReady(Spell, ThaumatizeSpells, Distance);
}
