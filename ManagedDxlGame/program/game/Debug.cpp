//-------------------------------------------------------
//Šù‘¶‚Ì‚à‚Ì
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "MyCamera.h"
#include "Player.h"
//-------------------------------------------------------
//Manager
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameManager.h"
#include "MapManager.h"
//-------------------------------------------------------
//Others
#include "Debug.h"


//------------------------------------------------------------------------------------------------------------
//Update
void DebugManager::Update(float delta_time) {
	/*if (tnl::Input::IsKeyDownTrigger(eKeys::KB_F2)) {
		ptr->is_debug = true;
	}*/
}

//------------------------------------------------------------------------------------------------------------
//
std::string DebugManager::DebugString(std::string debugstring) {
	tnl::DebugTrace(debugstring.c_str());
	return debugstring;
}
