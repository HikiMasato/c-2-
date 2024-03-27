///*****Description**********************
///オブジェクトを生成するクラス
///ゲーム全体に対して関連する動的生成などを主に行うクラス
/// (抽象クラスにしてサブクラスに生成を任せるべきなのか…)
///**************************************
#pragma once
//-------------------------------------------------------
//Object
#include "MyCamera.h" 

class Object;
class Character;
class MapChip;
class hm::Camera;
class Player;
class Enemy;
class Item;

class GameManager;
class EnemyManager;



class Factory {
public:

	Factory(){}
	~Factory();	

	Factory(GameManager* game_manager_) : game_manager(game_manager_) {};

	//プレイヤーのスポーン場所
	enum class PlayerSpawn {
		STARTMAP,
		DUNGEON
	};

	//=====================変数============================
	hm::Camera camera;
	//=====================関数============================
	
	//受注を受けて生成する関数
	//生成する数、タイミングはすべてObjectManagerが管理している
	//あくまでも生成のみを行う
	//arg1…初期ポジション(ランダムに決まる)
	//arg2…ヒットポイント(HP)
	//arg3…MP(WIP)
	//arg4…ATK
	//arg5…DEF
	//arg6…Name
	//arg7…画像ハンドル
	//arg8…すべてのオブジェクトを格納しているリスト
	void GeneratePlayer(PlayerSpawn now_spawn, tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, std::list<std::shared_ptr<Character>>& chara_list);
	//arg9…生存している敵のリスト
	void GenerateEnemy(tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, std::list<std::shared_ptr<Character>>& chara_list, std::list<std::shared_ptr<Enemy>>& ali_ene);
	//arg2…アイテムがプレイヤーに与える効果の数値(HP回復量や、ATK上昇量,DEF上昇量 etc...)
	void GenerateItem(tnl::Vector3 item_pos, int item_stetus, std::string item_name, std::string item_handle, std::string item_desctiption, int item_price, std::list<std::shared_ptr<Object>>& obj_list, std::list<std::shared_ptr<Item>>& ali_item);

	//すべてのアイテムを一度生成して格納する関数
	//ランダムに生成されたもの以外の
	void AllItemCreate(tnl::Vector3 item_pos, int item_stetus, std::string item_name, std::string item_handle, std::string item_desctiption, int item_price, std::vector<std::shared_ptr<Item>>& all_item);

	//マップチップをインスタンス化する
	//arg1…マップチップの元座標
	//arg2…画像ハンドル
	//arg3…インスタンス化したマップチップクラスを格納する配列
	//arg4…wall_chipを格納する配列
	//arg5…インスタンス化するマップチップが壁かどうかのフラグ(true…壁、false…壁以外)
	void GenerateMapChip(tnl::Vector3 old_chip_pos, int chip_handle, std::list<std::shared_ptr<MapChip>>& all_chip_list, std::list<std::shared_ptr<MapChip>>& wall_list, bool is_wall = false);


	//======================ゲッター===========================

	
	std::shared_ptr<Player> GetPlayer() const {
		return player;
	}

	std::shared_ptr<Enemy> GetEnemy() const {
		return enemy;
	}

	std::shared_ptr<MapChip> GetMapChip() const {
		return mapchip;
	}

	std::shared_ptr<Item> GetItem() const {
		return item;
	}

	PlayerSpawn GetPlayerSpawn() const {
		return now_spawn;
	}
	
	//======================セッター===========================
	void SetPlayerSpawn(PlayerSpawn set_spawn) {
		now_spawn = set_spawn;
	}

	void SetPlayer(std::shared_ptr<Player> player_) {
		player = player_;
	}


private:

	//=====================変数============================
	GameManager* game_manager;

	std::shared_ptr<Player> player = nullptr;
	std::shared_ptr<Enemy> enemy = nullptr;
	std::shared_ptr<MapChip>mapchip = nullptr;
	std::shared_ptr<Item> item = nullptr;

	//プレイヤーのスポーンタイプ
	PlayerSpawn now_spawn = PlayerSpawn::STARTMAP;

#if 0

	////呼び出し側で引数に設定したクラスがtemplate <A>に置き換わる
	////クラスのインスタンス化を行う関数
	//template<class A>
	//std::shared_ptr<A> GenerateObject(A) {
	//	object->object_list.emplace_back(object = std::make_shared<A>());
	//}

	//template<Object::ObjectType A>
	//std::list<std::shared_ptr<Object*>> StoreObject(Object::ObjectType) {
	//	auto it = object->object_list.begin();
	//	switch (Object::ObjectType)
	//	{
	//	case:Object::ObjectType::ENEMY
	//		for (it->get() == Object::ObjectType::ENEMY) {
	//			return
	//		}
	//		break;
	//	
	//	case:Object::ObjectType::ITEM
	//		for (it->get() == Object::ObjectType::ITEM) {
	//			return
	//		}
	//		break;
	//		
	//	default:
	//		break;
	//	}
	//	
	//}

		////objectを消去する関数
	//void  DeleteObject() {

	//	auto it = obj->objlist.begin();
	//	while (it != obj->objlist.end()) {

	//		if (Obj::ObjectType::TARGET == obj->GetActType()) {
	//			obj.reset();
	//		}
	//		if (Obj::ObjectType::PLAYER == obj->GetActType()) {
	//			obj.reset();
	//		}
	//		if (Obj::ObjectType::ITEM == obj->GetActType()) {
	//			obj.reset();
	//		}

	//		it++;
	//	}

	//}
#endif // 0
};
