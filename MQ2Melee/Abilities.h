#define DECLARE_ABILITY_OPTION( __var, __key, __help, __default, __show) char* __var[] = {\
                                                                                  __key, \
                                                                                  __help, \
                                                                                  __default, \
                                                                                  __show, \
                                                                                  };

#define REGISTER_ABILITY_OPTION( __var, __func, __ability ) MapInsert(&CmdListe, Option(__var[0], __var[1], __var[2], __var[3], __func, __ability));

DECLARE_ABILITY_OPTION(pDEBUG, "debug", "[ON/OFF]?", "0", "");

DECLARE_ABILITY_OPTION(pAGGRO, "aggro", "[ON/OFF]?", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD]},1,0]}", "${If[${meleemvi[plugin]},1,0]}");

DECLARE_ABILITY_OPTION(pAGGRP, "aggropri", "[ID] Primary (Aggro)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[aggro]},1,0]}");

DECLARE_ABILITY_OPTION(pAGGRS, "aggrosec", "[ID] Offhand (Aggro)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[aggro]} && ${meleemvi[aggropri]},1,0]}");

DECLARE_ABILITY_OPTION(pARROW, "arrow", "[ID] item?", "0", "${If[${meleemvi[plugin]} && (${Me.Skill[archery]} || ${Me.Skill[throwing]}),1,0]}");

DECLARE_ABILITY_OPTION(pASSAS, "assassinate", "Sneak/Hide/Behind/Strike/Stab [ON/OFF]?", "${If[${Me.Skill[backstab]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[backstab]},1,0]}");

DECLARE_ABILITY_OPTION(pASSLT, "assault", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Assault]} || ${Me.CombatAbility[Assault Rk. II]} || ${Me.CombatAbility[Assault Rk. III]} || ${Me.CombatAbility[Battery]} || ${Me.CombatAbility[Battery Rk. II]} || ${Me.CombatAbility[Battery Rk. III]} || ${Me.CombatAbility[Onslaught]} || ${Me.CombatAbility[Onslaught Rk. II]} || ${Me.CombatAbility[Onslaught Rk. III]} || ${Me.CombatAbility[Incursion]} || ${Me.CombatAbility[Incursion Rk. II]} || ${Me.CombatAbility[Incursion Rk. III]},60,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Assault]} || ${Me.CombatAbility[Assault Rk. II]} || ${Me.CombatAbility[Assault Rk. III]} || ${Me.CombatAbility[Battery]} || ${Me.CombatAbility[Battery Rk. II]} || ${Me.CombatAbility[Battery Rk. III]} || ${Me.CombatAbility[Onslaught]} || ${Me.CombatAbility[Onslaught Rk. II]} || ${Me.CombatAbility[Onslaught Rk. III]} || ${Me.CombatAbility[Incursion]} || ${Me.CombatAbility[Incursion Rk. II]} || ${Me.CombatAbility[Incursion Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pDEFEN, "defense", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Bracing Defense]} || ${Me.CombatAbility[Bracing Defense Rk. II]} || ${Me.CombatAbility[Bracing Defense Rk. III]} || ${Me.CombatAbility[Staunch Defense]} || ${Me.CombatAbility[Staunch Defense Rk. II]} || ${Me.CombatAbility[Staunch Defense Rk. III]} || ${Me.CombatAbility[Stalwart Defense]} || ${Me.CombatAbility[Stalwart Defense Rk. II]} || ${Me.CombatAbility[Stalwart Defense Rk. III]} || ${Me.CombatAbility[Steadfast Defense]} || ${Me.CombatAbility[Steadfast Defense Rk. II]} || ${Me.CombatAbility[Steadfast Defense Rk. III]} || ${Me.CombatAbility[Stout Defense]} || ${Me.CombatAbility[Stout Defense Rk. II]} || ${Me.CombatAbility[Stout Defense Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Bracing Defense]} || ${Me.CombatAbility[Bracing Defense Rk. II]} || ${Me.CombatAbility[Bracing Defense Rk. III]} || ${Me.CombatAbility[Staunch Defense]} || ${Me.CombatAbility[Staunch Defense Rk. II]} || ${Me.CombatAbility[Staunch Defense Rk. III]} || ${Me.CombatAbility[Stalwart Defense]} || ${Me.CombatAbility[Stalwart Defense Rk. II]} || ${Me.CombatAbility[Stalwart Defense Rk. III]} || ${Me.CombatAbility[Steadfast Defense]} || ${Me.CombatAbility[Steadfast Defense Rk. II]} || ${Me.CombatAbility[Steadfast Defense Rk. III]} || ${Me.CombatAbility[Stout Defense]} || ${Me.CombatAbility[Stout Defense Rk. II]} || ${Me.CombatAbility[Stout Defense Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pBKOFF, "backoff", "[#] Life% Below? 0=0ff", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && !${meleemvi[aggro]},1,0]}");

DECLARE_ABILITY_OPTION(pBSTAB, "backstab", "[ON/OFF]?", "${If[${Me.Skill[backstab]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[backstab]},1,0]}");

DECLARE_ABILITY_OPTION(pBTASP, "asp", "[ON/OFF]?", "${If[${Me.AltAbility[bite of the asp]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[bite of the asp]},1,0]}");

DECLARE_ABILITY_OPTION(pBVIVI, "bvivi", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Bestial Vivisection]} || ${Me.CombatAbility[Bestial Vivisection Rk. II]} || ${Me.CombatAbility[Bestial Vivisection Rk. III]} || ${Me.CombatAbility[Bestial Rending]} || ${Me.CombatAbility[Bestial Rending Rk. II]} || ${Me.CombatAbility[Bestial Rending Rk. III]} || ${Me.CombatAbility[Bestial Evulsing]} || ${Me.CombatAbility[Bestial Evulsing Rk. II]} || ${Me.CombatAbility[Bestial Evulsing Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Bestial Vivisection]} || ${Me.CombatAbility[Bestial Vivisection Rk. II]} || ${Me.CombatAbility[Bestial Vivisection Rk. III]} || ${Me.CombatAbility[Bestial Rending]} || ${Me.CombatAbility[Bestial Rending Rk. II]} || ${Me.CombatAbility[Bestial Rending Rk. III]} || ${Me.CombatAbility[Bestial Evulsing]} || ${Me.CombatAbility[Bestial Evulsing Rk. II]} || ${Me.CombatAbility[Bestial Evulsing Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pBLEED, "bleed", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Bleed]} || ${Me.CombatAbility[Bleed Rk. II]} || ${Me.CombatAbility[Bleed Rk. III]} || ${Me.CombatAbility[Wound]} || ${Me.CombatAbility[Wound Rk. II]} || ${Me.CombatAbility[Wound Rk. III]} || ${Me.CombatAbility[Lacerate]} || ${Me.CombatAbility[Lacerate Rk. II]} || ${Me.CombatAbility[Lacerate Rk. III]} || ${Me.CombatAbility[Gash]} || ${Me.CombatAbility[Gash Rk. II]} || ${Me.CombatAbility[Gash Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Bleed]} || ${Me.CombatAbility[Bleed Rk. II]} || ${Me.CombatAbility[Bleed Rk. III]}|| ${Me.CombatAbility[Wound]} || ${Me.CombatAbility[Wound Rk. II]} || ${Me.CombatAbility[Wound Rk. III]} || ${Me.CombatAbility[Lacerate]} || ${Me.CombatAbility[Lacerate Rk. II]} || ${Me.CombatAbility[Lacerate Rk. III]} || ${Me.CombatAbility[Gash]} || ${Me.CombatAbility[Gash Rk. II]} || ${Me.CombatAbility[Gash Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pBLUST, "bloodlust", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Shared Bloodlust]} || ${Me.CombatAbility[Shared Bloodlust Rk. II]} || ${Me.CombatAbility[Shared Bloodlust Rk. III]} || ${Me.CombatAbility[Shared Brutality]} || ${Me.CombatAbility[Shared Brutality Rk. II]} || ${Me.CombatAbility[Shared Brutality Rk. III]} || ${Me.CombatAbility[Shared Savagery]} || ${Me.CombatAbility[Shared Savagery Rk. II]} || ${Me.CombatAbility[Shared Savagery Rk. III]} || ${Me.CombatAbility[Shared Viciousness]} || ${Me.CombatAbility[Shared Viciousness Rk. II]} || ${Me.CombatAbility[Shared Viciousness Rk. III]} || ${Me.CombatAbility[Shared Cruelty]} || ${Me.CombatAbility[Shared Cruelty Rk. II]} || ${Me.CombatAbility[Shared Cruelty Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Shared Bloodlust]} || ${Me.CombatAbility[Shared Bloodlust Rk. II]} || ${Me.CombatAbility[Shared Bloodlust Rk. III]} || ${Me.CombatAbility[Shared Brutality]} || ${Me.CombatAbility[Shared Brutality Rk. II]} || ${Me.CombatAbility[Shared Brutality Rk. III]} || ${Me.CombatAbility[Shared Savagery]} || ${Me.CombatAbility[Shared Savagery Rk. II]} || ${Me.CombatAbility[Shared Savagery Rk. III]} || ${Me.CombatAbility[Shared Viciousness]} || ${Me.CombatAbility[Shared Viciousness Rk. II]} || ${Me.CombatAbility[Shared Viciousness Rk. III]} || ${Me.CombatAbility[Shared Cruelty]} || ${Me.CombatAbility[Shared Cruelty Rk. II]} || ${Me.CombatAbility[Shared Cruelty Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pBBLOW, "boastful", "[ON/OFF]?", "${If[${Me.AltAbility[boastful bellow]},0,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[boastful bellow]},1,0]}");

DECLARE_ABILITY_OPTION(pBASHS, "bash", "[#] Bash 0=0ff", "${If[${Me.Skill[bash]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[bash]},1,0]}");

DECLARE_ABILITY_OPTION(pBGING, "begging", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[begging]},1,0]}");

DECLARE_ABILITY_OPTION(pBOWID, "bow", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[archery]},1,0]}");

DECLARE_ABILITY_OPTION(pCALLC, "callchallenge", "[ON/OFF]?", "${If[${Me.AltAbility[call of challenge]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[call of challenge]},1,0]}");

DECLARE_ABILITY_OPTION(pCFIST, "cloud", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Cloud of Fists]} || ${Me.CombatAbility[Cloud of Fists Rk. II]} || ${Me.CombatAbility[Cloud of Fists Rk. III]} || ${Me.CombatAbility[Phantom Partisan]} || ${Me.CombatAbility[Phantom Partisan Rk. II]} || ${Me.CombatAbility[Phantom Partisan Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Cloud of Fists]} || ${Me.CombatAbility[Cloud of Fists Rk. II]} || ${Me.CombatAbility[Cloud of Fists Rk. III]} || ${Me.CombatAbility[Phantom Partisan]} || ${Me.CombatAbility[Phantom Partisan Rk. II]} || ${Me.CombatAbility[Phantom Partisan Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pCHAMS, "cstrike", "[ON/OFF]?", "${If[${Me.AltAbility[Chameleon Strike]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Chameleon Strike]},1,0]}");

DECLARE_ABILITY_OPTION(pCRIPS, "cripple", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[leg strike]} || ${Me.CombatAbility[leg cut]} || ${Me.CombatAbility[leg slice]} || ${Me.CombatAbility[crippling strike]} || ${Me.CombatAbility[tendon cleave]} || ${Me.CombatAbility[tendon cleave rk. ii]} || ${Me.CombatAbility[tendon cleave rk. iii]} || ${Me.CombatAbility[tendon sever]} || ${Me.CombatAbility[tendon sever rk. ii]} || ${Me.CombatAbility[tendon sever rk. iii]} || ${Me.CombatAbility[tendon shear]} || ${Me.CombatAbility[tendon shear rk. ii]} || ${Me.CombatAbility[tendon shear rk. iii]} || ${Me.CombatAbility[tendon lacerate]} || ${Me.CombatAbility[tendon lacerate rk. ii]} || ${Me.CombatAbility[tendon lacerate rk. iii]} || ${Me.CombatAbility[tendon Slash]} || ${Me.CombatAbility[tendon Slash rk. ii]} || ${Me.CombatAbility[tendon Slash rk. iii]} || ${Me.CombatAbility[Tendon Gash]} || ${Me.CombatAbility[Tendon Gash Rk. II]} || ${Me.CombatAbility[Tendon Gash Rk. III]} || ${Me.CombatAbility[Tendon Tear]} || ${Me.CombatAbility[Tendon Tear Rk. II]} || ${Me.CombatAbility[Tendon Tear Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[leg strike]} || ${Me.CombatAbility[leg cut]} || ${Me.CombatAbility[leg slice]} || ${Me.CombatAbility[crippling strike]} || ${Me.CombatAbility[tendon cleave]} || ${Me.CombatAbility[tendon cleave rk. ii]} || ${Me.CombatAbility[tendon cleave rk. iii]} || ${Me.CombatAbility[tendon sever]} || ${Me.CombatAbility[tendon sever rk. ii]} || ${Me.CombatAbility[tendon sever rk. iii]} || ${Me.CombatAbility[tendon shear]} || ${Me.CombatAbility[tendon shear rk. ii]} || ${Me.CombatAbility[tendon shear rk. iii]} || ${Me.CombatAbility[tendon lacerate]} || ${Me.CombatAbility[tendon lacerate rk. ii]} || ${Me.CombatAbility[tendon lacerate rk. iii]} || ${Me.CombatAbility[tendon Slash]} || ${Me.CombatAbility[tendon Slash rk. ii]} || ${Me.CombatAbility[tendon Slash rk. iii]} || ${Me.CombatAbility[Tendon Gash]} || ${Me.CombatAbility[Tendon Gash Rk. II]} || ${Me.CombatAbility[Tendon Gash Rk. III]} || ${Me.CombatAbility[Tendon Tear]} || ${Me.CombatAbility[Tendon Tear Rk. II]} || ${Me.CombatAbility[Tendon Tear Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pGTPUN, "gutpunch", "[ON/OFF]?", "${If[${Me.AltAbility[gut punch]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[gut punch]},1,0]}");

DECLARE_ABILITY_OPTION(pOFREN, "opfrenzy", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Overpowering Frenzy]} || ${Me.CombatAbility[Overpowering Frenzy Rk. II]} || ${Me.CombatAbility[Overpowering Frenzy Rk. III]} || ${Me.CombatAbility[Overwhelming Frenzy]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. II]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. III]} || ${Me.CombatAbility[Conquering Frenzy]} || ${Me.CombatAbility[Conquering Frenzy Rk. II]} || ${Me.CombatAbility[Conquering Frenzy Rk. III]} || ${Me.CombatAbility[Vanquishing Frenzy]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. II]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. III]} || ${Me.CombatAbility[Demolishing Frenzy]} || ${Me.CombatAbility[Demolishing Frenzy Rk. II]} || ${Me.CombatAbility[Demolishing Frenzy Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Overpowering Frenzy]} || ${Me.CombatAbility[Overpowering Frenzy Rk. II]} || ${Me.CombatAbility[Overpowering Frenzy Rk. III]} || ${Me.CombatAbility[Overwhelming Frenzy]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. II]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. III]} || ${Me.CombatAbility[Conquering Frenzy]} || ${Me.CombatAbility[Conquering Frenzy Rk. II]} || ${Me.CombatAbility[Conquering Frenzy Rk. III]} || ${Me.CombatAbility[Vanquishing Frenzy]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. II]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. III]} || ${Me.CombatAbility[Demolishing Frenzy]} || ${Me.CombatAbility[Demolishing Frenzy Rk. II]} || ${Me.CombatAbility[Demolishing Frenzy Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pOSTRK, "opportunisticstrike", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Opportunistic Strike]} || ${Me.CombatAbility[Opportunistic Strike Rk. II]} || ${Me.CombatAbility[Opportunistic Strike Rk. III]} || ${Me.CombatAbility[Strategic Strike]} || ${Me.CombatAbility[Strategic Strike Rk. II]} || ${Me.CombatAbility[Strategic Strike Rk. III]} || ${Me.CombatAbility[Vital Strike]} || ${Me.CombatAbility[Vital Strike Rk. II]} || ${Me.CombatAbility[Vital Strike Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Opportunistic Strike]} || ${Me.CombatAbility[Opportunistic Strike Rk. II]} || ${Me.CombatAbility[Opportunistic Strike Rk. III]} || ${Me.CombatAbility[Strategic Strike]} || ${Me.CombatAbility[Strategic Strike Rk. II]} || ${Me.CombatAbility[Strategic Strike Rk. III]} || ${Me.CombatAbility[Vital Strike]} || ${Me.CombatAbility[Vital Strike Rk. II]} || ${Me.CombatAbility[Vital Strike Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pRALLO, "rallos", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Axe of Rallos]} || ${Me.CombatAbility[Axe of Rallos Rk. II]} || ${Me.CombatAbility[Axe of Rallos Rk. III]} || ${Me.CombatAbility[Axe of Graster]} || ${Me.CombatAbility[Axe of Graster Rk. II]} || ${Me.CombatAbility[Axe of Graster Rk. III]} || ${Me.CombatAbility[Axe of Illdaera]} || ${Me.CombatAbility[Axe of Illdaera Rk. II]} || ${Me.CombatAbility[Axe of Illdaera Rk. III]} || ${Me.CombatAbility[Axe of Zurel]} || ${Me.CombatAbility[Axe of Zurel Rk. II]} || ${Me.CombatAbility[Axe of Zurel Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Axe of Rallos]} || ${Me.CombatAbility[Axe of Rallos Rk. II]} || ${Me.CombatAbility[Axe of Rallos Rk. III]} || ${Me.CombatAbility[Axe of Graster]} || ${Me.CombatAbility[Axe of Graster Rk. II]} || ${Me.CombatAbility[Axe of Graster Rk. III]} || ${Me.CombatAbility[Axe of Illdaera]} || ${Me.CombatAbility[Axe of Illdaera Rk. II]} || ${Me.CombatAbility[Axe of Illdaera Rk. III]} ${Me.CombatAbility[Axe of Illdaera Rk. III]} || ${Me.CombatAbility[Axe of Zurel]} || ${Me.CombatAbility[Axe of Zurel Rk. II]} || ${Me.CombatAbility[Axe of Zurel Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pSYNGY, "synergy", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Calanin's Synergy]} || ${Me.CombatAbility[Calanin's Synergy Rk. II]} || ${Me.CombatAbility[Calanin's Synergy Rk. III]} || ${Me.CombatAbility[Dreamwalker's Synergy]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. II]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. III]} || ${Me.CombatAbility[Veilwalker's Synergy]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. II]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. III]} || ${Me.CombatAbility[Shadewalker's Synergy]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. II]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. III]} || ${Me.CombatAbility[Doomwalker's Synergy]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. II]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Calanin's Synergy]} || ${Me.CombatAbility[Calanin's Synergy Rk. II]} || ${Me.CombatAbility[Calanin's Synergy Rk. III]} || ${Me.CombatAbility[Dreamwalker's Synergy]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. II]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. III]} || ${Me.CombatAbility[Veilwalker's Synergy]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. II]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. III]} || ${Me.CombatAbility[Shadewalker's Synergy]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. II]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. III]} || ${Me.CombatAbility[Doomwalker's Synergy]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. II]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pTTJAB, "throatjab", "[ON/OFF]?", "${If[${Me.AltAbility[throat jab]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[throat jab]},1,0]}");

DECLARE_ABILITY_OPTION(pBTLLP, "battleleap", "[ON/OFF]?", "${If[${Me.AltAbility[Battle Leap]},0,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Battle Leap]},1,0]}");

DECLARE_ABILITY_OPTION(pCHFOR, "challengefor", "[ON/OFF]?", "${If[${Me.Book[challenge for honor]} || ${Me.Book[challenge for honor rk. ii]} || ${Me.Book[challenge for honor rk. iii]} || ${Me.Book[trial for honor]} || ${Me.Book[trial for honor rk. ii]} || ${Me.Book[trial for honor rk. iii]} || ${Me.Book[charge for honor]} || ${Me.Book[charge for honor rk. ii]} || ${Me.Book[charge for honor rk. iii]} || ${Me.Book[challenge for power]} || ${Me.Book[challenge for power rk. ii]} || ${Me.Book[challenge for power rk. iii]} || ${Me.Book[trial for power]} || ${Me.Book[trial for power rk. ii]} || ${Me.Book[trial for power rk. iii]} || ${Me.Book[charge for honor]} || ${Me.Book[charge for honor rk. ii]} || ${Me.Book[charge for honor rk. iii]} || ${Me.Book[confrontation for power]} || ${Me.Book[confrontation for power rk. ii]} || ${Me.Book[confrontation for power rk. iii]} || ${Me.Book[confrontation for honor]} || ${Me.Book[confrontation for honor rk. ii]} || ${Me.Book[confrontation for honor rk. iii]} || ${Me.Book[Provocation for honor]} || ${Me.Book[Provocation for honor rk. ii]} || ${Me.Book[Provocation for honor rk. iii]} || ${Me.Book[Provocation for power]} || ${Me.Book[Provocation for power rk. ii]} || ${Me.Book[Provocation for power rk. iii]} || ${Me.Book[Demand for Power]} || ${Me.Book[Demand for Power rk. ii]} || ${Me.Book[Demand for Power rk. iii]} || ${Me.Book[Demand for Honor]} || ${Me.Book[Demand for Honor rk. ii]} || ${Me.Book[Demand for Honor rk. iii]} || ${Me.Book[Impose for Power]} || ${Me.Book[Impose for Power rk. ii]} ||${Me.Book[Impose for Power rk. iii]} || ${Me.Book[Impose for Honor]} || ${Me.Book[Impose for Honor rk. ii]} ||${Me.Book[Impose for Honor rk. iii]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && (${Me.Book[challenge for honor]} || ${Me.Book[challenge for honor rk. ii]} || ${Me.Book[challenge for honor rk. iii]} || ${Me.Book[trial for honor]} || ${Me.Book[trial for honor rk. ii]} || ${Me.Book[trial for honor rk. iii]} || ${Me.Book[charge for honor]} || ${Me.Book[charge for honor rk. ii]} || ${Me.Book[charge for honor rk. iii]} || ${Me.Book[challenge for power]} || ${Me.Book[challenge for power rk. ii]} || ${Me.Book[challenge for power rk. iii]} || ${Me.Book[trial for power]} || ${Me.Book[trial for power rk. ii]} || ${Me.Book[trial for power rk. iii]} || ${Me.Book[charge for honor]} || ${Me.Book[charge for honor rk. ii]} || ${Me.Book[charge for honor rk. iii]} || ${Me.Book[confrontation for power]} || ${Me.Book[confrontation for power rk. ii]} || ${Me.Book[confrontation for power rk. iii]} || ${Me.Book[confrontation for honor]} || ${Me.Book[confrontation for honor rk. ii]} || ${Me.Book[confrontation for honor rk. iii]} || ${Me.Book[Provocation for honor]} || ${Me.Book[Provocation for honor rk. ii]} || ${Me.Book[Provocation for honor rk. iii]} || ${Me.Book[Provocation for power]} || ${Me.Book[Provocation for power rk. ii]} || ${Me.Book[Provocation for power rk. iii]} || ${Me.Book[Demand for Power]} || ${Me.Book[Demand for Power rk. ii]} || ${Me.Book[Demand for Power rk. iii]} || ${Me.Book[Demand for Honor]} || ${Me.Book[Demand for Honor rk. ii]} || ${Me.Book[Demand for Honor rk. iii]} || ${Me.Book[Impose for Power]} || ${Me.Book[Impose for Power rk. ii]} ||${Me.Book[Impose for Power rk. iii]} || ${Me.Book[Impose for Honor]} || ${Me.Book[Impose for Honor rk. ii]} ||${Me.Book[Impose for Honor rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pCOMMG, "commanding", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[commanding voice]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[commanding voice]},1,0]}");

DECLARE_ABILITY_OPTION(pCRYHC, "cryhavoc", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[cry havoc]} || ${Me.CombatAbility[Cry Carnage]} || ${Me.CombatAbility[Cry Carnage Rk. II]} || ${Me.CombatAbility[Cry Carnage Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[cry havoc]} || ${Me.CombatAbility[Cry Carnage]} || ${Me.CombatAbility[Cry Carnage Rk. II]} || ${Me.CombatAbility[Cry Carnage Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pDISRM, "disarm", "[ON/OFF]?", "${If[${Me.Skill[disarm]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[disarm]},1,0]}");

DECLARE_ABILITY_OPTION(pDMONK, "monkey", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[Drunken Monkey Style]} || ${Me.CombatAbility[Drunken Monkey Style rk. ii]} || ${Me.CombatAbility[Drunken Monkey Style rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Drunken Monkey Style]} || ${Me.CombatAbility[Drunken Monkey Style rk. ii]} || ${Me.CombatAbility[Drunken Monkey Style rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pDWNF0, "downflag0", "[ON/OFF] downflag0?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit0].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF1, "downflag1", "[ON/OFF] downflag1?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit1].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF2, "downflag2", "[ON/OFF] downflag2?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit2].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF3, "downflag3", "[ON/OFF] downflag3?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit3].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF4, "downflag4", "[ON/OFF] downflag4?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit4].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF5, "downflag5", "[ON/OFF] downflag5?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit5].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF6, "downflag6", "[ON/OFF] downflag6?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit6].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF7, "downflag7", "[ON/OFF] downflag7?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit7].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF8, "downflag8", "[ON/OFF] downflag8?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit8].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF9, "downflag9", "[ON/OFF] downflag9?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit9].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF10, "downflag10", "[ON/OFF] downflag10?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit10].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF11, "downflag11", "[ON/OFF] downflag11?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit11].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF12, "downflag12", "[ON/OFF] downflag12?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit12].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF13, "downflag13", "[ON/OFF] downflag13?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit13].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF14, "downflag14", "[ON/OFF] downflag14?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit14].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF15, "downflag15", "[ON/OFF] downflag15?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit15].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF16, "downflag16", "[ON/OFF] downflag16?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit16].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF17, "downflag17", "[ON/OFF] downflag17?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit17].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF18, "downflag18", "[ON/OFF] downflag18?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit18].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF19, "downflag19", "[ON/OFF] downflag19?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit19].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF20, "downflag20", "[ON/OFF] downflag20?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit20].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF21, "downflag21", "[ON/OFF] downflag21?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit21].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF22, "downflag22", "[ON/OFF] downflag22?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit22].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF23, "downflag23", "[ON/OFF] downflag23?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit23].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF24, "downflag24", "[ON/OFF] downflag24?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit24].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF25, "downflag25", "[ON/OFF] downflag25?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit25].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF26, "downflag26", "[ON/OFF] downflag26?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit26].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF27, "downflag27", "[ON/OFF] downflag27?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit27].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF28, "downflag28", "[ON/OFF] downflag28?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit28].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF29, "downflag29", "[ON/OFF] downflag29?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit29].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF30, "downflag30", "[ON/OFF] downflag30?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit30].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF31, "downflag31", "[ON/OFF] downflag31?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit31].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF32, "downflag32", "[ON/OFF] downflag32?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit32].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF33, "downflag33", "[ON/OFF] downflag33?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit33].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF34, "downflag34", "[ON/OFF] downflag34?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit34].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF35, "downflag35", "[ON/OFF] downflag35?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit35].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF36, "downflag36", "[ON/OFF] downflag36?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit36].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF37, "downflag37", "[ON/OFF] downflag37?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit37].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF38, "downflag38", "[ON/OFF] downflag38?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit38].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF39, "downflag39", "[ON/OFF] downflag39?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit39].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF40, "downflag40", "[ON/OFF] downflag40?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit40].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF41, "downflag41", "[ON/OFF] downflag41?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit41].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF42, "downflag42", "[ON/OFF] downflag42?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit42].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF43, "downflag43", "[ON/OFF] downflag43?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit43].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF44, "downflag44", "[ON/OFF] downflag44?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit44].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF45, "downflag45", "[ON/OFF] downflag45?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit45].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF46, "downflag46", "[ON/OFF] downflag46?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit46].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF47, "downflag47", "[ON/OFF] downflag47?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit47].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF48, "downflag48", "[ON/OFF] downflag48?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit48].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF49, "downflag49", "[ON/OFF] downflag49?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit49].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF50, "downflag50", "[ON/OFF] downflag50?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit50].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF51, "downflag51", "[ON/OFF] downflag51?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit51].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF52, "downflag52", "[ON/OFF] downflag52?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit52].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF53, "downflag53", "[ON/OFF] downflag53?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit53].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF54, "downflag54", "[ON/OFF] downflag54?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit54].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF55, "downflag55", "[ON/OFF] downflag55?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit55].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF56, "downflag56", "[ON/OFF] downflag56?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit56].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF57, "downflag57", "[ON/OFF] downflag57?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit57].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF58, "downflag58", "[ON/OFF] downflag58?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit58].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF59, "downflag59", "[ON/OFF] downflag59?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit59].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDWNF60, "downflag60", "[ON/OFF] downflag60?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit60].Length},1,0]}");

DECLARE_ABILITY_OPTION(pDRPNC, "dragonpunch", "[ON/OFF]?", "${If[${Me.Skill[dragon punch]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[dragon punch]},1,0]}");

DECLARE_ABILITY_OPTION(pEAGLE, "eaglestrike", "[ON/OFF]?", "${If[${Me.Skill[eagle strike]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[eagle strike]},1,0]}");

DECLARE_ABILITY_OPTION(pERAGE, "enrage", "[ON/OFF]?", "1", "${If[${meleemvi[plugin]},1,0]}");

DECLARE_ABILITY_OPTION(pESCAP, "escape", "[#] Life% Below? 0=0ff", "${If[${Me.AltAbility[escape]},20,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Me.AltAbility[escape]},1,0]}");

DECLARE_ABILITY_OPTION(pERKCK, "enragingkick", "[#] Life% Below? 0=0ff", "${If[${Me.CombatAbility[Enraging Crescent Kicks]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. II]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. III]} || ${Me.CombatAbility[Enraging Heel Kicks]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. II]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. III]} || ${Me.CombatAbility[Enraging Cut Kicks]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. II]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. III]} ,20,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && (${Me.CombatAbility[Enraging Crescent Kicks]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. II]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. III]} || ${Me.CombatAbility[Enraging Heel Kicks]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. II]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. III]})|| ${Me.CombatAbility[Enraging Cut Kicks]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. II]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. III]},1,0]}");

DECLARE_ABILITY_OPTION(pEVADE, "evade", "[#] [ON/OFF]?", "${If[${Me.Skill[hide]} && ${Me.Class.ShortName.Equal[ROG]},1,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Me.Skill[hide]} && ${Me.Class.ShortName.Equal[ROG]},1,0]}");

DECLARE_ABILITY_OPTION(pEYEGO, "eyegouge", "[ON/OFF]?", "${If[${Me.AltAbility[eye gouge]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[eye gouge]},1,0]}");

DECLARE_ABILITY_OPTION(pFEIGN, "feigndeath", "[#] Life% Below? 0=0ff", "${If[${Select[${Me.Class.ShortName},BST,SHD,NEC,MNK]},30,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},BST,SHD,NEC,MNK]},1,0]}");

DECLARE_ABILITY_OPTION(pFACES, "facing", "[ON/OFF] Face Target (Range)?", "1", "${If[${meleemvi[plugin]} && ${meleemvi[range]},1,0]}");

DECLARE_ABILITY_OPTION(pFALLS, "falls", "[ON/OFF] Auto-Feign?", "0", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Me.Class.ShortName.Equal[MNK]},1,0]}");

DECLARE_ABILITY_OPTION(pFERAL, "feralswipe", "[ON/OFF]?", "${If[${Me.AltAbility[feral swipe]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[feral swipe]},1,0]}");

DECLARE_ABILITY_OPTION(pFIELD, "fieldarm", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Field Armorer]} || ${Me.CombatAbility[Field Armorer Rk. II]} || ${Me.CombatAbility[Field Armorer Rk. III]} || ${Me.CombatAbility[Field Outfitter]} || ${Me.CombatAbility[Field Outfitter Rk. II]} || ${Me.CombatAbility[Field Outfitter Rk. III]} || ${Me.CombatAbility[Field Defender]} || ${Me.CombatAbility[Field Defender Rk. II]} || ${Me.CombatAbility[Field Defender Rk. III]} || ${Me.CombatAbility[Field Guardian]} || ${Me.CombatAbility[Field Guardian Rk. II]} || ${Me.CombatAbility[Field Guardian Rk. III]} || ${Me.CombatAbility[Field Protector]} || ${Me.CombatAbility[Field Protector Rk. II]} || ${Me.CombatAbility[Field Protector Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Field Armorer]} || ${Me.CombatAbility[Field Armorer Rk. II]} || ${Me.CombatAbility[Field Armorer Rk. III]} || ${Me.CombatAbility[Field Outfitter]} || ${Me.CombatAbility[Field Outfitter Rk. II]} || ${Me.CombatAbility[Field Outfitter Rk. III]} || ${Me.CombatAbility[Field Defender]} || ${Me.CombatAbility[Field Defender Rk. II]} || ${Me.CombatAbility[Field Defender Rk. III]} || ${Me.CombatAbility[Field Guardian]} || ${Me.CombatAbility[Field Guardian Rk. II]} || ${Me.CombatAbility[Field Guardian Rk. III]} || ${Me.CombatAbility[Field Protector]} || ${Me.CombatAbility[Field Protector Rk. II]} || ${Me.CombatAbility[Field Protector Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pFISTS, "fistsofwu", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[fists of wu]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[fists of wu]},1,0]}");

DECLARE_ABILITY_OPTION(pFCLAW, "fclaw", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[flurry of claws]} || ${Me.CombatAbility[flurry of claws rk. ii]} || ${Me.CombatAbility[flurry of claws rk. iii]} || ${Me.CombatAbility[tumult of claws]} || ${Me.CombatAbility[tumult of claws rk. ii]} || ${Me.CombatAbility[tumult of claws rk. iii]} || ${Me.CombatAbility[clamor of claws]} || ${Me.CombatAbility[clamor of claws rk. ii]} || ${Me.CombatAbility[clamor of claws rk. iii]} || ${Me.CombatAbility[tempest of claws]} || ${Me.CombatAbility[tempest of claws rk. ii]} || ${Me.CombatAbility[tempest of claws rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[flurry of claws]} || ${Me.CombatAbility[flurry of claws rk. ii]} || ${Me.CombatAbility[flurry of claws rk. iii]} || ${Me.CombatAbility[tumult of claws]} || ${Me.CombatAbility[tumult of claws rk. ii]} || ${Me.CombatAbility[tumult of claws rk. iii]} || ${Me.CombatAbility[clamor of claws]} || ${Me.CombatAbility[clamor of claws rk. ii]} || ${Me.CombatAbility[clamor of claws rk. iii]} || ${Me.CombatAbility[tempest of claws]} || ${Me.CombatAbility[tempest of claws rk. ii]} || ${Me.CombatAbility[tempest of claws rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pFLYKC, "flyingkick", "[ON/OFF]?", "${If[${Me.Skill[flying kick]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[flying kick]},1,0]}");

DECLARE_ABILITY_OPTION(pFORAG, "forage", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[forage]},1,0]}");

DECLARE_ABILITY_OPTION(pFRENZ, "frenzy", "[ON/OFF]?", "${If[${Me.Skill[frenzy]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[frenzy]} && ${Me.Skill[frenzy]},1,0]}");

DECLARE_ABILITY_OPTION(pFKICK, "ferociouskick", "[ON/OFF]?", "${If[${Me.AltAbility[ferocious kick]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[ferocious kick]},1,0]}");

DECLARE_ABILITY_OPTION(pGORSM, "gorillasmash", "[ON/OFF]?", "${If[${Me.AltAbility[gorilla smash]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[gorilla smash]},1,0]}");

DECLARE_ABILITY_OPTION(pGBLDE, "gblade", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Gouging Blade]} || ${Me.CombatAbility[Gouging Blade Rk. II]} || ${Me.CombatAbility[Gouging Blade Rk. III]} || ${Me.CombatAbility[Gashing Blade]} || ${Me.CombatAbility[Gashing Blade Rk. II]} || ${Me.CombatAbility[Gashing Blade Rk. III]} || ${Me.CombatAbility[Lacerating Blade]} || ${Me.CombatAbility[Lacerating Blade Rk. II]} || ${Me.CombatAbility[Lacerating Blade Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[Gouging Blade]} || ${Me.CombatAbility[Gouging Blade Rk. II]} || ${Me.CombatAbility[Gouging Blade Rk. III]} || ${Me.CombatAbility[Gashing Blade]} || ${Me.CombatAbility[Gashing Blade Rk. II]} || ${Me.CombatAbility[Gashing Blade Rk. III]} || ${Me.CombatAbility[Lacerating Blade]} || ${Me.CombatAbility[Lacerating Blade Rk. II]} || ${Me.CombatAbility[Lacerating Blade Rk. III]},1,0]}");

DECLARE_ABILITY_OPTION(pHARMT, "harmtouch", "[ON/OFF]?", "${If[${Me.AltAbility[harm touch].ID},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[harm touch]}   && ${Me.Class.ShortName.Equal[SHD]},1,0]}");

DECLARE_ABILITY_OPTION(pHIDES, "hide", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[hide]},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF0, "holyflag0", "[ON/OFF] holyflag0?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit0].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF1, "holyflag1", "[ON/OFF] holyflag1?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit1].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF2, "holyflag2", "[ON/OFF] holyflag2?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit2].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF3, "holyflag3", "[ON/OFF] holyflag3?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit3].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF4, "holyflag4", "[ON/OFF] holyflag4?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit4].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF5, "holyflag5", "[ON/OFF] holyflag5?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit5].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF6, "holyflag6", "[ON/OFF] holyflag6?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit6].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF7, "holyflag7", "[ON/OFF] holyflag7?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit7].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF8, "holyflag8", "[ON/OFF] holyflag8?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit8].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF9, "holyflag9", "[ON/OFF] holyflag9?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit9].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF10, "holyflag10", "[ON/OFF] holyflag10?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit10].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF11, "holyflag11", "[ON/OFF] holyflag11?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit11].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF12, "holyflag12", "[ON/OFF] holyflag12?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit12].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF13, "holyflag13", "[ON/OFF] holyflag13?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit13].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF14, "holyflag14", "[ON/OFF] holyflag14?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit14].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF15, "holyflag15", "[ON/OFF] holyflag15?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit15].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF16, "holyflag16", "[ON/OFF] holyflag16?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit16].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF17, "holyflag17", "[ON/OFF] holyflag17?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit17].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF18, "holyflag18", "[ON/OFF] holyflag18?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit18].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF19, "holyflag19", "[ON/OFF] holyflag19?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit19].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF20, "holyflag20", "[ON/OFF] holyflag20?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit20].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF21, "holyflag21", "[ON/OFF] holyflag21?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit21].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF22, "holyflag22", "[ON/OFF] holyflag22?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit22].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF23, "holyflag23", "[ON/OFF] holyflag23?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit23].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF24, "holyflag24", "[ON/OFF] holyflag24?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit24].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF25, "holyflag25", "[ON/OFF] holyflag25?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit25].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF26, "holyflag26", "[ON/OFF] holyflag26?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit26].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF27, "holyflag27", "[ON/OFF] holyflag27?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit27].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF28, "holyflag28", "[ON/OFF] holyflag28?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit28].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF29, "holyflag29", "[ON/OFF] holyflag29?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit29].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF30, "holyflag30", "[ON/OFF] holyflag30?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit30].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF31, "holyflag31", "[ON/OFF] holyflag31?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit31].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF32, "holyflag32", "[ON/OFF] holyflag32?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit32].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF33, "holyflag33", "[ON/OFF] holyflag33?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit33].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF34, "holyflag34", "[ON/OFF] holyflag34?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit34].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF35, "holyflag35", "[ON/OFF] holyflag35?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit35].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF36, "holyflag36", "[ON/OFF] holyflag36?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit36].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF37, "holyflag37", "[ON/OFF] holyflag37?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit37].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF38, "holyflag38", "[ON/OFF] holyflag38?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit38].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF39, "holyflag39", "[ON/OFF] holyflag39?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit39].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF40, "holyflag40", "[ON/OFF] holyflag40?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit40].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF41, "holyflag41", "[ON/OFF] holyflag41?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit41].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF42, "holyflag42", "[ON/OFF] holyflag42?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit42].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF43, "holyflag43", "[ON/OFF] holyflag43?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit43].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF44, "holyflag44", "[ON/OFF] holyflag44?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit44].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF45, "holyflag45", "[ON/OFF] holyflag45?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit45].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF46, "holyflag46", "[ON/OFF] holyflag46?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit46].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF47, "holyflag47", "[ON/OFF] holyflag47?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit47].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF48, "holyflag48", "[ON/OFF] holyflag48?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit48].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF49, "holyflag49", "[ON/OFF] holyflag49?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit49].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF50, "holyflag50", "[ON/OFF] holyflag50?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit50].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF51, "holyflag51", "[ON/OFF] holyflag51?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit51].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF52, "holyflag52", "[ON/OFF] holyflag52?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit52].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF53, "holyflag53", "[ON/OFF] holyflag53?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit53].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF54, "holyflag54", "[ON/OFF] holyflag54?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit54].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF55, "holyflag55", "[ON/OFF] holyflag55?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit55].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF56, "holyflag56", "[ON/OFF] holyflag56?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit56].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF57, "holyflag57", "[ON/OFF] holyflag57?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit57].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF58, "holyflag58", "[ON/OFF] holyflag58?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit58].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF59, "holyflag59", "[ON/OFF] holyflag59?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit59].Length},1,0]}");

DECLARE_ABILITY_OPTION(pHOLF60, "holyflag60", "[ON/OFF] holyflag60?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit60].Length},1,0]}");

DECLARE_ABILITY_OPTION(pINFUR, "infuriate", "[ON/OFF]?", "1", "${If[${meleemvi[plugin]},1,0]}");

DECLARE_ABILITY_OPTION(pINTIM, "intimidation", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[intimidation]},1,0]}");

DECLARE_ABILITY_OPTION(pJOLTS, "jolt", "Every [#] of Hits,0=0ff", "0", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},BER,BST,RNG]},1,0]}");

DECLARE_ABILITY_OPTION(pJKICK, "jltkicks", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[jolting kicks]} || ${Me.CombatAbility[jolting kicks rk. ii]} || ${Me.CombatAbility[jolting kicks rk. iii]} || ${Me.CombatAbility[Jolting Snapkicks]} || ${Me.CombatAbility[Jolting Snapkicks rk. ii]} || ${Me.CombatAbility[Jolting Snapkicks rk. iii]} || ${Me.CombatAbility[Jolting Frontkicks]} || ${Me.CombatAbility[Jolting Frontkicks rk. ii]} || ${Me.CombatAbility[Jolting Frontkicks rk. iii]} || ${Me.CombatAbility[Jolting Hook kicks]} || ${Me.CombatAbility[Jolting Hook kicks rk. ii]} || ${Me.CombatAbility[Jolting Hook kicks rk. iii]} || ${Me.CombatAbility[Jolting Crescent kicks]} || ${Me.CombatAbility[Jolting Crescent kicks rk. ii]} || ${Me.CombatAbility[Jolting Crescent kicks rk. iii]} || ${Me.CombatAbility[Jolting Heel Kicks]} || ${Me.CombatAbility[Jolting Heel Kicks rk. ii]} || ${Me.CombatAbility[Jolting Heel Kicks rk. iii]} || ${Me.CombatAbility[Jolting Cut Kicks]} || ${Me.CombatAbility[Jolting Cut Kicks rk. ii]} || ${Me.CombatAbility[Jolting Cut Kicks rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[jolting kicks]} || ${Me.CombatAbility[jolting kicks rk. ii]} || ${Me.CombatAbility[jolting kicks rk. iii]} || ${Me.CombatAbility[Jolting Snapkicks]} || ${Me.CombatAbility[Jolting Snapkicks rk. ii]} || ${Me.CombatAbility[Jolting Snapkicks rk. iii]} || ${Me.CombatAbility[Jolting Frontkicks]} || ${Me.CombatAbility[Jolting Frontkicks rk. ii]} || ${Me.CombatAbility[Jolting Frontkicks rk. iii]} || ${Me.CombatAbility[Jolting Hook kicks]} || ${Me.CombatAbility[Jolting Hook kicks rk. ii]} || ${Me.CombatAbility[Jolting Hook kicks rk. iii]} || ${Me.CombatAbility[Jolting Crescent kicks]} || ${Me.CombatAbility[Jolting Crescent kicks rk. ii]} || ${Me.CombatAbility[Jolting Crescent kicks rk. iii]} || ${Me.CombatAbility[Jolting Heel Kicks]} || ${Me.CombatAbility[Jolting Heel Kicks rk. ii]} || ${Me.CombatAbility[Jolting Heel Kicks rk. iii]} || ${Me.CombatAbility[Jolting Cut Kicks]} || ${Me.CombatAbility[Jolting Cut Kicks rk. ii]} || ${Me.CombatAbility[Jolting Cut Kicks rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pJUGUL, "jugular", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[Jugular Slash]} || ${Me.CombatAbility[Jugular Slash rk. ii]} || ${Me.CombatAbility[Jugular Slash rk. iii]} || ${Me.CombatAbility[Jugular Slice]} || ${Me.CombatAbility[Jugular Slice rk. ii]} || ${Me.CombatAbility[Jugular Slice rk. iii]} || ${Me.CombatAbility[Jugular Sever]} || ${Me.CombatAbility[Jugular Sever rk. ii]} || ${Me.CombatAbility[Jugular Sever rk. iii]} || ${Me.CombatAbility[Jugular Gash]} || ${Me.CombatAbility[Jugular Gash rk. ii]} || ${Me.CombatAbility[Jugular Gash rk. iii]} || ${Me.CombatAbility[Jugular Lacerate]} || ${Me.CombatAbility[Jugular Lacerate rk. ii]} || ${Me.CombatAbility[Jugular Lacerate rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Jugular Slash]} || ${Me.CombatAbility[Jugular Slash rk. ii]} || ${Me.CombatAbility[Jugular Slash rk. iii]} || ${Me.CombatAbility[Jugular Slice]} || ${Me.CombatAbility[Jugular Slice rk. ii]} || ${Me.CombatAbility[Jugular Slice rk. iii]} || ${Me.CombatAbility[Jugular Sever]} || ${Me.CombatAbility[Jugular Sever rk. ii]} || ${Me.CombatAbility[Jugular Sever rk. iii]} || ${Me.CombatAbility[Jugular Gash]} || ${Me.CombatAbility[Jugular Gash rk. ii]} || ${Me.CombatAbility[Jugular Gash rk. iii]} || ${Me.CombatAbility[Jugular Lacerate]} || ${Me.CombatAbility[Jugular Lacerate rk. ii]} || ${Me.CombatAbility[Jugular Lacerate rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pKICKS, "kick", "[ON/OFF]?", "${If[${Me.Skill[kick]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[kick]},1,0]}");

DECLARE_ABILITY_OPTION(pKNEES, "kneestrike", "[ON/OFF]?", "${If[${Me.AltAbility[knee strike]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[knee strike]},1,0]}");

DECLARE_ABILITY_OPTION(pKNFPL, "knifeplay", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Knifeplay]} || ${Me.CombatAbility[Knifeplay Rk. II]} || ${Me.CombatAbility[Knifeplay Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Knifeplay]} || ${Me.CombatAbility[Knifeplay Rk. II]} || ${Me.CombatAbility[Knifeplay Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pLHAND, "layhand", "[#] MyLife% Below? 0=0ff", "${If[${Me.AltAbility[Lay on Hands]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Lay on Hands]} && ${Me.Class.ShortName.Equal[PAL]},1,0]}");

DECLARE_ABILITY_OPTION(pLCLAW, "leopardclaw", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[leopard claw]} || ${Me.CombatAbility[dragon fang]} || ${Me.CombatAbility[clawstriker's flurry]} || ${Me.CombatAbility[clawstriker's flurry rk. ii]} || ${Me.CombatAbility[clawstriker's flurry rk. iii]} || ${Me.CombatAbility[wheel of fists]} || ${Me.CombatAbility[wheel of fists rk. ii]} || ${Me.CombatAbility[wheel of fists rk. iii]} || ${Me.CombatAbility[Six-Step Pattern]} || ${Me.CombatAbility[Six-Step Pattern rk. ii]} || ${Me.CombatAbility[Six-Step Pattern rk. iii]} || ${Me.CombatAbility[Seven-Step Pattern]} || ${Me.CombatAbility[Seven-Step Pattern rk. ii]} || ${Me.CombatAbility[Seven-Step Pattern rk. iii]} || ${Me.CombatAbility[Eight-Step Pattern]} || ${Me.CombatAbility[Eight-Step Pattern Rk. II]} || ${Me.CombatAbility[Eight-Step Pattern Rk. III]} || ${Me.CombatAbility[Torrent of Fists]} || ${Me.CombatAbility[Torrent of Fists Rk. II]} || ${Me.CombatAbility[Torrent of Fists Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[leopard claw]} || ${Me.CombatAbility[dragon fang]} || ${Me.CombatAbility[clawstriker's flurry]} || ${Me.CombatAbility[clawstriker's flurry rk. ii]} || ${Me.CombatAbility[clawstriker's flurry rk. iii]} || ${Me.CombatAbility[wheel of fists]} || ${Me.CombatAbility[wheel of fists rk. ii]} || ${Me.CombatAbility[wheel of fists rk. iii]} || ${Me.CombatAbility[Six-Step Pattern]} || ${Me.CombatAbility[Six-Step Pattern rk. ii]} || ${Me.CombatAbility[Six-Step Pattern rk. iii]} || ${Me.CombatAbility[Seven-Step Pattern]} || ${Me.CombatAbility[Seven-Step Pattern rk. ii]} || ${Me.CombatAbility[Seven-Step Pattern rk. iii]} || ${Me.CombatAbility[Eight-Step Pattern]} || ${Me.CombatAbility[Eight-Step Pattern Rk. II]} || ${Me.CombatAbility[Eight-Step Pattern Rk. III]} || ${Me.CombatAbility[Torrent of Fists]} || ${Me.CombatAbility[Torrent of Fists Rk. II]} || ${Me.CombatAbility[Torrent of Fists Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pMELEE, "melee", "[ON/OFF] Melee Mode? 0=0ff", "${If[${Select[${Me.Class.ShortName},WAR,PAL,RNG,SHD,MNK,BRD,ROG,BST,BER]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]},1,0]}");

DECLARE_ABILITY_OPTION(pMELEP, "meleepri", "[ID] Primary (Melee)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && !${meleemvi[aggro]},1,0]}");

DECLARE_ABILITY_OPTION(pMELES, "meleesec", "[ID] Offhand (Melee)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && !${meleemvi[aggro]},1,0]} && ${meleemvi[meleepri]}");

DECLARE_ABILITY_OPTION(pMENDS, "mend", "[#] MyLife% Below? 0=0ff", "${If[${Me.Skill[mend]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.Skill[mend]},1,0]}");

DECLARE_ABILITY_OPTION(pPETAS, "petassist", "[ON/OFF] Assist Me?", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");

DECLARE_ABILITY_OPTION(pPETDE, "petdelay", "[#] # Sec Delay Before Engaging?", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},0,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");

DECLARE_ABILITY_OPTION(pPETRN, "petrange", "[#] Target/Pet in this range?", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},75,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");

DECLARE_ABILITY_OPTION(pPETMN, "petmend", "[#] Mend Pet Life % Below 0=0ff?", "${If[${Me.AltAbility[mend companion]},20,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Me.AltAbility[mend companion]},1,0]}");

DECLARE_ABILITY_OPTION(pPETENG, "petengagehps", "[#] TargetCurrentHPS% Below? 0=0ff", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},98,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");

DECLARE_ABILITY_OPTION(pPICKP, "pickpocket", "[ON/OFF]?", "${If[${Me.Skill[pick pockets]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[pick pockets]},1,0]}");

DECLARE_ABILITY_OPTION(pPINPT, "pinpoint", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[Pinpoint Vulnerability]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. ii]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. iii]} || ${Me.CombatAbility[Pinpoint Weaknesses]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. ii]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. iii]} || ${Me.CombatAbility[Pinpoint Vitals]} || ${Me.CombatAbility[Pinpoint Vitals rk. ii]} || ${Me.CombatAbility[Pinpoint Vitals rk. iii]} || ${Me.CombatAbility[Pinpoint Flaws]} || ${Me.CombatAbility[Pinpoint Flaws rk. ii]} || ${Me.CombatAbility[Pinpoint Flaws rk. iii]} || ${Me.CombatAbility[Pinpoint Liabilities]} || ${Me.CombatAbility[Pinpoint Liabilities rk. ii]} || ${Me.CombatAbility[Pinpoint Liabilities rk. iii]} || ${Me.CombatAbility[Pinpoint Deficiencies]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. ii]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Pinpoint Vulnerability]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. ii]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. iii]} || ${Me.CombatAbility[Pinpoint Weaknesses]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. ii]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. iii]} || ${Me.CombatAbility[Pinpoint Vitals]} || ${Me.CombatAbility[Pinpoint Vitals rk. ii]} || ${Me.CombatAbility[Pinpoint Vitals rk. iii]} || ${Me.CombatAbility[Pinpoint Flaws]} || ${Me.CombatAbility[Pinpoint Flaws rk. ii]} || ${Me.CombatAbility[Pinpoint Flaws rk. iii]} || ${Me.CombatAbility[Pinpoint Liabilities]} || ${Me.CombatAbility[Pinpoint Liabilities rk. ii]} || ${Me.CombatAbility[Pinpoint Liabilities rk. iii]} || ${Me.CombatAbility[Pinpoint Deficiencies]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. ii]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pPLUGS, "plugin", "[ON/OFF]?", "1", "${If[${meleemvi[plugin]},1,0]}");

DECLARE_ABILITY_OPTION(pPOKER, "poker", "[ID] item?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[backstab]},1,0]}");

DECLARE_ABILITY_OPTION(pHFAST, "pothealfast", "[#] MyLife% Below? 0=0ff (FAST)", "${If[${meleemvi[idpothealfast]},30,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[idpothealfast]},1,0]}");

DECLARE_ABILITY_OPTION(pHOVER, "pothealover", "[#] MyLife% Below? 0=0ff (OVER)", "${If[${meleemvi[idpothealover]},20,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[idpothealover]},1,0]}");

DECLARE_ABILITY_OPTION(pPRVKO, "provokeonce", "[ON/OFF]?", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");

DECLARE_ABILITY_OPTION(pPRVKM, "provokemax", "[#] Counter? ,1=try once, 0=0ff", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");

DECLARE_ABILITY_OPTION(pPRVKE, "provokeend", "[#] Stop when Target Life% Below?", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},20,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${meleemvi[provokemax]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");

DECLARE_ABILITY_OPTION(pPRVK0, "provoke0", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${meleemvi[provokemax]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");

DECLARE_ABILITY_OPTION(pPRVK1, "provoke1", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${meleemvi[provokemax]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");

DECLARE_ABILITY_OPTION(pRAVOL, "ragevolley", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[rage volley]} || ${Me.CombatAbility[destroyer's volley]} || ${Me.CombatAbility[giantslayer's volley]} || ${Me.CombatAbility[giantslayer's volley rk. ii]} || ${Me.CombatAbility[giantslayer's volley rk. iii]} || ${Me.CombatAbility[annihilator's volley]} || ${Me.CombatAbility[annihilator's volley rk. ii]} || ${Me.CombatAbility[annihilator's volley rk. iii]} || ${Me.CombatAbility[decimator's volley]} || ${Me.CombatAbility[decimator's volley rk. ii]} || ${Me.CombatAbility[decimator's volley rk. iii]} || ${Me.CombatAbility[Eradicator's Volley]} || ${Me.CombatAbility[Eradicator's Volley rk. ii]} || ${Me.CombatAbility[Eradicator's Volley rk. iii]} || ${Me.CombatAbility[Savage Volley]} || ${Me.CombatAbility[Savage Volley rk. ii]} || ${Me.CombatAbility[Savage Volley rk. iii]} || ${Me.CombatAbility[Sundering Volley]} || ${Me.CombatAbility[Sundering Volley rk. ii]} || ${Me.CombatAbility[Sundering Volley rk. iii]} || ${Me.CombatAbility[Brutal Volley]} || ${Me.CombatAbility[Brutal Volley rk. ii]} || ${Me.CombatAbility[Brutal Volley rk. iii]} || ${Me.CombatAbility[Demolishing Volley]} || ${Me.CombatAbility[Demolishing Volley rk. ii]} || ${Me.CombatAbility[Demolishing Volley rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[rage volley]} || ${Me.CombatAbility[destroyer's volley]} || ${Me.CombatAbility[giantslayer's volley]} || ${Me.CombatAbility[giantslayer's volley rk. ii]} || ${Me.CombatAbility[giantslayer's volley rk. iii]} || ${Me.CombatAbility[annihilator's volley]} || ${Me.CombatAbility[annihilator's volley rk. ii]} || ${Me.CombatAbility[annihilator's volley rk. iii]} || ${Me.CombatAbility[decimator's volley]} || ${Me.CombatAbility[decimator's volley rk. ii]} || ${Me.CombatAbility[decimator's volley rk. iii]} || ${Me.CombatAbility[Eradicator's Volley]} || ${Me.CombatAbility[Eradicator's Volley rk. ii]} || ${Me.CombatAbility[Eradicator's Volley rk. iii]} || ${Me.CombatAbility[Savage Volley]} || ${Me.CombatAbility[Savage Volley rk. ii]} || ${Me.CombatAbility[Savage Volley rk. iii]} || ${Me.CombatAbility[Sundering Volley]} || ${Me.CombatAbility[Sundering Volley rk. ii]} || ${Me.CombatAbility[Sundering Volley rk. iii]} || ${Me.CombatAbility[Brutal Volley]} || ${Me.CombatAbility[Brutal Volley rk. ii]} || ${Me.CombatAbility[Brutal Volley rk. iii]} || ${Me.CombatAbility[Demolishing Volley]} || ${Me.CombatAbility[Demolishing Volley rk. ii]} || ${Me.CombatAbility[Demolishing Volley rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pRAKES, "rake", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[rake]} || ${Me.CombatAbility[harrow]} || ${Me.CombatAbility[harrow rk. ii]} || ${Me.CombatAbility[harrow rk. iii]} || ${Me.CombatAbility[foray]} || ${Me.CombatAbility[foray rk. ii]} || ${Me.CombatAbility[foray rk. iii]} || ${Me.CombatAbility[rush]} || ${Me.CombatAbility[rush rk. ii]} || ${Me.CombatAbility[rush rk. iii]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage rk. ii]} || ${Me.CombatAbility[Barrage rk. iii]} || ${Me.CombatAbility[Pummel]} || ${Me.CombatAbility[Pummel rk. ii]} || ${Me.CombatAbility[Pummel rk. iii]} || ${Me.CombatAbility[Maul]} || ${Me.CombatAbility[Maul rk. ii]} || ${Me.CombatAbility[Maul rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[rake]} || ${Me.CombatAbility[harrow]} || ${Me.CombatAbility[harrow rk. ii]} || ${Me.CombatAbility[harrow rk. iii]} || ${Me.CombatAbility[foray]} || ${Me.CombatAbility[foray rk. ii]} || ${Me.CombatAbility[foray rk. iii]} || ${Me.CombatAbility[rush]} || ${Me.CombatAbility[rush rk. ii]} || ${Me.CombatAbility[rush rk. iii]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage rk. ii]} || ${Me.CombatAbility[Barrage rk. iii]} || ${Me.CombatAbility[Pummel]} || ${Me.CombatAbility[Pummel rk. ii]} || ${Me.CombatAbility[Pummel rk. iii]} || ${Me.CombatAbility[Maul]} || ${Me.CombatAbility[Maul rk. ii]} || ${Me.CombatAbility[Maul rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pRANGE, "range", "[#] Max Range? 0=0ff", "0", "${If[${meleemvi[plugin]},1,0]}");

DECLARE_ABILITY_OPTION(pRAVEN, "ravens", "[ON/OFF]?", "${If[${Me.AltAbility[raven's claw]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[raven's claw]},1,0]}");

DECLARE_ABILITY_OPTION(pRESUM, "resume", "[#] Life% Above? 100=0ff", "75", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]},1,0]}");

DECLARE_ABILITY_OPTION(pRGHTI, "rightidg", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Righteous Indignation]} || ${Me.CombatAbility[Righteous Indignation rk. ii]} || ${Me.CombatAbility[Righteous Indignation rk. iii]} || ${Me.CombatAbility[Righteous Vexation]} || ${Me.CombatAbility[Righteous Vexation rk. ii]} || ${Me.CombatAbility[Righteous Vexation rk. iii]} || ${Me.CombatAbility[Righteous Umbrage]} || ${Me.CombatAbility[Righteous Umbrage rk. ii]} || ${Me.CombatAbility[Righteous Umbrage rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Righteous Indignation]} || ${Me.CombatAbility[Righteous Indignation rk. ii]} || ${Me.CombatAbility[Righteous Indignation rk. iii]} || ${Me.CombatAbility[Righteous Vexation]} || ${Me.CombatAbility[Righteous Vexation rk. ii]} || ${Me.CombatAbility[Righteous Vexation rk. iii]} || ${Me.CombatAbility[Righteous Umbrage]} || ${Me.CombatAbility[Righteous Umbrage rk. ii]} || ${Me.CombatAbility[Righteous Umbrage rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pRKICK, "roundkick", "[ON/OFF]?", "${If[${Me.Skill[round kick]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[round kick]},1,0]}");

DECLARE_ABILITY_OPTION(pSBLADES, "stormblades", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[focused storm of blades]} || ${Me.CombatAbility[focused storm of blades rk. ii]} || ${Me.CombatAbility[focused storm of blades rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[focused storm of blades]} || ${Me.CombatAbility[focused storm of blades rk. ii]} || ${Me.CombatAbility[focused storm of blades rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pSELOK, "selos", "[ON/OFF]?", "${If[${Me.AltAbility[selo's kick]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[selo's kick]},1,0]}");

DECLARE_ABILITY_OPTION(pSENSE, "sensetraps", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[sensetraps]},1,0]}");

DECLARE_ABILITY_OPTION(pSHIEL, "shield", "[ID] item?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[bash]},0,0]}");

DECLARE_ABILITY_OPTION(pSLAMS, "slam", "[ON/OFF]?", "${If[${Select[${Me.Race.ID},2,9,10]},1,0]}", // 2=barbarian 9=troll 10=ogre
"${If[${meleemvi[plugin]} && ${Select[${Me.Race.ID},2,9,10]},1,0]}");

DECLARE_ABILITY_OPTION(pSLAPF, "slapface", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Slap in the Face]} || ${Me.CombatAbility[Slap in the Face rk. ii]} || ${Me.CombatAbility[Slap in the Face rk. iii]} || ${Me.CombatAbility[Kick in the Teeth]} || ${Me.CombatAbility[Kick in the Teeth rk. ii]} || ${Me.CombatAbility[Kick in the Teeth rk. iii]} || ${Me.CombatAbility[Punch in the Throat]} || ${Me.CombatAbility[Punch in the Throat rk. ii]} || ${Me.CombatAbility[Punch in the Throat rk. iii]} || ${Me.CombatAbility[Kick in the Shins]} || ${Me.CombatAbility[Kick in the Shins rk. ii]} || ${Me.CombatAbility[Kick in the Shins rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Slap in the Face]} || ${Me.CombatAbility[Slap in the Face rk. ii]} || ${Me.CombatAbility[Slap in the Face rk. iii]} || ${Me.CombatAbility[Kick in the Teeth]} || ${Me.CombatAbility[Kick in the Teeth rk. ii]} || ${Me.CombatAbility[Kick in the Teeth rk. iii]} || ${Me.CombatAbility[Punch in the Throat]} || ${Me.CombatAbility[Punch in the Throat rk. ii]} || ${Me.CombatAbility[Punch in the Throat rk. iii]} || ${Me.CombatAbility[Kick in the Shins]} || ${Me.CombatAbility[Kick in the Shins rk. ii]} || ${Me.CombatAbility[Kick in the Shins rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pSNEAK, "sneak", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[sneak]},1,0]}");

DECLARE_ABILITY_OPTION(pSTAND, "standup", "[ON/OFF] Authorize to StandUp?", "0", "${If[${meleemvi[plugin]},1,0]}");

DECLARE_ABILITY_OPTION(pSTEEL, "steely", "[ON/OFF]", "${If[${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.Book[Steely Stance]} || ${Me.Book[Steely Stance rk. ii]} || ${Me.Book[Steely Stance rk. iii]} || ${Me.Book[Stubborn Stance]} || ${Me.Book[Stubborn Stance rk. ii]} || ${Me.Book[Stubborn Stance rk. iii]} || ${Me.Book[Stoic Stance]} || ${Me.Book[Stoic Stance rk. ii]} || ${Me.Book[Stoic Stance rk. iii]} || ${Me.Book[Steadfast Stance]} || ${Me.Book[Steadfast Stance rk. ii]} || ${Me.Book[Steadfast Stance rk. iii]} || ${Me.Book[Staunch Stance]} || ${Me.Book[Staunch Stance rk. ii]} || ${Me.Book[Staunch Stance rk. iii]}),0,0]}", "${If[${meleemvi[plugin]} && ${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.Book[Steely Stance]} || ${Me.Book[Steely Stance rk. ii]} || ${Me.Book[Steely Stance rk. iii]} || ${Me.Book[Stubborn Stance]} || ${Me.Book[Stubborn Stance rk. ii]} || ${Me.Book[Stubborn Stance rk. iii]} || ${Me.Book[Stoic Stance]} || ${Me.Book[Stoic Stance rk. ii]} || ${Me.Book[Stoic Stance rk. iii]} || ${Me.Book[Steadfast Stance]} || ${Me.Book[Steadfast Stance rk. ii]} || ${Me.Book[Steadfast Stance rk. iii]} || ${Me.Book[Staunch Stance]} || ${Me.Book[Staunch Stance rk. ii]} || ${Me.Book[Staunch Stance rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pSTIKKB, "stickbreak", "0=Normal, 1=Allow BreakOnKB", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");

DECLARE_ABILITY_OPTION(pSTIKNR, "sticknorange", "0=Normal, 1=No Range Check", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");

DECLARE_ABILITY_OPTION(pSTIKR, "stickrange", "[#] Target in Range? 0=0ff", "${If[${Stick.Status.NotEqual[NULL]},75,0]}", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]} && ${meleemvi[stickrange]},1,0]}");

DECLARE_ABILITY_OPTION(pSTIKD, "stickdelay", "[#] Sec to Wait Target in Range?", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");

DECLARE_ABILITY_OPTION(pSTIKM, "stickmode", "[ON/OFF] Use stickcmd from ini?", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");

DECLARE_ABILITY_OPTION(pSTRIK, "strike", "Use best sneak attack disc [ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[backstab]} && ${meleemvi[idstrike]},1,0]}");

DECLARE_ABILITY_OPTION(pSTRIKM, "strikemode", "[ON/OFF] Use strikecmd from ini?", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");

DECLARE_ABILITY_OPTION(pSTUNS, "stunning", "[#] Target Life% Below? 0=0ff", "0", "${If[${meleemvi[plugin]},1,0]}");

DECLARE_ABILITY_OPTION(pSTUN0, "stun0", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[stunning]},1,0]}");

DECLARE_ABILITY_OPTION(pSTUN1, "stun1", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[stunning]},1,0]}");

DECLARE_ABILITY_OPTION(pTAUNT, "taunt", "[ON/OFF]?", "${If[${Me.Skill[taunt]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[aggro]} && ${Me.Skill[taunt]},1,0]}");

DECLARE_ABILITY_OPTION(pTHIEF, "thiefeye", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[thief's eyes]} || ${Me.CombatAbility[Thief's Vision]} || ${Me.CombatAbility[Thief's Vision Rk. II]} || ${Me.CombatAbility[Thief's Vision Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[thief's eyes]} || ${Me.CombatAbility[Thief's Vision]} || ${Me.CombatAbility[Thief's Vision Rk. II]} || ${Me.CombatAbility[Thief's Vision Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pTHROW, "throwstone", "[#] Endu% Above? 0=0ff", "0", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[throw stone]},1,0]}");

DECLARE_ABILITY_OPTION(pTIGER, "tigerclaw", "[ON/OFF]?", "${If[${Me.Skill[tiger claw]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[tiger claw]},1,0]}");

DECLARE_ABILITY_OPTION(pTWIST, "twistedshank", "[ON/OFF]?", "${If[${Me.AltAbility[twisted shank]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.AltAbility[twisted shank]},1,0]}");

DECLARE_ABILITY_OPTION(pVIGAX, "vigaxe", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Vigorous Axe Throw]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. II]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. III]} || ${Me.CombatAbility[Energetic Axe Throw]} || ${Me.CombatAbility[Energetic Axe Throw Rk. II]} || ${Me.CombatAbility[Energetic Axe Throw Rk. III]} || ${Me.CombatAbility[Spirited Axe Throw]} || ${Me.CombatAbility[Spirited Axe Throw Rk. II]} || ${Me.CombatAbility[Spirited Axe Throw Rk. III]} || ${Me.CombatAbility[Brutal Axe Throw]} || ${Me.CombatAbility[Brutal Axe Throw Rk. II]} || ${Me.CombatAbility[Brutal Axe Throw Rk. III]} || ${Me.CombatAbility[Demolishing Axe Throw]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. II]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Vigorous Axe Throw]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. II]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. III]} || ${Me.CombatAbility[Energetic Axe Throw]} || ${Me.CombatAbility[Energetic Axe Throw Rk. II]} || ${Me.CombatAbility[Energetic Axe Throw Rk. III]} || ${Me.CombatAbility[Spirited Axe Throw]} || ${Me.CombatAbility[Spirited Axe Throw Rk. II]} || ${Me.CombatAbility[Spirited Axe Throw Rk. III]} || ${Me.CombatAbility[Brutal Axe Throw]} || ${Me.CombatAbility[Brutal Axe Throw Rk. II]} || ${Me.CombatAbility[Brutal Axe Throw Rk. III]} || ${Me.CombatAbility[Demolishing Axe Throw]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. II]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pVIGDR, "vigdagger", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Vigorous Dagger-Throw]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. III]} || ${Me.CombatAbility[Vigorous Dagger-Strike]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Strike]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Throw]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Vigorous Dagger-Throw]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. III]} || ${Me.CombatAbility[Vigorous Dagger-Strike]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Strike]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Throw]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pVIGSN, "vigshuriken", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Vigorous Shuriken]} || ${Me.CombatAbility[Vigorous Shuriken Rk. II]} || ${Me.CombatAbility[Vigorous Shuriken Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Vigorous Shuriken]} || ${Me.CombatAbility[Vigorous Shuriken Rk. II]} || ${Me.CombatAbility[Vigorous Shuriken Rk. III]}),1,0]}");

DECLARE_ABILITY_OPTION(pWITHS, "withstand", "[#] Endu% Above? 0=Off", "${If[${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.CombatAbility[withstand]} || ${Me.CombatAbility[withstand rk. ii]} || ${Me.CombatAbility[withstand rk. iii]} || ${Me.CombatAbility[defy]} || ${Me.CombatAbility[defy rk. ii]} || ${Me.CombatAbility[defy rk. iii]} || ${Me.CombatAbility[Reprove]} || ${Me.CombatAbility[Reprove rk. ii]} || ${Me.CombatAbility[Reprove rk. iii]} || ${Me.CombatAbility[Repel]} || ${Me.CombatAbility[Repel rk. ii]} || ${Me.CombatAbility[Repel rk. iii]}),20,0]}", "${If[${meleemvi[plugin]} && ${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.CombatAbility[withstand]} || ${Me.CombatAbility[withstand rk. ii]} || ${Me.CombatAbility[withstand rk. iii]} || ${Me.CombatAbility[defy]} || ${Me.CombatAbility[defy rk. ii]} || ${Me.CombatAbility[defy rk. iii]} || ${Me.CombatAbility[Reprove]} || ${Me.CombatAbility[Reprove rk. ii]} || ${Me.CombatAbility[Reprove rk. iii]} || ${Me.CombatAbility[Repel]} || ${Me.CombatAbility[Repel rk. ii]} || ${Me.CombatAbility[Repel rk. iii]}),1,0]}");

DECLARE_ABILITY_OPTION(pYAULP, "yaulp", "[ON/OFF]?", "${If[${Me.AltAbility[yaulp]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[yaulp]},0,0]}");
