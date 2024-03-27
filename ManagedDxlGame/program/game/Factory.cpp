#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
#include <list>
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "GameManager.h"
#include "ObjectManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "EnemyManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "MyCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
//-------------------------------------------------------
//Others
#include "Debug.h"





//------------------------------------------------------------------------------------------------------------
//デストラクタ
Factory::~Factory() 
{
	delete game_manager;
}

//------------------------------------------------------------------------------------------------------------
//受注を受けてプレイヤーを生成する関数
void Factory::GeneratePlayer(PlayerSpawn now_spawn, tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, std::list<std::shared_ptr<Character>>& chara_list)
{
	//スポーンがスターとマップなら
	if (now_spawn == PlayerSpawn::STARTMAP) {
		player = std::make_shared<Player>(start_pos,
			//arg1…Position
			//arg2…Hp
			//arg3…Mp(WIP)
			//arg4…Attack
			//arg5…Defence
			//arg6…GraphicHandle
			hp, mp, attack, defence, name, chara_ph, exp, level, game_manager);
	}
	//ダンジョンなら
	else if (now_spawn == PlayerSpawn::DUNGEON) {
		//プレイヤーの初期化
		player = std::make_shared<Player>(start_pos,
				//arg1…Position
				//arg2…Hp
				//arg3…Mp
				//arg4…Attack
				//arg5…Defence
				//arg6…GraphicHandle
				hp, mp, attack, defence, name, chara_ph, exp, level, game_manager);
	}
	chara_list.emplace_back(player);

	SetPlayer(player);

}

//------------------------------------------------------------------------------------------------------------
//受注を受けて敵をする関数
void Factory::GenerateEnemy(tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, std::list<std::shared_ptr<Character>>& chara_list, std::list<std::shared_ptr<Enemy>>& ali_ene)
{
	enemy = std::make_shared<Enemy>(start_pos, hp, mp, attack, defence, name, chara_ph, exp, level, game_manager);
	ali_ene.emplace_back(enemy);
	chara_list.emplace_back(enemy);
}

//------------------------------------------------------------------------------------------------------------
//受注を受けてアイテムを生成する関数
void Factory::GenerateItem(tnl::Vector3 item_pos, int item_stetus, std::string item_name, std::string item_handle, std::string item_desctiption, int item_price, std::list<std::shared_ptr<Object>>& obj_list, std::list<std::shared_ptr<Item>>& ali_item)
{
	item = std::make_shared<Item>(item_pos, item_stetus, item_name, item_handle, item_desctiption, item_price, game_manager);
	obj_list.emplace_back(item);
	ali_item.emplace_back(item);

}

//------------------------------------------------------------------------------------------------------------
//すべてのアイテムを生成する
void Factory::AllItemCreate(tnl::Vector3 item_pos, int item_stetus, std::string item_name, std::string item_handle, std::string item_desctiption, int item_price, std::vector<std::shared_ptr<Item>>& all_item)
{
	
	item = std::make_shared<Item>(item_pos, item_stetus, item_name, item_handle, item_desctiption, item_price, game_manager);
	//すべてのアイテムを格納する
	all_item.emplace_back(item);

}

//------------------------------------------------------------------------------------------------------------
//マップチップをインスタンス化する
void Factory::GenerateMapChip(tnl::Vector3 old_chip_pos, int chip_handle, std::list<std::shared_ptr<MapChip>>& all_chip_list, std::list<std::shared_ptr<MapChip>>& wall_list, bool is_wall)
{

	mapchip = std::make_shared<MapChip>(old_chip_pos, chip_handle);
	
	all_chip_list.emplace_back(mapchip);

	//インスタンス化をするマップチップがwallなら、wall_listにも追加する
	if (is_wall) {
		wall_list.emplace_back(mapchip);
	}

}






