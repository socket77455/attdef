/*

	Example usage: gator3016.hostgator.com/~maarij94/tiger/script/ac2.txt

*/

#include <a_samp>
#include <a_http>

#if defined _sampac_api_included
	#endinput
#endif
#define _sampac_api_included

#if !defined _sampac_TIMERMODE

	#define _sampac_TIMERMODE 0

#endif

#define _sampac_DEBUG 0

#include <strlib>
#define QUERY_URL 			"gator3016.hostgator.com/~maarij94/AC_API/pInfo.php"
#define UPDATE_URL          "gator3016.hostgator.com/~maarij94/AC_API/update.php"
#define ALL_QUERY_URL       "gator3016.hostgator.com/~maarij94/AC_API/allpInfo.php"
#define THREAD_WAIT_URL     "gator3016.hostgator.com/~maarij94/AC_API/query_bug_thread_wait.php"
#define BAN_URL             "gator3016.hostgator.com/~maarij94/AC_API/ban.php"

#include <YSI\y_iterate>

#define VERSION 2.1

new AC_TIME_OFFSET = 30000;

#define THREAD_OFFSET 100
#define FILES 31

#if THREAD_OFFSET > MAX_PLAYERS
	#undef THREAD_OFFSET
	#define THREAD_OFFSET MAX_PLAYERS
#endif

new AC_Update;

new bool:ACRunning[MAX_PLAYERS];
new bool:HasASI[MAX_PLAYERS];
new bool:HasTrainer[MAX_PLAYERS];
new bool:samp_ac_Frozen[MAX_PLAYERS];
new bool:ignoreNext[MAX_PLAYERS];
new bool:ACOn;
new bool:checkedUpdate;

#if _sampac_TIMERMODE == 1
new ac_Timers[MAX_PLAYERS];
#endif

new bool:Files[MAX_PLAYERS][FILES];

stock bool:AC_Toggle(bool:set);

new fileNames[FILES][50] = {
	!"anim/ped.ifp",             // file_0
    !"data/carmods.dat",         // file_1
    !"data/animgrp.dat",         // file_2
    !"data/melee.dat",           // file_3
    !"data/clothes.dat",         // file_4
    !"data/object.dat",          // file_5
    !"data/default.dat",         // file_6
    !"data/surface.dat",         // file_7
    !"data/default.ide",         // file_8
    !"data/gta.dat",             // file_9
    !"data/surfinfo.dat",        // file_10
    !"data/peds.ide",            // file_11
    !"data/vehicles.ide",        // file_12
    !"data/water.dat",           // file_13
    !"data/txdcut.ide",          // file_14
    !"data/water1.dat",          // file_15
    !"models/coll/weapons.col",  // file_16
    !"data/plants.dat",          // file_17
    !"data/furnitur.dat",        // file_18
    !"data/procobj.dat",         // file_19
    !"data/handling.cfg",        // file_20
    !"models/coll/peds.col",     // file_21
    !"models/coll/vehicles.col", // file_22
    !"samp.saa",                 // file_23
    !"samp.exe",                 // file_24
    !"vorbis.dll",               // file_25
    !"data/pedstats.dat",        // file_26
    !"data/ar_stats.dat",        // file_27
    !"vorbisfile.dll",           // file_28
    !"gta_sa.exe",               // file_29
    !"samp.dll"                  // file_30
    //                              ^^ internal file names for AC

};


public OnFilterScriptInit()
{
	if(checkedUpdate == false) {
		HTTP(THREAD_OFFSET, HTTP_GET, UPDATE_URL, "", "UpdateResponse");
		checkedUpdate = true;
		AC_Update = SetTimer("AC_MainUpdate", AC_TIME_OFFSET, 1);
		ACOn = false;
	}
	return CallLocalFunction("samp_ac_OnFilterScriptInit", "");
}

#if defined _ALS_OnFilterScriptInit
    #undef OnFilterScriptInit
#else
    #define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit samp_ac_OnFilterScriptInit

forward samp_ac_OnFilterScriptInit();

public OnGameModeInit() {
    if(checkedUpdate == false) {
		HTTP(THREAD_OFFSET, HTTP_GET, UPDATE_URL, "", "UpdateResponse");
		checkedUpdate = true;
		AC_Update = SetTimer("AC_MainUpdate", AC_TIME_OFFSET, 1);
		ACOn = false;
	}
	return CallLocalFunction("samp_ac_OnGameModeInit", "");
}

#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit samp_ac_OnGameModeInit

forward samp_ac_OnGameModeInit();

public OnGameModeExit() {
	KillTimer(AC_Update);
	return CallLocalFunction("samp_ac_OnGameModeExit", "");
}

#if defined _ALS_OnGameModeExit
    #undef OnGameModeExit
#else
    #define _ALS_OnGameModeExit
#endif
#define OnGameModeExit samp_ac_OnGameModeExit

forward samp_ac_OnGameModeExit();

public OnFilterScriptExit() {
	KillTimer(AC_Update);
	return CallLocalFunction("samp_ac_OnFilterScriptExit", "");
}

#if defined _ALS_OnFilterScriptExit
    #undef OnFilterScriptExit
#else
    #define _ALS_OnFilterScriptExit
#endif
#define OnFilterScriptExit samp_ac_OnFilterScriptExit

forward samp_ac_OnFilterScriptExit();


forward UpdateResponse(index, code, data[]);
public UpdateResponse(index, code, data[]) {
    if(code != 200) {
        //HTTP(THREAD_OFFSET, HTTP_GET, UPDATE_URL, "", "UpdateResponse");
	    return printf("Whitetiger's Anti-cheat error: Failed to connect to website API");
	}
	#pragma unused index

	new Float:version = 0.0, url[128];

	new str[5][128];
	strexplode(str, data, ",");
	version = floatstr(str[0]);
	url = str[1];

	if(!strlen(url) || version == 0.0) {
	    return printf("Whitetiger's AC Error: Failed to check for an update.");
	}

	if(version > VERSION) {
	    printf("__________________________________________________");
	    printf("");
	    printf("** Your Whitetiger's Anti-Cheat API include is out of date, your old version may not continue to work.");
		printf("A new version is available at: %s", url);
		printf("");
		printf("__________________________________________________");

	} else if(!strcmp(url, "- Server not approved for Anti-Cheat usage.")) {
	    printf(data);
	    printf("__________________________________________________");
	    printf("");
	    printf("** Server not approved for Whitetiger's Anti-cheat Usage, Disabled.");
	    printf("");
	    printf("__________________________________________________");
	} else if(!strcmp(url, "Under Maintenance.")) {
	    printf("__________________________________________________");
	    printf("");
	    printf("** Whitetiger's Anti-Cheat is current under scheduled maintenance, it cannot be enabled at this time.");
	    printf("");
	    printf("__________________________________________________");
	    AC_Toggle(false);
	} else {
	    printf("__________________________________________________");
	    printf("");
	    printf("** Whitetiger's Anti-Cheat is on the most up-to-date version!");
	    printf("");
	    printf("__________________________________________________");
	}

	HTTP(THREAD_OFFSET + THREAD_OFFSET, HTTP_GET, THREAD_WAIT_URL, "", "ThreadWaitTime");
	return 1;
}

public OnPlayerConnect(playerid)
{
 	//HTTP(playerid + THREAD_OFFSET + THREAD_OFFSET, HTTP_POST, BAN_URL, postdata, "BanResponse");
   	if(ACOn == true) {
		AC_GetInfo(playerid);
		#if _sampac_TIMERMODE == 1
		ac_Timers[playerid] = SetTimerEx("AC_GetInfoTimer", AC_TIME_OFFSET, 0, "d", playerid);
		#endif
	}
   	return CallLocalFunction("samp_ac_OnPlayerConnect", "i", playerid);
}

#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect samp_ac_OnPlayerConnect

forward samp_ac_OnPlayerConnect(playerid);

public OnPlayerDisconnect(playerid, reason) {

	ACRunning[playerid] 		= false;
	HasASI[playerid] 			= false;
	HasTrainer[playerid] 		= false;
	samp_ac_Frozen[playerid]    = false;
	ignoreNext[playerid]        = false;

	for(new i=0; i < FILES; ++i) {
	    Files[playerid][i] = true;
	}

	#if _sampac_TIMERMODE == 1
	KillTimer(ac_Timers[playerid]);
	ac_Timers[playerid] = -1;
	#endif

	return CallLocalFunction("samp_ac_OnPlayerDisconnect", "ii", playerid, reason);
}

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect samp_ac_OnPlayerDisconnect

forward samp_ac_OnPlayerDisconnect(playerid, reason);

public OnPlayerSpawn(playerid) {

	samp_ac_Frozen[playerid] = false;

	return CallLocalFunction("samp_ac_OnPlayerSpawn", "i", playerid);
}

#if defined _ALS_OnPlayerSpawn
    #undef OnPlayerSpawn
#else
    #define _ALS_OnPlayerSpawn
#endif
#define OnPlayerSpawn samp_ac_OnPlayerSpawn

forward samp_ac_OnPlayerSpawn(playerid);

stock samp_ac_Controllable(playerid, frozen) {

	samp_ac_Frozen[playerid] = frozen == 0 ? true : false;
	ignoreNext[playerid] = true;

	SetPlayerVelocity(playerid, 0.0, 0.0, 0.0);
	if(IsPlayerInAnyVehicle(playerid)) SetVehicleVelocity(GetPlayerVehicleID(playerid), 0.0, 0.0, 0.0);
	else ClearAnimations(playerid);

	TogglePlayerControllable(playerid, frozen);
}

#if defined _ALS_TogglePlayerControllable
    #undef TogglePlayerControllable
#else
    #define _ALS_TogglePlayerControllable
#endif
#define TogglePlayerControllable samp_ac_Controllable

#if _sampac_TIMERMODE == 0
forward AC_MainUpdate();
public AC_MainUpdate() {
	if(ACOn == true) {
		AC_GetAllInfo();
	} else {
		KillTimer(AC_Update);
	}
}

#else
forward AC_GetInfoTimer(playerid);
public AC_GetInfoTimer(playerid) {
	if(!IsPlayerConnected(playerid)) return 1;

    AC_GetInfo(playerid);
	ac_Timers[playerid] = SetTimerEx("AC_GetInfoTimer", AC_TIME_OFFSET, 0, "d", playerid);

	return 1;
}

stock AC_EnableForPlayer(playerid) {
	ac_Timers[playerid] = SetTimerEx("AC_GetInfoTimer", AC_TIME_OFFSET, 0, "d", playerid);
}

stock AC_DisableForPlayer(playerid) {
	KillTimer(ac_Timers[playerid]);
}
#endif

stock AC_GetInfo(playerid) {

    new IP[MAX_PLAYER_NAME], url[128], postdata[50];
	GetPlayerIp(playerid, IP, sizeof(IP));

	#if _sampac_DEBUG == 1
	if(!strcmp(IP, "127.0.0.1", true)) {
		IP = "";
	}
	#endif

 	format(url, sizeof(url), QUERY_URL);
 	format(postdata, sizeof(postdata), "IP=%s", IP);
	HTTP(playerid + THREAD_OFFSET, HTTP_POST, url, postdata, "ACResponse");

	return 1;
}

stock AC_GetAllInfo() {
	new IP[MAX_PLAYER_NAME], postdata[256];

	foreach(new i : Player) {
		GetPlayerIp(i, IP, sizeof(IP));

		#if _sampac_DEBUG == 1
		if(!strcmp(IP, "127.0.0.1", true)) {
			IP = "";
		}
		#endif

		if(strlen(postdata) > 0) format(postdata, sizeof(postdata), "%s&IP%d=%s", postdata, i, IP);
		else format(postdata, sizeof(postdata), "IP%d=%s", i, IP);

		if(i % 5 == 0) {
		    HTTP(THREAD_OFFSET, HTTP_POST, ALL_QUERY_URL, postdata, "AllACResponse");
		    postdata[0] = '\0';
		}
	}
	if(postdata[0] != '\0') {
	    HTTP(THREAD_OFFSET, HTTP_POST, ALL_QUERY_URL, postdata, "AllACResponse");
	}
}

/*forward BanResponse(index, code, data[]);
public BanResponse(index, code, data[]) {
	new playerid = (index - THREAD_OFFSET) - THREAD_OFFSET;
	if(code != 200) {
	    new IP[MAX_PLAYER_NAME], postdata[50];
		GetPlayerIp(playerid, IP, sizeof(IP));

		#if _sampac_DEBUG == 1
		if(!strcmp(IP, "127.0.0.1", true)) {
			IP = "";
		}
		#endif
		format(postdata, sizeof(postdata), "IP=%s", IP);

		HTTP(playerid + THREAD_OFFSET + THREAD_OFFSET, HTTP_POST, BAN_URL, postdata, "BanResponse");
		return printf("Whitetiger's Anti-cheat error: Failed to connect to website API");
	}

	if(!strcmp(data, "1", true)) {
	    SetTimerEx("KickPlayer", 1, 0, "d", playerid); // 1 ms timer fix so player who gets kicked actually recieves this message
	    new str[128], name[MAX_PLAYER_NAME];
	    GetPlayerName(playerid, name, sizeof(name));
	    format(str, sizeof(str), "{FF0000}%s{FFFFFF} has been kicked from the server for being banned from Whitetiger's Anti-Cheat", name);
	    SendClientMessageToAll(-1, str);
	}
	return 1;
}

forward KickPlayer(playerid);
public KickPlayer(playerid) {
	Kick(playerid);
}*/

forward ACResponse(index, code, data[]);
public ACResponse(index, code, data[]) {
    if(code != 200 && ACOn) {
        // try again?
        //AC_GetInfo(index - THREAD_OFFSET);
	    return printf("Whitetiger's Anti-cheat error: Failed to connect to website API");
	}

	new playerid = index - THREAD_OFFSET;
	new fileid, Float:X, Float:Y, Float:Z;

	if(strlen(data) > 1) {

		if(!strcmp(data, "Under Maintenance")) {
		    printf("__________________________________________________");
		    printf("");
		    printf("** Whitetiger's Anti-Cheat is current under scheduled maintenance, it cannot be enabled at this time.");
		    printf("");
		    printf("__________________________________________________");
		    AC_Toggle(false);
		    return 1;
		}

		#if _sampac_DEBUG == 1
		printf(data);
		#endif

		new str[7][20];
		strexplode(str, data, ",");

		ACRunning[playerid] 			= strcmp(str[0], "false") == 0 ? false : strcmp(str[0], "0") == 0 ? false : true;

		if(ACRunning[index - THREAD_OFFSET] == false) {
		    CallLocalFunction("OnACUpdated", "d", playerid);

			#if _sampac_DEBUG == 1
			printf("FIRST CHECK!!! Called On AC Updated. on %d", playerid);
			printf("Results: ACRunning(): %d", ACRunning[playerid]);
			printf("Trainer: %d", HasTrainer[playerid]);
			printf("ASI: %d", HasASI[playerid]);
			#endif

			return 1;
		}

		HasASI[playerid] 				= strcmp(str[1], "false") == 0 ? false : strcmp(str[1], "0") == 0 ? false : true;
		HasTrainer[playerid] 			= strcmp(str[2], "false") == 0 ? false : strcmp(str[2], "0") == 0 ? false : true;

		/*for(new i=0; i < FILES; ++i) {

		    if(!strcmp(str[i+3], "0")) { // prevent anim/ped.ifp false kicks
				Files[index - THREAD_OFFSET][i] 	= true;
				continue;
			}
			Files[index - THREAD_OFFSET][i] 		= strcmp(str[i+3], "false") == 0 ? false : true;
		}*/

		fileid = strval(str[3]);
		X = floatstr(str[4]);
		Y = floatstr(str[5]);
		Z = floatstr(str[6]);

		if(samp_ac_Frozen[playerid] == true && ignoreNext[playerid] == false && ACRunning[playerid] == true && IsPlayerConnected(playerid) && CallLocalFunction("OnPlayerUpdate", "d", playerid) != 0) {

			new Float:X2, Float:Y2, Float:Z2;
			GetPlayerPos(playerid, X2, Y2, Z2);

			new string[128];
			new string2[128];
			new maxcharacters_mismatchable = 1;
			if(!IsPlayerInAnyVehicle(playerid)) {
				format(string, sizeof(string), "%0.5f %0.5f %0.5f", X, Y, Z);
				format(string2, sizeof(string2), "%0.5f %0.5f %0.5f", X2, Y2, Z2);
			} else {

			    new Float:X3, Float:Y3, Float:Z3;
				GetVehicleVelocity(GetPlayerVehicleID(playerid), X3, Y3, Z3);

				if(X3 == 0.0 || Y3 == 0.0 || Z3 == 0.0) { // prevent while in heli or vehicle is moving somehow

				    format(string, sizeof(string), "%0.2f %0.2f", X, Y);
					format(string2, sizeof(string2), "%0.2f %0.2f", X2, Y2);
					maxcharacters_mismatchable = 3;
				}
			}


			if(proper_strcmp(string, string2) > maxcharacters_mismatchable && strlen(string) > 0 && strlen(string2) > 0) {
			    // 2 PC trick
			    CallLocalFunction("OnUsingAnotherPC", "d", playerid);
			    samp_ac_Frozen[playerid] = false; // don't call again unless frozen again to avoid spam
			}
		}

		if(ignoreNext[playerid] == true) {
		    ignoreNext[playerid] = false;
		}
	} else {
	    ACRunning[playerid] 			= false;
	}

	// check if they're still connected when the HTTP thread finished.
	if(IsPlayerConnected(playerid)) {
		CallLocalFunction("OnACUpdated", "d", playerid);

		#if _sampac_DEBUG == 1

		printf("Called On AC Updated. on %d", playerid);
		printf("Results: ACRunning(): %d", ACRunning[playerid]);
		printf("Trainer: %d", HasTrainer[playerid]);
		printf("ASI: %d", HasASI[playerid]);
		printf("fileid: %d", fileid);

		#endif

		if(IsPlayerConnected(playerid) && ACRunning[playerid] == true && fileid != -1) {
	    	CallLocalFunction("OnACFileModified", "ds", playerid, fileNames[fileid]);
		}
		return 1;
	}
	return 0;
}

forward AllACResponse(index, code, data[]);
public AllACResponse(index, code, data[]) {
	new idx = strfind(data, "|"), lastidx = -1, strOut[256], playerid;
	do {
	    // Extract all params between last | and the next |
		strmid(strOut, data, lastidx + 1, idx);

		// set lastidx = to position of next |
		lastidx = idx;

		// extract playerid
		new str[5];
		new playerid_idx = strfind(strOut, ",");
		strmid(str, strOut, 0, playerid_idx);
		playerid = strval(str);

		// find next |
		idx = strfind(data, "|", false, lastidx+1);

		// check if player is still connected (they should be unless they disconnected between HTTP function and this function getting called)
		if(!IsPlayerConnected(playerid)) continue;

		// trim out playerid
		strdel(strOut, 0, playerid_idx+2);

		// call like a normal AC_GetInfo
		ACResponse(index + playerid, code, strOut);
	} while(idx != -1);
}

stock proper_strcmp(string1[], string2[]) {
	new len = strlen(string1) > strlen(string2) ? strlen(string1) : strlen(string2);
	new differences = 0;
	for(new i=0; i < len; ++i) {
	    if(string1[i] != string2[i]) differences++;
	}
	return differences;
}

#define AC_Running(%0) ACRunning[%0]
#define AC_HasTrainer(%0) HasTrainer[%0]
#define AC_ASI(%0) HasASI[%0]
#define AC_GetEnabled() ACOn

stock bool:AC_Toggle(bool:set) {
	ACOn = set;

	if(set) {
		#if _sampac_TIMERMODE == 0
	    AC_MainUpdate();
	    AC_Update = SetTimer("AC_MainUpdate", AC_TIME_OFFSET, 1);
	    #else
	    foreach(new i : Player) {
	        ac_Timers[i] = SetTimerEx("AC_GetInfoTimer", AC_TIME_OFFSET, 0, "d", i);
	    }
	    #endif
	} else {
	    #if _sampac_TIMERMODE == 0
		KillTimer(AC_Update);
		#else
		/*foreach(new i : Player) {
		    ac_Timers[i] = SetTimerEx("AC_GetInfoTimer", AC_TIME_OFFSET, 0, "d", i);
		}*/
		for(new i=0; i < MAX_PLAYERS; ++i) {
		    KillTimer(ac_Timers[i]);
		}
		#endif
	}

	CallLocalFunction("OnACToggled", "d", set);
}

forward ThreadWaitTime(index, code, data[]);
public ThreadWaitTime(index, code, data[]) {

    for (new i = 0, j = strlen(data); i < j; i++)
    {
        if (data[i] > '9' || data[i] < '0') return 0;
    }

    AC_TIME_OFFSET = strval(data) + 2500;
	return 1;
}

forward OnACToggled(bool:set);
forward OnACUpdated(playerid);
forward OnUsingAnotherPC(playerid);
forward OnACFileModified(playerid, file[]);

