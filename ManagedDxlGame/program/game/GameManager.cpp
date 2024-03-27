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
#include "SceneManager.h"
#include "ObjectManager.h"
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
#include "Character.h"
#include "MapChip.h"
#include "Player.h"
#include "Enemy.h"
#include "MyCamera.h"
#include "MenuWindow.h"
#include "Item.h"
//-------------------------------------------------------
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//others
#include "Stetus.h"
#include "Debug.h"
#include "Skill.h"
#include "Inventory.h"


//------------------------------------------------------------------------------------------------------------
//コンストラクタ
GameManager::GameManager() {
	
	//=================インスタンス化============================
	re_manager = std::make_shared<ResourceManager>(this);
	sound_manager = std::make_shared<SoundManager>(re_manager->sound_handle);
	//object生成
	object_manager = std::make_shared<ObjectManager>(re_manager->chara_stetus, re_manager->effect_graphics, re_manager->item_handle, this);

	debug_manager = new DebugManager();
	collision = std::make_shared<Collision>();

}

GameManager::~GameManager()
{	
	if (!inventories.empty()) {
		for (int i = 0; i < inventories.size(); ++i) {
			delete inventories[i];
		}
	}
	InitGraph();
}

//------------------------------------------------------------------------------------------------------------
//毎フレーム実行
void GameManager::update(float delta_time) {

	object_manager->Update(delta_time);
}

//------------------------------------------------------------------------------------------------------------
//描画関数
void GameManager::Draw() {
	//全体のマップを描画
	map_manager->MapDraw(object_manager->factory->camera, now_gh_size, ratio);
	//ミニマップの描画
	map_manager->MiniMapDraw(object_manager->factory->camera);
	
	object_manager->Draw();

	
}
 
//------------------------------------------------------------------------------------------------------------
//GameManagerを初期化する
void GameManager::InitGameManager() {
	SRand(time(0));
	//マップチップをロード
	re_manager->LoadMapChipCsv();

	//描画サイズ変更
	ScaleChange();
	ModifyScalePos();

	inventory = new Inventory(0,this);
	inventories.emplace_back(inventory);
	bankinventory = new Inventory(0,this);
	bank_inventroy_list.emplace_back(bankinventory);
	
	
	//ダンジョン生成
	GenerateDungeon(DungeonType::TOWER);
	
}

//------------------------------------------------------------------------------------------------------------
//ダンジョン再生成(次の階層に行くときに)
void GameManager::DungeonReCreate()
{
	if (map_manager != nullptr) {
		map_manager.reset();
	}
	waypoint.clear();
	//ダンジョンを生成開始
	GenerateDungeon(nowdungeon);
}

//------------------------------------------------------------------------------------------------------------
//ショップのアイテムをプレイヤーのインベントリに追加する関数
void GameManager::AddItemFromShop(std::shared_ptr<Item> shop_item)
{
	//今のインベントリの持つアイテム配列が最大なら
	if (inventories[inventory_value]->inventory_list.size() >= 10) {
		Inventory* new_inventory = new Inventory(inventory_value + 1, this);
		//inventory配列に追加する
		inventories.emplace_back(new_inventory);
		//登録インベントリを更新
		inventory_value++;
	}

	//アイテムの名前を取得
	std::string item_name = shop_item->GetItemName();

	//取得したアイテムの名前のアイテムを取得
	std::shared_ptr<Item> new_item = object_manager->GetItemManager()->GetItemData(item_name);
	
	//プレイヤーインベントリにアイテムを追加
	inventories[inventory_value]->AddInventory(new_item);

	tnl::DebugTrace("\n===================ショップからインベントリに追加=========================\n");
}

//------------------------------------------------------------------------------------------------------------
//インベントリにアイテムを追加する
void GameManager::AddItemToInventory(const std::string name, std::vector<Inventory*>& inventories, int& inventory_value)
{
	//今のインベントリの持つアイテム配列が最大なら
	if (inventories[inventory_value]->inventory_list.size() >= 10) {
		Inventory* new_inventory = new Inventory(inventory_value + 1,this);
		//inventory配列に追加する
		inventories.emplace_back(new_inventory);
		//登録インベントリを更新
		inventory_value++;
	}

	//マップ内で取得した名前と一致するアイテムを取得
	std::shared_ptr<Item> item = object_manager->GetItemManager()->GetItemData(name);

	inventories[inventory_value]->AddInventory(item);

	tnl::DebugTrace("\n===================マップからからインベントリに追加=========================\n");

}

//------------------------------------------------------------------------------------------------------------
//インベントリからアイテムを削除する関数(インベントリアイテムを前に詰める処理もしている)
void GameManager::EraseItemFromInventory(const int now_inventory_id)
{
	//現在の位置の配列番号を取得
	int select_value = inventories[now_inventory_id]->GetCursorValue();
	//表示中のインベントリを取得
	auto now_inventory = inventories[now_inventory_id]->inventory_list.begin();

	//選択されたアイテムまでイテレータ移動
	for (int i = 0; i < select_value; i++) {
		now_inventory++;
	}

	now_inventory = inventories[now_inventory_id]->inventory_list.erase(now_inventory);
	//選択中のアイテムは消去しているのでカーソルを一つ上に移動
	inventories[now_inventory_id]->SetCursorValue(-1);

	if (now_inventory_id != inventory_value) {
		int check_inv_value = now_inventory_id;
		while (1) {

			if (inventories[check_inv_value + 1]->inventory_list.empty())break;
			//次のページの最初のアイテムをコピーして消したアイテムを末尾に追加する
			auto item = inventories[check_inv_value + 1]->inventory_list.begin();
			//アイテムを追加
			inventories[check_inv_value]->inventory_list.emplace_back((*item));
			//次のページの最初のアイテムをpopする
			inventories[check_inv_value]->inventory_list.pop_front();
			//インベントリの最後のページにたどり着いたらbreak.
			if (check_inv_value + 1 == inventory_value)break;
			check_inv_value++;
		}
	}
	//最初のインベントリなら
	else {
		//インベントリ内のアイテムを１減らす
		//これ以上前のページがないため
		inventories[now_inventory_id]->SetItemValue(-1);
	}
	//何もアイテムが鳴インベントリを削除する
	if (inventories[now_inventory_id]->inventory_list.empty()) {
		if (inventory_value != 0) {
			delete inventories[now_inventory_id];
			inventories[inventory_value] = nullptr;
			inventories.pop_back();
			//インベントリ数を一つ減らす
			inventory_value--;
			//削除した
			is_delete_invantory = true;

		}
	}
	//インベントリの削除フラグが立っていたらreturn
	if (is_delete_invantory)return;
	//カーソルの位置を一番上にリセット
	if (inventories[now_inventory_id]->inventory_list.empty())inventories[now_inventory_id]->CursorReset();

}

//------------------------------------------------------------------------------------------------------------
//特定のアイテムを生存リストから削除する
bool GameManager::DetectDropItem(std::shared_ptr<Item> item, std::list<std::shared_ptr<Item>>& it_list)
{
	bool erase = false;
	for (auto item_itr = it_list.begin(); item_itr != it_list.end();) {
		//一致するものがあれば削除する
		if ((*item_itr) == item) {
			item_itr = it_list.erase(item_itr);
			erase = true;
			break;
		}
		else {
			item_itr++;
		}
	}
	return erase;
}

//------------------------------------------------------------------------------------------------------------
bool GameManager::GetDoneBuyInStores() const
{
	return map_manager->GetDoneBuy();
}

//------------------------------------------------------------------------------------------------------------
//ダンジョンごとの画像を取得する
std::vector<int>& GameManager::GetGraphicsHandles(DungeonType dungeontype) const 
{
	//引数によって画像を取得する
	return re_manager->dungeon_handls[std::underlying_type<DungeonType>::type(dungeontype)];
}
//------------------------------------------------------------------------------------------------------------
//マップチップの情報を取得する関数
int GameManager::GetMapChip(tnl::Vector3 mappos) const
{
	return map_manager->GetChip(mappos.x,mappos.y);
}

//------------------------------------------------------------------------------------------------------------
//ダンジョンの生成(プレイヤーの生成も行っている)
void GameManager::GenerateDungeon(DungeonType dungeontype) {
	
	//MapManagerがdeleteされていなければ一度deleteする(Mapを一から作り直す)
	if (map_manager != nullptr) {
		map_manager.reset();
	}

	waypoint.clear();
	
	//引数で渡したダンジョンタイプを現在のダンジョンに変更する
	nowdungeon = dungeontype;
	debug_manager->DebugString("\n======================ダンジョン生成======================\n");
	
	//Mapの生成を開始
	GenerateDungeonMap(dungeontype);
	debug_manager->DebugString("\n======================ダンジョン生成終了======================\n");
	
	
	//マップ自動生成
	map_manager->AreaDivideStart(MAPWIDTH, MAPHEIGHT, map_manager);
	debug_manager->DebugString("\n======================分割終了======================\n");

	
	//階段のマップ座標を取得
	tnl::Vector3 stairpos = SetStartPosition(SetStartPositionType::STAIR);
	debug_manager->DebugString("\n======================階段座標取得完了======================\n");
	
	
	//階段設置
	map_manager->ResetChip(stairpos.x, stairpos.y, object_manager->factory->GetMapChip()->STAIRS);
	
	//debug用
	map_manager->AddStairList(stairpos);
	

	//ショップをランダムで設置
	map_manager->SetShop();
	debug_manager->DebugString("\n======================ショップ設置======================\n");


	//マップチップクラスインスタンス化
	object_manager->GenerateOrdersToMapChip();
	debug_manager->DebugString("\n======================マップチップインスタンス化完了=======================\n");


	//プレイヤーがいなければ以下の処理はしない
	if (object_manager->factory->GetPlayer() == nullptr) {
		return;
	}
	//ポジションを決定
	object_manager->factory->GetPlayer()->SetCharaPos(SetStartPosition(SetStartPositionType::PLAYER));
	//ポジションを取得
	tnl::Vector3 player_pos = object_manager->factory->GetPlayer()->GetCharaPos();
	
	map_manager->ChangeRoomVisit(WorldToLocalPos(player_pos));
	debug_manager->DebugString("\n======================ポジション登録完了======================\n");


	CameraReset();
}

//------------------------------------------------------------------------------------------------------------
//ダンジョンマップの生成
void GameManager::GenerateDungeonMap(DungeonType dungeontype)
{
	//int型のvectorに格納
	std::vector<int>gf_handles = GetGraphicsHandles(dungeontype);
	//Mapのインスタンス化(引数には、Mapの幅と高さを指定,画像ハンドル)
	map_manager = std::make_shared<MapManager>(MAPWIDTH, MAPHEIGHT, gf_handles,this);
}

//------------------------------------------------------------------------------------------------------------
//特定の座標に敵がいるかどうかをチェックする
bool GameManager::CheckIsThereEnemy(tnl::Vector3 pos)
{
	bool ene_is_there = false;
	for (auto enemy : object_manager->GetAliveEnemyList()) {
		tnl::Vector3 enemy_pos = WorldToLocalPos(enemy->GetCharaPos());
		//引数に渡された敵のポジションと敵の生存リストに格納されている敵のポジションが同じ場合フラグを立てる
		if (enemy_pos.x == pos.x && enemy_pos.y == pos.y) {
			ene_is_there = true;
			break;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
std::shared_ptr<Enemy> GameManager::GetIsThereEnemyToDir(tnl::Vector3 pos)
{
	std::shared_ptr<Enemy>there_enemy = nullptr;

	for (auto enemy : object_manager->GetAliveEnemyList()) {
		//敵のポジションを取得
		tnl::Vector3 get_enemy_pos = enemy->GetCharaPos();
		tnl::Vector3 enemy_pos = WorldToLocalPos(get_enemy_pos);
		//引数の座標と一致する場合
		if (enemy_pos.x == pos.x && enemy_pos.y == pos.y) {
			there_enemy = enemy;
			break;
		}
	}

	return there_enemy;
}

//------------------------------------------------------------------------------------------------------------
//特定の敵とプレイヤーが隣接しているかチェック
bool GameManager::CheckNearByPlayer(std::shared_ptr<Enemy> enemy)
{
	//enemyPosはマップ座標
	tnl::Vector3 enemy_pos = WorldToLocalPos(enemy->GetCharaPos());

	tnl::Vector3 down;
	tnl::Vector3 left;
	tnl::Vector3 right;
	tnl::Vector3 up;

	if (enemy_pos.y > 0)		up = enemy_pos + GetFirstDirVector((int)Character::MoveDir::UP);
	if (enemy_pos.x < MAPWIDTH)	right = enemy_pos + GetFirstDirVector((int)Character::MoveDir::RIGHT);
	if (enemy_pos.y < MAPHEIGHT)down = enemy_pos + GetFirstDirVector((int)Character::MoveDir::DOWN);
	if (enemy_pos.x > 0)		left = enemy_pos + GetFirstDirVector((int)Character::MoveDir::LEFT);

	tnl::Vector3 enemyPosNear[4] = { left,up,right,down };

	tnl::Vector3 playerPos = WorldToLocalPos(object_manager->factory->GetPlayer()->GetCharaPos());

	bool isNear = false;
	for (int i = 0; i < 4; ++i) {
		if (playerPos.x == enemyPosNear[i].x && playerPos.y == enemyPosNear[i].y) {
			isNear = true;
			break;
		}
	}
	return isNear;
	return false;

}
//------------------------------------------------------------------------------------------------------------
void GameManager::CheckIsDeadCharactor(std::shared_ptr<Character> object, tnl::Vector3 pos)
{
	tnl::Vector3 dead_pos;
	//dir方向のポジションをとる
	dead_pos = WorldToLocalPos(object->GetCharaPos()) + GetFirstDirVector((int)object->GetCharaDir());


}

//------------------------------------------------------------------------------------------------------------
void GameManager::SetDoneBuyInStores() const
{
	map_manager->SetDoneBuy();
}

//------------------------------------------------------------------------------------------------------------
//オブジェクト同士の当たり判定の処理を主にする関数
void GameManager::SetObjectSHitCheck() {


	//------------------------------------------------------------------------------------------
	//プレイヤーと敵のペアを作成
	collision->RegistIntersectedProcess<Player, Enemy>(
		[](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {

			//参照カウンターを保ったままポインタの変換を行ってくれる
			std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(a);
			std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(b);

			if (&Collision::IsIntersectAABB) {
				DebugManager::DebugString("\n=========プレイヤーと敵が衝突==========\n");
			}
		});

	for (auto& enemy : object_manager->GetAliveEnemyList()) {
		collision->RegistPairObject<Player, Enemy>(object_manager->factory->GetPlayer(), enemy);
	}


	//------------------------------------------------------------------------------------------
	//敵同士の当たり判定のペアを作成
	collision->RegistIntersectedProcess<Enemy, Enemy>(
		[](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {

			//参照カウンターを保ったままポインタの変換を行ってくれる
			std::shared_ptr<Enemy> enemy_first = std::dynamic_pointer_cast<Enemy>(a);
			std::shared_ptr<Enemy> enemy_second = std::dynamic_pointer_cast<Enemy>(b);

			if (&Collision::IsIntersectAABB) {
				DebugManager::DebugString("\n===========敵と敵が衝突=============\n");
			}
		});

	//外側のループがコンテナの最初の要素から始まり、内側のループが外側のループの現在の要素の次の要素から始める
	//これにより、各ペアが一度だけ処理されることが保証される
	//std::next(it1) を使用することで、it1 と it2 が同じオブジェクトを指すことはない
	for (auto it1 = object_manager->GetAliveEnemyList().begin(); it1 != object_manager->GetAliveEnemyList().end(); ++it1) {
		for (auto it2 = std::next(it1); it2 != object_manager->GetAliveEnemyList().end(); ++it2) {
			collision->RegistPairObject<Enemy, Enemy>(*it2, *it1);
		}
	}


	//------------------------------------------------------------------------------------------
	//プレイヤーと壁の当たり判定のペアを作成
	collision->RegistIntersectedProcess<Player, MapChip>(
		[](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {

			//参照カウンターを保ったままポインタの変換を行ってくれる
			std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(a);
			std::shared_ptr<MapChip> mapchip = std::dynamic_pointer_cast<MapChip>(b);

			if (&Collision::IsIntersectAABB) {
				DebugManager::DebugString("\n============プレイヤーと壁が衝突===============\n");
			}
		});

	for (auto& mapchip : object_manager->GetWallMapChipList()) {
		collision->RegistPairObject<Player, MapChip>(object_manager->factory->GetPlayer(), mapchip);
	}


	//------------------------------------------------------------------------------------------
	//敵と壁の当たり判定のペアを作成
	collision->RegistIntersectedProcess<Enemy, MapChip>(
		[](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {

			//参照カウンターを保ったままポインタの変換を行ってくれる
			std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(b);
			std::shared_ptr<MapChip> mapchip = std::dynamic_pointer_cast<MapChip>(a);

			if (&Collision::IsIntersectAABB) {
				DebugManager::DebugString("\n=============敵と壁が衝突=============\n");
			}
		});

	for (auto it_enemy = object_manager->GetAliveEnemyList().begin(); it_enemy != object_manager->GetAliveEnemyList().end(); ++it_enemy) {
		for (auto it_wall = object_manager->GetWallMapChipList().begin(); it_wall != object_manager->GetWallMapChipList().end(); ++it_wall) {
			collision->RegistPairObject<Enemy, MapChip>(*it_enemy, *it_wall);
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//ペアにしたObjectをどうしが合ったっているかを判定
void GameManager::UpdateHitCheck()
{

	//プレイヤーの当たり判定
	for (auto& pl_en : collision->GetIntersectList()) {

		//IsIntersectAABBはbool型
		//プレイヤーと敵の当たり判定
		if (collision->IsIntersectAABB(pl_en.object_a->GetCharaPos(), tnl::Vector3(pl_en.object_a->GetCharaChipSize()),
			pl_en.object_b->GetCharaPos(), tnl::Vector3(pl_en.object_b->GetCharaChipSize())))

		  
		{ // li->a_ と li->b_ の衝突判定
			pl_en.intersected_call(pl_en.object_a, pl_en.object_b);

			tnl::CorrectPositionRect(pl_en.object_a->GetCharaPos(), 
									 pl_en.object_b->GetCharaPos(), 
									 tnl::Vector2i(pl_en.object_a->GetCharaChipSize(), pl_en.object_a->GetCharaChipSize()),
									 tnl::Vector2i(pl_en.object_b->GetCharaChipSize(), pl_en.object_b->GetCharaChipSize()), 
									 pl_en.object_a->SetCharaPosToCollistion(),
									 pl_en.object_b->SetCharaPosToCollistion(),
									 tnl::eCorrTypeRect::BOTH, 
									 tnl::eCorrTypeRect::BOTH);

		}

	}

	//敵同士の当たり判定チェック
	for (auto& enemy : collision->GetIntersectList()) {
		
		//IsIntersectAABBはbool型
		if (collision->IsIntersectAABB(enemy.object_a->GetCharaPos(), tnl::Vector3(enemy.object_a->GetCharaChipSize()),
			enemy.object_b->GetCharaPos(), tnl::Vector3(enemy.object_b->GetCharaChipSize())))


		{ // enemy と enemy の衝突判定
			enemy.intersected_call(enemy.object_a, enemy.object_b);

			tnl::CorrectPositionRect(enemy.object_a->GetCharaPos(),
									 enemy.object_b->GetCharaPos(),
									 tnl::Vector2i(enemy.object_a->GetCharaChipSize(), enemy.object_a->GetCharaChipSize()),
									 tnl::Vector2i(enemy.object_b->GetCharaChipSize(), enemy.object_b->GetCharaChipSize()),
									 enemy.object_a->SetCharaPosToCollistion(),
									 enemy.object_b->SetCharaPosToCollistion(),
									 tnl::eCorrTypeRect::BOTH,
									 tnl::eCorrTypeRect::BOTH);

		}
	}


	//プレイヤーの当たり判定
	for (auto& pl_en : collision->GetIntersectList()) {

		//IsIntersectAABBはbool型
		//プレイヤーと壁の当たり判定チェック
		if (collision->IsIntersectAABB(pl_en.object_a->GetCharaPos(), tnl::Vector3(pl_en.object_a->GetCharaChipSize()),
			pl_en.object_b->GetOldChipPos(), tnl::Vector3(pl_en.object_b->GetMapChipSize())))


		{ // li->a_ と li->b_ の衝突判定
			pl_en.intersected_call(pl_en.object_a, pl_en.object_b);

			tnl::CorrectPositionRect(pl_en.object_a->GetCharaPos(),
									 pl_en.object_b->GetOldChipPos(),
									 tnl::Vector2i(pl_en.object_a->GetCharaChipSize(), pl_en.object_a->GetCharaChipSize()),
									 tnl::Vector2i(pl_en.object_b->GetMapChipSize(), pl_en.object_b->GetMapChipSize()),
									 pl_en.object_a->SetCharaPosToCollistion(),
									 pl_en.object_b->SetOldMapChipPos(),
									 tnl::eCorrTypeRect::PWRFL_B,
									 tnl::eCorrTypeRect::PWRFL_B);

		}

	}


	//敵の当たり判定
	for (auto& pl_en : collision->GetIntersectList()) {

		//IsIntersectAABBはbool型
		//敵と壁の当たり判定チェック
		if (collision->IsIntersectAABB(pl_en.object_a->GetCharaPos(), tnl::Vector3(pl_en.object_a->GetCharaChipSize()),
			pl_en.object_b->GetOldChipPos(), tnl::Vector3(pl_en.object_b->GetMapChipSize())))


		{ // li->a_ と li->b_ の衝突判定
			pl_en.intersected_call(pl_en.object_a, pl_en.object_b);

			tnl::CorrectPositionRect(pl_en.object_a->GetCharaPos(),
									 pl_en.object_b->GetOldChipPos(),
									 tnl::Vector2i(pl_en.object_a->GetCharaChipSize(), pl_en.object_a->GetCharaChipSize()),
									 tnl::Vector2i(pl_en.object_b->GetMapChipSize(), pl_en.object_b->GetMapChipSize()),
									 pl_en.object_a->SetCharaPosToCollistion(),
									 pl_en.object_b->SetOldMapChipPos(),
									 tnl::eCorrTypeRect::PWRFL_B,
									 tnl::eCorrTypeRect::PWRFL_B);

		}

	}
}

//------------------------------------------------------------------------------------------------------------
//HPが0のキャラクターの当たり判定を削除する
void GameManager::EraseHitList()
{
	//先頭イテレーターを取得
	auto character = collision->GetIntersectList().begin();
	
	while (character != collision->GetIntersectList().end())
	{
		//どちらかのオブジェクトの生存フラグが下りていた場合
		if (!character->object_a->GetIsAlive() || !character->object_b->GetIsAlive()) {
			//そのペアを削除
			character = collision->GetIntersectList().erase(character);
		}
		else {
			character++;
		}
		
	}
}

//------------------------------------------------------------------------------------------------------------
//描画座標からマップ座標に変換
tnl::Vector3 GameManager::WorldToLocalPos(tnl::Vector3 pos) {

	return WorldToMap(pos.x, pos.y);
}

//------------------------------------------------------------------------------------------------------------
//マップ座標から描画座標に変換
tnl::Vector3 GameManager::LocalToWorldPos(int map_x, int map_y) {

	return MapToWorld(map_x, map_y);
}

//------------------------------------------------------------------------------------------------------------
tnl::Vector3 GameManager::WorldToMap(const int world_x, const int world_y)
{

	int map_x = world_x / now_gh_size;
	int map_y = world_y / now_gh_size;

	return tnl::Vector3(map_x, map_y, 0);
}

//------------------------------------------------------------------------------------------------------------
tnl::Vector3 GameManager::MapToWorld(const int map_x, const int map_y)
{
	
	int world_x = map_x * now_gh_size;
	int world_y = map_y * now_gh_size;

	return tnl::Vector3(world_x, world_y, 0);
}

//------------------------------------------------------------------------------------------------------------
//フライウェイトパターンを採用したLoadGraph(std::string_viewは実体を持たない文字列,ポインタと長さだけを持つ)
int GameManager::LoadGraphEx(std::string_view graph) {

	auto it = map_graph.find(graph);
	if (it != map_graph.end()) {
		return map_graph[graph];
	}
	else {
		int existing_gh = LoadGraph(graph.data());
		map_graph.insert(std::make_pair(graph, existing_gh));
	}

	return map_graph[graph];
}

//------------------------------------------------------------------------------------------------------------
int GameManager::CheckIsThere(tnl::Vector3 pos)
{
	return map_manager->CheckIsThere(pos.x,pos.y);
}

//------------------------------------------------------------------------------------------------------------  
//画面の拡大縮小を変更する関数
void GameManager::ScaleChange()
{
	tnl::Vector3 pos = object_manager->factory->GetPlayer()->GetCharaPos();
	//プレイヤーのローカル座標を取得
	player_pos_buff = WorldToLocalPos(pos);
	//今の描画方法が通常倍率なら
	if (now_scale == ScaleMode::NOMAL) {
		//描画方法を拡大にする
		now_scale = ScaleMode::ENLAEGE;
	}
	//今の描画が拡大倍率なら
	else {
		now_scale = ScaleMode::NOMAL;
	}
	//描画チップのサイズを変更する
	now_gh_size = scale[std::underlying_type<ScaleMode>::type(now_scale)];
	//描画倍率を変更
	ratio = now_gh_size / GRAPHICSIZE;

}

//------------------------------------------------------------------------------------------------------------  
//スケールチェンジ時のポジション修正
void GameManager::ModifyScalePos()
{
	object_manager->factory->GetPlayer()->SetCharaPos(LocalToWorldPos(player_pos_buff.x, player_pos_buff.y));

	CameraReset();
}

//------------------------------------------------------------------------------------------------------------  
//MiniMapが描画できるか判定
bool GameManager::CanDrawMiniMap(tnl::Vector3 localpos) {
	return map_manager->CheckCanDraw(localpos);
}

//------------------------------------------------------------------------------------------------------------
//MiniMapの更新
void GameManager::RefreshMiniMap(tnl::Vector3 localpos) {

	map_manager->ChangeRoomVisit(localpos);
	map_manager->ChangeWayVisit(localpos);
}

//------------------------------------------------------------------------------------------------------------
//MiniMapをblendしてから描画
void GameManager::DrawBlendedMiniMap() {

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	map_manager->MiniMapDraw(object_manager->factory->camera);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

//------------------------------------------------------------------------------------------------------------
//引数で渡された部屋の数に1を足した値を、waypointベクターのサイズに設定
void GameManager::InitWayPointVector(const int initroomnum) {
	//引数で渡された部屋の数に1を足した値を、waypointベクターのサイズに設定
	//これは、部屋の番号が0から始まるため
	//waypointベクターの各要素は、その部屋に到達するための最短経路を表すウェイポイントのリスト
	waypoint.resize(initroomnum + 1);
}

//------------------------------------------------------------------------------------------------------------
//通路生成時に、部屋番号と、ポジションをvectorに追加する関数
void GameManager::SetRoomWayPoint(const tnl::Vector3 pos, const int roomid) {
	
	waypoint[roomid].emplace_back(pos);
}

//------------------------------------------------------------------------------------------------------------
//引数に渡された部屋番号を座標から一番遠い出口を取得する関数
tnl::Vector3 GameManager::GetFarPoint(int room_id, tnl::Vector3 pos)
{
	//room_idから出口の座標を取得する
	std::vector<tnl::Vector3>out_point;
	for (int i = 0; i < waypoint[room_id].size(); i++) {
		out_point.emplace_back(waypoint[room_id][i]);
	}

	std::vector<int>distances_list;

	//out_pointの座標とposの差分を求めて、distance_listに追加する
	for (auto point : out_point) {
		int dis_x = point.x - pos.x;
		int dis_y = point.y - pos.y;
		int distance = dis_x * dis_x + dis_y * dis_y;
		distances_list.emplace_back(distance);
	}

	//distancesの添え字番号とout_pointの添字番号は一致
	//最大距離時のout_pointの番号を取得する
	 
	//最大距離の配列番号を保持する変数
	int max = 0;
	//比較値がない場合の一時保管用変数
	int buff = 0;

	for (int i = 0; i < distances_list.size(); i++) {
		//buffに値が入ってなければ自分自身を入れる
		if(buff == 0)buff = distances_list[i];
		if (buff < distances_list[i]) {
			//最大距離配列番号
			max = i;
		}
	}
	//一番遠いout_pointを返す
	return out_point[max];
}

//------------------------------------------------------------------------------------------------------------
//プレイヤーの部屋番号を変更する
void GameManager::SetPlayerRoomValue(int nowroomvalue)
{	
	//キャラの部屋番号の変更後の値をセットする
	object_manager->factory->GetPlayer()->SetNowRoomValue(nowroomvalue);
}

//------------------------------------------------------------------------------------------------------------
//ランダムな座標を設定する関数
tnl::Vector3 GameManager::SetStartPosition(SetStartPositionType type) {
	
	//ランダムな部屋番号を取得
	int random_room = rand() % (map_manager->GetRoomNum());
	//部屋番号から部屋を取得
	std::vector<int> room = map_manager->GetRoom(random_room);
	//部屋の中のランダムなマップ座標を取得
	int type_x = GetRandValue(room[0], room[2]);
	int type_y = GetRandValue(room[1], room[3]);
	//敵の生成の場合(被りを防ぐチェック)
	//プレイヤーのポジションと被るのを防ぐ
	if (type == SetStartPositionType::ENEMY) {
		int player_x = object_manager->factory->GetPlayer()->GetCharaPos().x;
		int player_y = object_manager->factory->GetPlayer()->GetCharaPos().y;
		int debagcount = 0;
		while (1)
		{
			//プレイヤーと敵の座標が重ならなくなるまでループ
			if (player_x != type_x && player_y != type_y)break; {
				type_x = GetRandValue(room[0], room[2]);
				type_y = GetRandValue(room[1], room[3]);
				debagcount++;
				tnl::DebugTrace("\n================座標生成%d回目===================\n", debagcount);

			}
		}
					
	}
	//階段の場合
	if (type == SetStartPositionType::STAIR)return tnl::Vector3( type_x,type_y,0 );

	//ランダムに設定した座標を描画座標に変換
	tnl::Vector3 newpos = MapToWorld(type_x, type_y);

	return newpos;
}

//------------------------------------------------------------------------------------------------------------
//メルセンヌ・ツイスター(引数に最小最大値を与え、その間からランダムに値を返す)
int GameManager::GetRandValue(int a, int b) {

	//std::mt19937は、メルセンヌ・ツイスターというアルゴリズムを使って、高品質な擬似乱数を生成する乱数生成器
	//乱数生成器(mt19937という乱数生成器オブジェクトを作り、
	//random_deviceというのが非決定的な乱数生成期から得た値を種としてmtに渡す)
	std::mt19937 mt(std::random_device{}());


	if (b > a or a == b) {

		// 分布オブジェクト(乱数生成期で出す値の最小値、最大値を決める
		std::uniform_int_distribution<int> dist(a, b);
		// ランダムな値
		int random = dist(mt);

		return random;
	}
	else {

		// 分布オブジェクト(乱数生成期で出す値の最小値、最大値を決める
		std::uniform_int_distribution<int> dist(b, a);
		// ランダムな値
		int random = dist(mt);

		return random;
	}

}

//------------------------------------------------------------------------------------------------------------
//スキル以外のダメージを与える関数
float GameManager::CalculationDamage(int atk, int def)
{
	float calc_value = (float)GetRandValue(-5, 8);
	float damage = atk / std::pow(2.5f, (def / 10));

	//0以下なら
	if (damage < 0)damage = 0;

	return damage;
}

//------------------------------------------------------------------------------------------------------------
void GameManager::InflictDamageToTarget(Character* object, tnl::Vector3 pos)
{
	//プレイヤーの場合
	if (object->GetObjectType() == Object::ObjectType::PLAYER) {
		//すべての敵から前にいる敵を取得
		std::shared_ptr<Enemy>front_enemy = GetIsThereEnemyToDir(pos);
		//空振り
		if (front_enemy == nullptr) {
			return;
		}
		//攻撃力と防御力を取得して計算する
		float damage = CalculationDamage(object_manager->factory->GetPlayer()->GetCharaStetus(Character::Stetus::HP), front_enemy->GetCharaStetus(Character::Stetus::DEF));
		//ダメージ反映
		AffectDamage(damage, front_enemy);

	}
	else {
		float damage = CalculationDamage(object->GetCharaStetus(Character::Stetus::ATK), object_manager->factory->GetPlayer()->GetCharaStetus(Character::Stetus::DEF));

		//ダメージ反映
		AffectDamage(damage, object_manager->factory->GetPlayer());
	}


}

//------------------------------------------------------------------------------------------------------------
//ターゲットにダメージを与える関数(スキルの場合)
void GameManager::CalculationSkillDamageToTarget(Character* object, const tnl::Vector3 pos, Skill* now_use_skill)
{
	//スキルの数値データを取得
	const float* skill_data = now_use_skill->GetSkillFloat();
	//スキル名を取得
	std::string skill_name = now_use_skill->GetSkillName();

	//プレイヤーが攻撃するなら
	if (object->GetObjectType() == Object::ObjectType::PLAYER) {
		//すべての敵から前にいる敵を取得
		std::shared_ptr<Enemy>front_enemy = GetIsThereEnemyToDir(pos);
		//もし空振りなら
		if (front_enemy == nullptr) {
			return;
		}
		float damage = CalculationSkillDamage(skill_data[0], object_manager->factory->GetPlayer()->GetCharaStetus(Character::Stetus::ATK), front_enemy->GetCharaStetus(Character::Stetus::DEF));

		//実際にhpに反映させる
		AffectDamage(damage, front_enemy);
	}
	else {
		float damage = CalculationSkillDamage(skill_data[0], object->GetCharaStetus(Character::Stetus::ATK), object_manager->factory->GetPlayer()->GetCharaStetus(Character::Stetus::DEF));
		//実際にhpに反映させる
		AffectDamage(damage, object_manager->factory->GetPlayer());
	}
}
//------------------------------------------------------------------------------------------------------------
float GameManager::CalculationSkillDamage(const float skill_rate, const int atk, const int def)
{
	float skill_damage = atk * skill_rate;

	return CalculationDamage(skill_damage, def);
}

//------------------------------------------------------------------------------------------------------------
//実際にステータスに影響させる関数
void GameManager::AffectDamage(float damage, std::shared_ptr<Character> object)
{
	object->SetAffectToCharaStetus(Character::Stetus::HP, -damage);
}

//------------------------------------------------------------------------------------------------------------
//カメラの位置をプレイヤーの位置に合わせる
void GameManager::CameraReset()
{
	if(tnl::Input::IsKeyDownTrigger(eKeys::KB_6))
	{ object_manager->factory->camera.Zoom(); }

	//カメラが動かんここ直す
	tnl::Vector3 pos = object_manager->factory->GetPlayer()->GetCharaPos();

	//カメラをプレイヤーに合わせる
	object_manager->factory->camera.cameraPos = pos - tnl::Vector3(DXE_WINDOW_WIDTH >> 1, DXE_WINDOW_HEIGHT >> 1, 0);

	
#if 0
	//デバッグ
	tnl::DebugTrace("\nplayer:(%f,%f),pos:(%f,%f),camera:(%f,%f)(CameraReset)\n",
		//プレイヤーのポジション
		object_manager->factory->GetPlayer()->GetCharaPos().x, object_manager->factory->GetPlayer()->GetCharaPos().y,
		//取得したプレイヤーのポジション(ここは上記のプレイヤーの座標と同じになっているはず)
		pos.x, pos.y,
		//カメラ座標
		object_manager->factory->camera.cameraPos.x, object_manager->factory->camera.cameraPos.y);

#endif // 0


}
