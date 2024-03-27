///******************Description***********************
///オブジェクトを管理するクラス
///Factoryクラスに生成を受注する
///インスタンス化されたオブジェクトが格納されているリストを保持している 
///(その時にステータスクラスから生成するオブジェクトのデータを受け取りそのデータとともに受注する)
///GameManagerクラスにはこのクラスをインスタンス化すればすべてのオブジェクトを描画、などを一括で行える
///**************************************************** 
#pragma once
#include "MyCamera.h"
#include "Factory.h"
#include "Skill.h"
#include "SkillManager.h"

class GameManager;
class ItemManager;

class Factory;
class Object;
class Character;
class Stetus;

class hm::Camera;
class Enemy;
class Item;

class SkillManager;
class Skill;

class ObjectManager {
public:
	ObjectManager(std::vector<std::vector<std::string>>& re_stetus, std::vector<std::vector<std::string>>& effect_csv, std::vector<std::vector<std::string>>re_item, GameManager* game_manager_ = nullptr);
	~ObjectManager();

	


	//*****************変数宣言******************
	std::shared_ptr<Factory>factory = nullptr;


	//キャラクター以外のオブジェクトが格納されている(Itemなどetc...)
	std::list<std::shared_ptr<Object>>object_list;
	//キャラクターオブジェクトが格納されている
	std::list<std::shared_ptr<Character>>charactor_list;
	
	

	//敵のステータスと名前、画像ハンドルを一つのvectorにまとめる
	std::vector<std::pair<std::vector<int>, std::vector<std::string>>> chara_info;

	//*******************関数********************
	void Update(const float delta_time);
	void Draw();

	//Factoryクラスに生成を受注する関数(生成はFactoryが行うためここではステータスを渡すのと受注のみ行う)
	//arg1…現在のスポーンタイプ(STARTMAPの場合 : プレイヤーのみ生成、DUNGEONの場合 : プレイヤー、アイテム、敵生成)
	//arg2…階層移動の場合に呼び出す場合 : false(プレイヤーの生成以外を行う)、true(すべてのオブジェクトの生成を行う)
	void GenerateOrders(Factory::PlayerSpawn now_spawn_, bool is_dungeon_level_up = false);

	//DungeonMapを生成
	void GenerateOrdersToMapChip();

	//スタートマップ用のマップを生成
	void GenerateOrdersToStartMapChip();

	//すべてのアイテムを事前にすべて生成しておく関数
	void GenerateAllItem();

	//ショップに並べるアイテムを決める関数
	//string型で返す
	std::string GenerateShopItem();

	//敵を倒したときにドロップアイテム
	void GenerateDropItem(tnl::Vector3 pos);

	//条件に合致するリストの要素をすべて削除にする関数
	void EraseObjectList();
		
	//条件に合致するキャラリストの要素をすべて削除する関数(主に敵に攻撃した後に呼び出す)
	void EraseCharaList();

	//このクラスが所有するリストの用を祖すべてクリアする関数
	//arg1…プレイヤーを削除するかどうかのフラグ(true : 削除する、false…削除しない)
	void AllListClear(bool player_deletion_flag = true);

	//マップチップ格納リストを削除する
	void AllChipListClear();

	//キャラクターのステータスを強化する
	void RainForceCharaStetus();

	//*****************ゲッター*******************
	 
	//すべてのアイテムが格納されているリストを返す
	const std::vector<std::shared_ptr<Item>>& GetAllItemList() const {
		return all_item;
	}

	//マップ内に生存しているアイテムリストを変えす
	std::list<std::shared_ptr<Item>>& GetAliveItem() {
		return alive_item;
	}
	//生存中の敵リストを返す
	std::list<std::shared_ptr<Enemy>>& GetAliveEnemyList() {
		return alive_enemy;
	}

	std::shared_ptr<ItemManager> GetItemManager() {
		return item_manager;
	}
	//スターとマップチップが格納されたリストを返す
	std::list<std::shared_ptr<MapChip>>& GetStartMapChipList() {
		return start_map_chip_list;
	}
	//スタートマップのレイヤー2が格納されているリストを返す
	std::list<std::shared_ptr<MapChip>>& GetStartMapSecondChipList() {
		return start_map_second_chip_list;
	}

	//壁が格納されているリストを返す
	std::list<std::shared_ptr<MapChip>>& GetWallMapChipList() {
		return wall_chip;
	}

	//すべてのマップチップが格納されているリストを返す
	std::list<std::shared_ptr<MapChip>>& GetAllMapChipList() {
		return all_chip_list;
	}

	std::shared_ptr<SkillManager>& GetSkillManager() {
		return skill_manager;
	}
	//*****************セッター*******************

	//プレイヤーのスポーン場所を変更する
	void SetPlayerSpawnType(Factory::PlayerSpawn next_spawn);


	//すべてのアイテムを格納するリストにアイテムを追加する関数
	void AddAllItemList(Item* item) {
		all_item.emplace_back(item);
	}



private:

	//*****************変数宣言******************
	GameManager* game_manager;
	std::shared_ptr<Stetus>stetus = nullptr;
	std::shared_ptr<ItemManager>item_manager = nullptr;
	std::shared_ptr<SkillManager>skill_manager = nullptr;

	//ResourceManagerからコンストラクタでItemのデータを受け取る
	std::vector<std::vector<std::string>> item_data;
	
	//すべてのアイテムを格納しておく配列
	std::vector<std::shared_ptr<Item>>all_item;
	//アイテムの生存リスト
	std::list<std::shared_ptr<Item>>alive_item;

	//敵の生存リスト
	std::list<std::shared_ptr<Enemy>>alive_enemy;

	//壁格納リスト(当たり判定に使用)
	std::list<std::shared_ptr<MapChip>>wall_chip;
	//すべてのマップチップが格納されている
	std::list <std::shared_ptr<MapChip>>all_chip_list;

	//生成する特定のオブジェクトの数
	const int GENERATE_ENEMY = 5;
	const int GENERATE_ITEM = 5;

	//プレイヤーのスタートマップポジション
	tnl::Vector3 start_pl_pos = { 700, 600, 0 };
	//マップチップの元デバイス系座標
	tnl::Vector3 old_chip_pos;


	//=====スターとマップ用変数======
	//マップチップの縦数、横数
	int map_graph_value_x = 8;
	int map_graph_value_y = 11;
	//DivGraphで分割したマップチップを格納
	int start_map_handle[88];

	const int ALL_START_MAP_CHIP = 88;
	//マップチップ１枚当たりのサイズ
	const int MAPCHIPSIZE = 32;
	
	//スターとマップのデバイス座標(元座標)
	tnl::Vector3 former_map_pos = { -625.0f, -350.0f, 0 };
	
	//スタートマップのマップ情報を格納する
	std::vector<std::vector<int>>start_map_csv;
	std::vector<std::vector<int>>start_map_second_csv;

	//csvに書き込まれているチップ番号に沿って配置したデータを格納する
	std::list<std::shared_ptr<MapChip>> start_map_chip_list;
	std::list<std::shared_ptr<MapChip>> start_map_second_chip_list;

	//*******************関数********************
	//ランダムに敵の名前を返す(この値によって生成する敵を決める)
	std::string GetRandKindEnemyValue();
	//ランダムにアイテムの名前を返す(この値によって生成するアイテムを決める)
	std::string GetRandKindItemValue();

	//キャラのスキルデータをSkillManagerに渡す
	void GiveSkillEffectData();
	//キャラのスキルを取得する関数
	void SetCharaSkill(std::vector<Skill*>& skill, SkillManager::UseEffectType type, SkillManager::EffectName name);

	//プレイヤーのスキルをセットする
	void SetPlayerSkill();
	//敵スキルをセットする
	void SetEnemySkill(std::string name);
};