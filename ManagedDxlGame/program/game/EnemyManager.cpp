//------------------------------------------------------ -
//既存のもの
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//個人的に追加した機能
#include <string_view>
#include <random>         
#include <iostream>  
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "SceneManager.h"
#include "GameManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
#include "EnemyManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "Character.h"
#include "MyCamera.h"
#include "Enemy.h"
#include "Player.h"
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Debug.h"


//------------------------------------------------------------------------------------------------------------
//コンストラクタ
EnemyManager::EnemyManager(std::vector<std::vector<int>>& re_stetus, std::vector<std::vector<std::string>>& re_graphic, GameManager* game_manager_)
{

	game_manager = game_manager_;
	
	//re_graphicに格納されているstring型の数値は使用しない
	//引数で渡された2種類のvector配列を1つにまとめる	
	ene_info.resize(re_stetus.size() + 2);
	for (size_t i = 0; i < re_stetus.size(); i++) {
		
		ene_info[i] = std::make_pair(re_stetus[i], re_graphic[i]);
		
	}

}

//------------------------------------------------------------------------------------------------------------
//毎フレーム実行
void EnemyManager::Update(float delta_time)
{
	//Hpが０の敵を生存リストから削除
	//DeadEnemyErase();
}

