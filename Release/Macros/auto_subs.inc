| Auto-Subs Shared routines across Auto series macros by Noobhaxor
| Version 6.2
| Updated: 1-23-2018
| ----------------------------------------------------------------------------
| SUB: Compatibility with KissAssist Aliases Old and New
| ----------------------------------------------------------------------------
#Event Camping           "#*#seconds to prepare your camp."
#Event GainSomething	 "#*#You have gained|#1#|"
#Event TaskUpdate		 "#*#Your task |#1#| has been updated#*#" 
#Event Zoned             "LOADING, PLEASE WAIT#*#"
#Event Zoned             "You have entered#*#"
#bind BackOff        /backoff
#bind Campfire       /campfire
#bind ChangeVarInt   /changevarint
#bind ToggleVariable /togglevariable

Sub AliasSetup
	/squelch /alias /camphere       /togglevariable ReturnToCamp
	/noparse /alias /campoff	    /togglevariable ReturnToCamp 0
	/squelch /alias /campradius     /changevarint General CampRadius
	/squelch /alias /chase          /togglevariable ChaseAssist
	/squelch /alias /chaseoff       /changevarint General ChaseAssist 0        
	/squelch /alias /chaseon        /changevarint General ChaseAssist 1 
	/squelch /alias /meleeon        /togglevariable MeleeOn
	/squelch /alias /meleeoff       /togglevariable MeleeOn 0
	/squelch /alias /mezon          /changevarint Mez MezOn
	/squelch /alias /mezoff         /changevarint Mez MezOn 0
	/squelch /alias /slowon         /togglevariable UseSlow 1
	/squelch /alias /slowoff        /togglevariable UseSlow 0
	/squelch /alias /returntocamp   /togglevariable ReturnToCamp
	/noparse /alias /instantrelease /togglevariable InstantRelease
	/noparse /alias /usefellowship 	/togglevariable UseFellowship
	
/return
Sub VarSetup
	/declare MacroName       string outer Auto
	/declare IniFileName     string outer ${MacroName}_Player_${Me.CleanName}.ini
	/declare autotargetid	int outer 0
	/declare sittimer		timer outer 0
	/declare enablesit		int outer 0
	/declare ReturnToCamp	int outer 0
	/declare AutoCampRadius	int outer 30
	/declare AutoCampX		int outer ${Me.X}
	/declare AutoCampY		int outer ${Me.Y}
	/declare changetoini	int outer 0
	/declare targetcleartimer	timer outer 0
	/declare numoftargets	int outer 0
	/declare ChaseAssist	int outer 0
	/if (${Me.AltAbility[Mnemonic Retention].Rank}==5) {
		/declare spellmisc	int outer 13
        /declare spellslots int outer 12
	} else /if (${Me.AltAbility[Mnemonic Retention].Rank}==4) {
		/declare spellmisc 	int outer 12
        /declare spellslots int outer 11
	} else /if (${Me.AltAbility[Mnemonic Retention].Rank}==3) {
		/declare spellmisc 	int outer 11
        /declare spellslots int outer 10
	} else /if (${Me.AltAbility[Mnemonic Retention].Rank}==2) {
		/declare spellmisc 	int outer 10
        /declare spellslots int outer 9
	} else /if (${Me.AltAbility[Mnemonic Retention].Rank}==1) {
		/declare spellmisc 	int outer 9
        /declare spellslots int outer 8
	} else {
		/declare spellmisc 	int outer 8
		/declare spellslots int outer 7
	}
	
	/echo -----${MacroName} LOADING------
	/if (${Me.XTarget[1].TargetType.NotEqual[Auto Hater]}) {
		/echo Extended Target 1 HAS TO BE SET TO Auto Hater
		/xtarget set 1 Autohater
		/echo Set Extended Target 1 to Auto
	  }
	|-Check for loaded plugins
	/call CheckPlugin MQ2Cast
    /call CheckPlugin MQ2Exchange
	/call CheckPlugin MQ2Rez
	/call CheckPlugin MQ2AdvPath
	/call UnCheckPlugin MQ2Twist
	/squelch /rez accept on
    /squelch /rez loot off
	/squelch /rez pct 90
/return
|------------------------KissAssist Alias 9.2.4 Updates-----------------------
| ----------------------------------------------------------------------------
| SUB: Bind Change Var Int resets various interger settings from ini file
| ----------------------------------------------------------------------------
    Sub Bind_ChangeVarInt(string ISection, string IName, int IVar)
        /docommand /varset changetoini 1
	|ChaseAssist Var Set
        /if (${IName.Equal[ChaseAssist]} && ${IVar}==1 &&${Target.Type.Equal[PC]}) {
			/docommand /varset FollowToonName ${Target.Name}
			/echo Following ${If[${FollowToonName.Equal[NULL ]},OFF,${FollowToonName}]}
		} else /if (${IName.Equal[ChaseAssist]} && ${IVar}==0) {
			/varset FollowToonName NULL
			/echo Following OFF
			/if (${AdvPath.Following}) /docommand /afollow off
		}
	|Camp Radius Set
		/if (${IName.Equal[CampRadius]} && ${IVar}>=20 && ${IVar}<=100) {
			/varset AutoCampRadius ${IVar}
			/echo Camp Radius set to ${IVar}
		} else /if (${IName.Equal[CampRadius]} && ${IVar}<20 && ${IVar}>100) {
			/echo Invalid Camp Radius - Set between 20-100 units
		}
	|Mez Var Change
		/if (${IName.Equal[MezOn]} && ${IVar}!=0) {
			/varset UseMez 1
			/echo Mez is now On
		} else /if (${IName.Equal[MezOn]} && ${IVar}!=1) {
			/varset UseMez 0
			/echo Mez is now Off
		}
		
	/return
| -------------------------------------------------------------------------------------
| SUB: Bind_ToggleVariable  Check whenever the player is changing any variable via /echo
| -------------------------------------------------------------------------------------
    Sub Bind_ToggleVariable(string Command, string Command2, string Command3)
        /declare i int local
		/declare OnOff local
        | Toggle Variables & Set Variables
        /if (${Defined[${Command}]}) {
            /if (${Command2.Length} && ${Command2.NotEqual[null]}) {
                /if (${Select[${Command2},0,off]}) {
                    /varset ${Command} 0
                    /varset OnOff Off
                } else /if (${Select[${Command2},1,on]}) {
                    /varset ${Command} 1
                    /varset OnOff On
                } 
            } else /if (${${Command}}) {
				/varset ${Command} 0
                /varset OnOff Off
			} else {
                /varset ${Command} 1
                /varset OnOff On
            }
			/docommand /varset changetoini 1
		|Camp Toggle
            /if (${Command.Equal[ReturnToCamp]} && ${OnOff.Equal[on]} ) {
				/echo Camp turned ON
				/docommand /varset ReturnToCamp 1
				/docommand /varset AutoCampX ${Me.X}
				/docommand /varset AutoCampY ${Me.Y}
            } else /if (${Command.Equal[ReturnToCamp]} && ${OnOff.Equal[off]}) {
                /echo Turning ALL Camp settings to OFF
				/docommand /varset ReturnToCamp 0
            }
		|Chase Toon Toggle
			/if (${Command.Equal[ChaseAssist]} && ${OnOff.Equal[on]} &&${Target.Type.Equal[PC]}) {
				/docommand /varset FollowToonName ${Target.Name}
				/echo Following ${If[${FollowToonName.Equal[NULL ]},OFF,${FollowToonName}]}
			} else /if (${Command.Equal[ChaseAssist]} && ${OnOff.Equal[off]}) {
				/varset FollowToonName NULL
				/echo Following OFF
				/if (${AdvPath.Following}) /docommand /afollow off
			}
		|Melee Toggle
			/if (${Command.Equal[MeleeOn]} && ${OnOff.Equal[on]} ) {
				/varset UseMelee 1
			} else /if (${Command.Equal[MeleeOn]} && ${OnOff.Equal[off]}) {
				/varset UseMelee 0
			}
        |Slow Toggle
			/if (${Command.Equal[UseSlow]} && ${OnOff.Equal[on]} ) {
				/varset UseSlow 1
			} else /if (${Command.Equal[UseSlow]} && ${OnOff.Equal[off]}) {
				/varset UseSlow 0
			}
		|InstantRelease on Death
			/if (${Command.Equal[InstantRelease]} && ${OnOff.Equal[on]} ) {
				/varset InstantRelease 1
			} else /if (${Command.Equal[InstantRelease]} && ${OnOff.Equal[off]}) {
				/varset InstantRelease 0
			}
        |Use Fellowship to port back on death
			/if (${Command.Equal[UseFellowship]} && ${OnOff.Equal[on]} ) {
				/varset UseFellowship 1
			} else /if (${Command.Equal[UseFellowship]} && ${OnOff.Equal[off]}) {
				/varset UseFellowship 0
			}
            /echo >> Set ${Command}: ${OnOff}
        }
    /return
|-----------------------------------------------------------------------------  
| SUB: Back Off and stop melee  New Version
| ----------------------------------------------------------------------------      
	Sub Bind_BackOff  
		/echo Resetting. No Longer Backing off.  
		/if (${Target.ID}) /target clear
		/if (${Me.Combat}) /squelch /attack off 
		/if (${Stick.Active}) /stick off 
		/call AutoCampCheck
		/doevents flush Event_BackOff  
	/return 
| ----------------------------------------------------------------------------
| SUB: Event Camping - end macro
| ----------------------------------------------------------------------------
    Sub Event_Camping	
		/if (${Me.Casting.ID}) /stopcast
	    /end
    /return
| ----------------------------------------------------------------------------
| SUB: Campfire ${Window[FellowshipWnd].Child[FP_CampPage].Child[FP_CampsiteViewer].Text}
| ----------------------------------------------------------------------------
    Sub Bind_Campfire
    /if ((${Me.Fellowship.Campfire} && (${Select[${Me.Fellowship.CampfireZone},${Zone.Name}]} || ${Me.Fellowship.CampfireZone.Name.Find[guild hall]})) || (${Me.InInstance}==FALSE)) /return
        /declare FellowCount int local 0
        /declare i int local
        /declare j int local
        /for i 1 to ${SpawnCount[pc radius 50]}
            /for j 1 to ${Me.Fellowship.Members}
                /if (${NearestSpawn[${i},PC radius 50].CleanName.Equal[${Me.Fellowship.Member[${j}]}]}) /varcalc FellowCount ${FellowCount}+1
            /next j
        /next i
        /if (${FellowCount}>=3) {
            /windowstate FellowshipWnd open
			/delay 10
			/nomodkey /notify FellowshipWnd FP_Subwindows tabselect 2
			/nomodkey /notify FellowshipWnd FP_DestroyCampsite leftmouseup
			/delay 5s ${Window[ConfirmationDialogBox].Open}
			/if (${Window[ConfirmationDialogBox].Open}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
			/delay 5s !${Me.Fellowship.Campfire}
			/delay 1s
			/nomodkey /notify FellowshipWnd FP_RefreshList leftmouseup
			/delay 1s        
			/nomodkey /notify FellowshipWnd FP_CampsiteKitList listselect 1
			/delay 1s
			/nomodkey /notify FellowshipWnd FP_CreateCampsite leftmouseup
			/delay 5s ${Me.Fellowship.Campfire}
			/windowstate FellowshipWnd close
			/if (${Me.Fellowship.Campfire}) /echo Campfire Dropped
        }
    /return
| ----------------------------------------------------------------------------
| SUB: Event Zoned
| ----------------------------------------------------------------------------
    Sub Event_Zoned(Message)
		/call Zoning
	/return
	
	Sub Zoning
		/delay 1m !${Me.Zoning}
		/if (${Bool[${FollowToonName}]}) /delay 1m ${Spawn[${FollowToonName}].ID}
		/delay 50
		/if (${ReturnToCamp}!=0) /varset ReturnToCamp 0
	/return
|----------------------------------------------------------------------------
| SUB: Load Ini
|----------------------------------------------------------------------------
    Sub LoadIni(string sectionName, string varName, string varType, string varValue)
     
		/if (!${Defined[${varName}]} && ${Defined[varType]}) /declare ${varName} ${varType} outer 0
		/if (!${Ini[${IniFileName},${sectionName},${varName}].Length}) {
			/if (${varValue.Length}) {
				/ini "${IniFileName}" "${sectionName}" "${varName}" "${varValue}"
				/varset ${varName} ${varValue}
				/echo set ${varName} ${varValue}
			}
		} else {
			/varset ${varName} ${Ini[${IniFileName},${sectionName},${varName}]} 
			/echo set ${varName} ${varValue}
		}

    /return

|----------------------------------------------------------------------------
| SUB: Set Ini
|----------------------------------------------------------------------------
    Sub SetIni(string sectionName, string varName, string varType, string varValue)
        /if (${varValue.Length}) {
            /ini "${IniFileName}" "${sectionName}" "${varName}" "${varValue}"
        }
    /return
|----------------------------------------------------------------------------
| SUB: FollowToon
|---------------------------------------------------------------------------- 
Sub FollowToon
/if (${Spawn[pc ${FollowToonName}].ID}) {
	/if (${Me.Dead}) /return
	/if (${Spawn[pc ${FollowToonName}].Distance}>20 && !${Spawn[pc ${FollowToonName}].Dead} && ${Spawn[pc ${FollowToonName}].Distance}<400) {
		 /squelch /afollow spawn ${Spawn[pc ${FollowToonName}].ID}
		 /delay 2s ${Spawn[pc ${FollowToonName}].Distance}<20
		 /if (${Spawn[pc ${FollowToonName}].Distance}<20) /squelch /afollow off
	}
}
/return
|----------------------------------------------------------------------------
| SUB: AutoCampCheck
|----------------------------------------------------------------------------
Sub AutoCampCheck
|----Camp Return Logic
	/if (${Spawn[${FollowToonName}].ID}&&(!${Me.Casting.ID})&&!${MoveTo.Moving}&&${ReturnToCamp}==1) {
		/varset ReturnToCamp 0
		/echo FollowToon Detected - Disabling Camp
	} else /if (${Stick.Status.Equal[ON]}&&${ReturnToCamp}==1) {
		/varset ReturnToCamp 0
		/echo Stick Detected - Disabling Camp
	} else /if (${AdvPath.Following}&&${ReturnToCamp}==1) {
		/varset ReturnToCamp 0
		/echo AdvPath Follow Detected - Disabling Camp
	} else /if (!${Spawn[${FollowToonName}].ID}&&${Stick.Status.Equal[OFF]} && !${Me.Casting.ID} && ${Math.Distance[${Me.Y},${Me.X}:${AutoCampY},${AutoCampX}]}>${AutoCampRadius} && (${Math.Distance[${Me.Y},${Me.X}:${AutoCampY},${AutoCampX}]}<400) && !${MoveTo.Moving} && ${ReturnToCamp}==1) {
		/moveto loc ${AutoCampY} ${AutoCampX}|on
		:movinghome
		/delay 5
		/if ((${Math.Distance[${Me.Y},${Me.X}:${AutoCampY},${AutoCampX}]}>20)&&(${MoveTo.Moving})) /goto :movinghome
		/moveto off
	}
/return
|----------------------------------------------------------------------------
| SUB: BagModRods
|----------------------------------------------------------------------------
Sub BagModRods
	/if ((${Cursor.ID})&&${CursorTimer}==0&&${ShitOnCursor.Equal[NULL]}&&(${Me.FreeInventory}>1)) {
		/varset ShitOnCursor ${Cursor}
		/varset CursorTimer 1m
	} else /if (${Cursor.ID}&&${CursorTimer}==0&&${ShitOnCursor.NotEqual[NULL]}&&(${Me.FreeInventory}>1)) {
		/echo Something has been left on cursor for over a minute Bagging it
		/autoinventory
		/varset ShitOnCursor NULL
	} else /if (${Cursor.ID}&&${ShitOnCursor.Find[Transvergence]}&&(${Me.FreeInventory}>1)) {
		/echo Bagging Mod Rod
		/autoinventory
		/varset ShitOnCursor NULL
	} else /if (${Cursor.ID}&&${ShitOnCursor.Find[Modulation]}&&(${Me.FreeInventory}>1)) {
		/echo Bagging Mod Rod
		/autoinventory
		/varset ShitOnCursor NULL
	}
/return
|----------------------------------------------------------------------------
| SUB: Event Gain Something - EQBC message 
|----------------------------------------------------------------------------    
    Sub Event_GainSomething(string Line,string text)
        /if (!${EQBC.Connected}) /return
        /if (${Line.Find["ABILITY POINT!"]}) {
            /docommand /bc [+w+]${Me.Name} gained an AA, now has ${Me.AAPoints} unspent[+x+]
        } else /if (${Line.Find[LEVEL]}) {
            /docommand /bc [+w+]${Me.Name} gained a level, now is Level ${Me.Level}[+x+]
		}
    /return 
|----------------------------------------------------------------------------
| SUB: Event Task Update - EQBC message 
|----------------------------------------------------------------------------    
    Sub Event_TaskUpdate(string Line,string name) 
        /if (${EQBC.Connected}&&${acverbose}!=0) /docommand /bc [+t+]Task updated...(${name})[+x+]    
    /return
|----------------------------------------------------------------------------
| SUB: Check Plugin
|----------------------------------------------------------------------------
    Sub CheckPlugin(string pluginname)
        /if (!${Bool[${Plugin[${pluginname}]}]}) {
            /squelch /plugin ${pluginname}
            /echo ${pluginname} not detected! This macro requires it! Loading ...
        }
    /return	
|----------------------------------------------------------------------------
| SUB: Death Sub
|----------------------------------------------------------------------------
Sub DeathSub
	/echo You are sleeping with the fishes
	/if (${ReturnToCamp}!=0) /varset ReturnToCamp 0
    :DeathSubwait
	/if (${Window[RespawnWnd].Open} && ${UseFellowship}==1 && ${InstantRelease}==1) {
		/nomodkey /notify RespawnWnd RW_OptionsList listselect 1 
		/delay 1s
		/nomodkey /notify RespawnWnd RW_SelectButton leftmouseup
		/delay 5s ${Me.Zoning}
	}
    /if (${Me.Hovering}) /goto :DeathSubwait
	/delay 1m !${Me.Zoning}
	/if (${UseFellowship}==1 && ${FindItem["Fellowship Registration Insignia"].Timer}==0) {
		/delay 30s ${Me.CombatState.Equal[ACTIVE]}
		/useitem "Fellowship Registration Insignia"
		/delay 2s ${FindItem["Fellowship Registration Insignia"].Timer}!=0
	} else /if (${UseFellowship}==1 && ${FindItem["Fellowship Registration Insignia"].Timer}!=0) {
		/echo Bummer, Insignia on cooldown, you must really suck at this game...
	} else {
		/echo Waiting on someone that knows how to heal to rez me
	}
/return
|----------------------------------------------------------------------------
| SUB: SitCheck
|---------------------------------------------------------------------------- 
Sub SitCheck
|---SIT Check
   /if ((${Me.Moving} || ${Me.XTarget[1].ID} || ${MoveTo.Moving} || ${Me.CombatState.Equal[COMBAT]} || ${Stick.Status.Equal[ON]} || ${AdvPath.Following}) && (${enablesit}==1)) /varset enablesit 0
|--Out of Combat MedTime - Sitting
	/if (${Me.Standing} && !${Me.Moving} && ${Stick.Status.Equal[OFF]} && !${Me.XTarget[1].ID} && !${MoveTo.Moving} && !${AdvPath.Following} && ${Me.CombatState.NotEqual[COMBAT]}&&${Me.CombatState.NotEqual[DEBUFFED]}&&${enablesit}==0&&(${Me.PctMana}<=80&&${Me.MaxMana}>1000 || ${Me.PctEndurance}<=80)) {
		/varset enablesit 1
	} else /if (${sittimer}==0&&${enablesit}==0&&${Me.Sitting}&&(${Me.Moving} || ${Me.XTarget[1].ID} || ${MoveTo.Moving} || ${Me.CombatState.Equal[COMBAT]} || ${Stick.Status.Equal[ON]} || ${AdvPath.Following})) {
		/stand
		/varset sittimer 10s
	} else /if (${sittimer}==0&&${enablesit}==1&&!${Me.Sitting}) {
		/sit
		/varset sittimer 10s
	}

/return
|----------------------------------------------------------------------------
| SUB: UnCheck Plugin
|----------------------------------------------------------------------------
    Sub UnCheckPlugin(string pluginname)
        /if (${Bool[${Plugin[${pluginname}]}]}) {
            /squelch /plugin ${pluginname} unload
            /echo ${pluginname} detected! Unloading it before it fucks shit up! UnLoading ...
        }
    /return	
|----------------------------------------------------------------------------
| SUB: DiscQueue
|---------------------------------------------------------------------------- 
Sub DiscQueue(string nextdisc)
	/if (!${Me.Standing}) /stand
	/delay 10 ${Me.Standing}
	/if (${Window[SpellBookWnd]}) /book
	/if (${Me.ActiveDisc.ID}) /stopdisc
	/delay 20 !${Me.ActiveDisc.ID}
	/squelch /disc ${nextdisc}
	/delay 20 ${Me.ActiveDisc.ID}
	/delay 8 !${Me.CombatAbilityReady[${nextdisc}]}
	/echo Casting ${nextdisc}
/return
|----------------------------------------------------------------------------
| SUB: DiscNow
|---------------------------------------------------------------------------- 
Sub DiscNow(string nextdisc)
	/if (${Window[CastingWindow].Open}) /return
	/if (!${Me.Standing}) /stand
	/delay 10 ${Me.Standing}
	/if (${Window[SpellBookWnd]}) /book
	/squelch /disc ${nextdisc}
	/delay 8 !${Me.CombatAbilityReady[${nextdisc}]}
	/echo Casting ${nextdisc}
/return
|----------------------------------------------------------------------------
| SUB: AANow
|---------------------------------------------------------------------------- 
Sub AANow(int nextaa,int aatargetid)
	/if (${Spawn[ID ${aatargetid}].FeetWet}!=${Me.FeetWet}&&${Spawn[id ${aatargetid} npc].ID}&&${Window[CastingWindow].Open}) /return
	/if (!${Me.Standing}) /stand
	/delay 10 ${Me.Standing}
	/if (${Window[SpellBookWnd]}) /book
	/if (${Window[CastingWindow].Open}) /interrupt
	/delay 5 !${Window[CastingWindow].Open}
	/if (${Target.ID}!=${aatargetid}&&${Spawn[id ${aatargetid} npc].ID}) {
		/target id ${aatargetid}
		/delay 10 ${Target.ID}==${aatargetid}
	}
	/squelch /alt act ${nextaa}
	/delay 8 !${Me.AltAbilityReady[${nextaa}]}
	/echo Casting ${Me.AltAbility[${nextaa}].Name}
/return
|----------------------------------------------------------------------------
| SUB: AbilityNow
|---------------------------------------------------------------------------- 
Sub AbilityNow(string nextability)
	/squelch /doability ${nextability}
	/delay 8 !${Me.AbilityReady[${nextability}]}
	/echo Casting ${nextability}
/return
|----------------------------------------------------------------------------
| SUB: ItemNow
|---------------------------------------------------------------------------- 
Sub ItemNow(string nextitem)
	/if (${Window[CastingWindow].Open}) /return
	/if (!${Me.Standing}) /stand
	/delay 10 ${Me.Standing}
	/if (${Window[SpellBookWnd]}) /book
	/if (${Window[CastingWindow].Open}) /interrupt
	/delay 5 !${Window[CastingWindow].Open}
	/useitem ${nextitem}
	/delay 15 !${Me.ItemReady[${nextitem}]}
|--Checks if item is used and does it again if not
	/if (${Me.ItemReady[${nextitem}]}&&${FindItem[${nextitem}].ID}) {
		/useitem ${nextitem}
		/delay 15 !${Me.ItemReady[${nextitem}]}
	}
	/echo Using Item ${nextitem}
/return

|----------------------------------------------------------------------------
| SUB: SpellQueue
|---------------------------------------------------------------------------- 
Sub SpellQueue(string nextspell,int nextid)
	/if (${Me.CurrentMana}<${Spell[${nextspell}].Mana}) /return
	/if (!${Me.Book[${nextspell}]}) /return
	/if (${Me.Sitting}) /stand
	/if (${Window[SpellBookWnd]}) /book
	
	/if (!${Me.Gem[${nextspell}]}) { 
		/delay 1s !${Me.Casting.ID}
		/call MemSpells "${nextspell}" ${spellmisc}
		/if (!${Me.XTarget[1].ID}) {
			/delay 5s ${Me.GemTimer[${nextspell}]}==0
			/delay 5s ${Me.SpellReady[${nextspell}]}
		}
    }

	/if (${Me.SpellReady[${nextspell}]}) {
	/if (${Target.ID}!=${nextid}) {
		/if (${Me.Combat}) /squelch /attack off
		/target id ${nextid}
		/delay 10 ${Target.ID}==${nextid}
		/if (${Target.FeetWet}!=${Me.FeetWet}) /return
		/if (${Target.Hovering}||${Target.Type.Equal[Corpse]}) /return
	}
	/cast "${nextspell}"
	/delay 10 ${Window[CastingWindow].Open}
	/delay ${Math.Calc[(${Spell[${nextspell}].MyCastTime}*10)+15]} !${Me.Casting.ID}
	/if ((!${EQBC.Connected} || !${Bool[${Plugin[MQ2EQBC]}]})&&(${Cast.Result.Equal[CAST_SUCCESS]})) /docommand /echo [+o+]${nextspell} =>> ${Spawn[ID ${nextid}].Name} <<=[+x+]
	/if ((${EQBC.Connected}&&${acverbose}!=0)&&(${Cast.Result.Equal[CAST_SUCCESS]})) /docommand /bc [+o+]${nextspell} =>> ${Spawn[ID ${nextid}].Name} <<=[+x+]
	}
/return

|----------------------------------------------------------------------------
| SUB: AutoVersionCheck
|---------------------------------------------------------------------------- 
Sub AutoVersionCheck(int autoversion)
	/if (${autoversion}!=6) {
		/echo AUTOSUBS VERSION MISMATCH
		/echo download new autosubs.inc - ending macro
	}
/return
|----------------------------------------------------------------------------
| SUB: FindTarget
|---------------------------------------------------------------------------- 
Sub FindTarget
/declare xt int local 1
/declare xtlow int local ${AutoAssistAt}
/declare xtid int local 0
	/if (!${Spawn[npc id ${autotargetid}].ID}&&${autotargetid}!=0) /varset autotargetid 0
|--Query XTarget to find hp percents
	/for xt 1 to ${Me.XTarget}
		/if (${Me.XTarget[${xt}].PctHPs}<=${xtlow}&&${Spawn[id ${Me.XTarget[${xt}].ID} npc].ID}) {
			/varset xtlow ${Me.XTarget[${xt}].PctHPs}
			/varset xtid ${Me.XTarget[${xt}].ID}
		}
	/next xt
|--Check target matches MA if not based off lowest hp in XTarget
	/if (${Target.ID}!=${Me.GroupAssistTarget.ID} && ${Group.MainAssist.ID}!=${Me.ID} && ${autotargetid}!=${Me.GroupAssistTarget.ID} && ${Spawn[id ${Me.GroupAssistTarget.ID} npc].ID}) {
		/varset autotargetid ${Me.GroupAssistTarget.ID}
	} else /if ((!${Group.MainAssist.ID}||${Group.MainAssist.ID}==${Me.ID})&&${Me.XTarget[1].ID}&&${Spawn[NPC id ${xtid}].ID}) {
		/varset autotargetid ${xtid}
	}
|--Target the new target
	/if (${Target.ID}!=${autotargetid} && ${Spawn[npc id ${autotargetid}].ID}) {
		/squelch /target id ${autotargetid}
		/delay 10 ${Target.ID}==${autotargetid}
	}
|--Determine Engage	
	/if (!${Me.Combat}&&${Me.XTarget[1].ID}&&${UseMelee}==1&&${Spawn[npc id ${autotargetid}].PctHPs}<=${AutoAssistAt}&&${Spawn[npc id ${autotargetid}].PctHPs}>0&&${Target.ID}==${autotargetid}&& ${Spawn[id ${autotargetid} npc los radius ${AutoCampRadius}].ID}) {
		/squelch /attack on
		/delay 10 ${Me.Combat}
	}
/return
|----------------------------------------------------------------------------
| SUB: Engage Target
|---------------------------------------------------------------------------- 
Sub EngageTarget
	/if (${Me.State.Equal[FEIGN]}) /stand
	/if (${Me.Combat}&&${Me.XTarget[1].ID}&&${UseMelee}==1&&${Spawn[npc id ${autotargetid}].PctHPs}>0&&${Target.ID}==${autotargetid}&& ${Stick.Status.Equal[OFF]}) {
		/if (${Math.Calc[${Target.MaxRangeTo}*0.8]} > 29) {
			/stick 25 moveback loose uw
		} else {
			/stick ${Math.Calc[${Target.MaxRangeTo}*0.8]} moveback loose uw
		}
		/delay 10 ${Stick.Active}
	}
/return
|----------------------------------------------------------------------------
| SUB: FindNumTargets
|---------------------------------------------------------------------------- 
Sub FindNumTargets
/declare xt int local 1
/if (!${Defined[numoftargets]}) /declare numoftargets int outer 0
/varset numoftargets 0
|--Query XTarget to find npcs
	/for xt 1 to ${Me.XTarget}
		/if (${Me.XTarget[${xt}].Type.Equal[NPC]}) /varcalc numoftargets ${numoftargets}+1
	/next xt
/return
|----------------------------------------------------------------------------
| SUB: MemSpells
|---------------------------------------------------------------------------- 
Sub MemSpells(string memspell,int memgem)
	/if (${Me.Casting.ID}) /stopcast
	/delay 2s !${Me.Casting.ID}
	/if (${spellmisc}<${memgem}) {
			/echo Macro attempting memorize more songs than spell slots availiable. Ending.
			/end
	}
	/echo Memorizing ${memspell}
	/memspell ${memgem} "${memspell}"
	/delay 25
/return