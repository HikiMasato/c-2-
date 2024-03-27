//-------------------------------------------------------
//既存のもの
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
//Manager
#include "GameManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "EffectManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MyCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
//-------------------------------------------------------
//Effect
#include "ActionEffect.h" 
//-------------------------------------------------------
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Debug.h"


#if 0
///
///             ****************使用例*****************
/// 
//Character* charcter;
//Item* potion;
//Item* power_up;
//
// 
// Item herb("薬草",  { target->heal(10); }); // 薬草のインスタンスを作る
// herb.Use(this); // 薬草を自分に使う
// 
// 
//int main()
//{
//    potion = new Item([&](Character* target) {
//        target->hp_ += 50;
//        });
//    power_up = new Item([&](Character* target) {
//        target->power_ += 5;
//        });
//
//    Item* p = potion->clone();
//
//    std::cout << "Hello World!\n";
//}

//void ItemManager::RandCreateItem()
//{
//	/*game_manager->map_manager->factory->potion = new Item([&](std::shared_ptr<Object> target) {
//			target->chara_hp += 50;
//	});
//	game_manager->map_manager->factory->apple = new Item([&](std::shared_ptr<Object> target) {
//			target->chara_attack += 5;
//	});
//	game_manager->map_manager->factory->apple = new Item([&](std::shared_ptr<Object> target) {
//			target->chara_attack += 5;
//	});*/
//
//}
// 
#endif // 0


 
//------------------------------------------------------------------------------------------------------------
//コンストラクタ
Item::Item(tnl::Vector3 item_pos, int item_stetus, std::string item_name, std::string item_handle, std::string item_desctiption, int item_price, GameManager* game_manager_)
{
	get_item_stetus = item_stetus;
	get_item_name = item_name;
	get_item_desc = item_desctiption;
	game_manager = game_manager_;

	item_desc[0] = get_item_name;
	item_desc[1] = get_item_desc;
	get_st_item_handle = item_handle;
	//get_item_pos = item_pos;
	get_item_handle = game_manager->LoadGraphEx(item_handle);
	get_item_price = item_price;
	SetItemPos(item_pos);

}

//------------------------------------------------------------------------------------------------------------
//デストラクタ
Item::~Item()
{
	tnl::DebugTrace("\n===================Itemのデストラクタ処理===================\n");
}

//------------------------------------------------------------------------------------------------------------
//描画
void Item::Draw(const hm::Camera& camera)
{
	DrawRotaGraph(get_item_pos.x - camera.cameraPos.x, get_item_pos.y - camera.cameraPos.y, game_manager->GetRatio(), 0, get_item_handle, true);
}

//------------------------------------------------------------------------------------------------------------
//アイテムのポジションを設定
void Item::SetItemPos(tnl::Vector3 pos)
{
	get_item_pos = pos;
	item_disc_pos = game_manager->WorldToLocalPos(pos);
}

//------------------------------------------------------------------------------------------------------------
//踏んでいたらtrue
bool Item::DeleteCheckOnPlayer(tnl::Vector3 pos)
{
	tnl::Vector3 now_item_pos = game_manager->WorldToLocalPos(get_item_pos);
	if (now_item_pos.x == pos.x && now_item_pos.y == pos.y) {
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//アイテムの説明描画
void Item::DrawItemStringData(int x, int y)
{
	DrawStringEx(x, y, -1, "%s", item_desc[0].c_str());
	DrawStringEx(x, y + 30, -1, "%s", item_desc[1].c_str());

}

//------------------------------------------------------------------------------------------------------------
//アイテムの生存フラグを返す関数
bool Item::GetIsAlive()
{
	return is_alive_item;
}

//------------------------------------------------------------------------------------------------------------
//アイテムの生存フラグを下す
bool Item::SetIsAliveFalse()
{
	return is_alive_item = false;
}

