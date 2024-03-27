//-------------------------------------------------------
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
#include <variant>
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "ObjectManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "SkillManager.h"
#include "SoundManager.h"
#include "ItemManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "Player.h"
#include "Enemy.h"
#include "MyCamera.h"
#include "MenuWindow.h"
#include "Item.h"
//-------------------------------------------------------
//Collision
#include "Collision.h" 
//-------------------------------------------------------
//others
#include "Stetus.h"
#include "Debug.h"
#include "Skill.h"
#include "Inventory.h"

Inventory::Inventory(int my_inventory_value_, GameManager* game_manager_)
{
	game_manager = game_manager_;
	cursor_handle = game_manager->LoadGraphEx("graphics/gamecursor.png");
	select_item_back_handle = game_manager->LoadGraphEx("testgraphics/selectItemBack.png");
	my_inventory_value = my_inventory_value_ + 1;

}

Inventory::~Inventory()
{
}

//インベントリに引数のアイテムを追加する関数
void Inventory::AddInventory(std::shared_ptr<Item> item)
{
	//現在のインベントリに入る最大数(max_inventoryと変数化したほうがいいかも)
	if (inventory_list.size() >= 10)return;
	//インベントリにアイテムを追加
	inventory_list.emplace_back(item);
	//アイテム数も追加
	item_value++;

}


//カーソルの上下移動処理
void Inventory::CursorMove()
{
	//カーソルの上下移動
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_UP)) {
		item_value = GetItemCount();
		//カーソル移動したときのseを鳴らす
		game_manager->GetSoundManager()->ChosePlaySystemSound(game_manager->GetSoundManager()->sound_csv[11]);
		select_cursor = (select_cursor + (item_value - 1)) % item_value;
	
	}
	//下
	else if (tnl::Input::IsKeyDownTrigger(eKeys::KB_DOWN)) {
		item_value = GetItemCount();
		//カーソル移動したときのseを鳴らす
		game_manager->GetSoundManager()->ChosePlaySystemSound(game_manager->GetSoundManager()->sound_csv[11]);
		select_cursor = (select_cursor + 1) % item_value;

	}
}

//カーソルの位置を一番上にリセットする関数
void Inventory::CursorReset()
{
	select_cursor = 0;
}


//インベントリ内のアイテム名を描画する関数
void Inventory::DrawInventory(const int x, const int y)
{
	int i = 0;
	for (auto item : inventory_list) {
		//アイテム名の描画
		DrawStringEx(x + 80, y + 10 + 30 * i, -1, "%s", item->GetItemName().c_str());
		i++;
	}
	DrawRotaGraph(x + 30, y + 20 + select_cursor * 30, 0.7, 0, cursor_handle, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 30);
	DrawRotaGraph(x + 220, y + 20 + select_cursor * 30, 1, 0, select_item_back_handle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);


}


//アイテムの説明を描画する関数
void Inventory::DrawItemDesc(const int x, const int y)
{
	SetFontSize(14);

	//インベントリ内のアイテムがなければ処理スルー
	if (inventory_list.empty())return;
	auto item_desc = inventory_list.begin();
	for (int i = 0; i < select_cursor; i++) {
		if (item_desc == inventory_list.end()) {
			break;
		}
		item_desc++;
	}
	//アイテム説明を描画
	(*item_desc)->DrawItemStringData(x, y);
}


//ショップインベントリ用(購入額、売却額)
void Inventory::DrawNeedCoin(int x, int y)
{
	if (inventory_list.empty())return;
	auto item = inventory_list.begin();
	for (int i = 0; i < select_cursor; i++) {
		//イテレータが最後のコンテナにいるなら
		if (item == inventory_list.end()) {
			break;
		}
		item++;
	}
	//必要コイン数
	int need_coin = 0;
	//アイテムの価格取得
	need_coin = (*item)->GetItemPrice();
	SetFontSize(25);
	DrawStringEx(x + 10, y + 10, -1, "購入必要コイン:%d", need_coin);
	DrawStringEx(x + 10, y + 60, -1, "所持コイン:%d", game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::GOLD));
	SetFontSize(16);
}

int Inventory::GetCursorValue()
{
	return select_cursor;
}

//カーソルの位置をセットする
void Inventory::SetCursorValue(const int move_num)
{
	if (select_cursor != inventory_list.size())return;
	select_cursor += move_num;
	if (select_cursor < 0)select_cursor = 0;
}

