///*****Description**********************
///ゲーム機能全般に関するクラス
///ゲーム全体に対して関連する処理及びインスタンスを保有するクラス
/// ゲーム全般の機能クラス同士の処理などもここで行う
///**************************************
#pragma once
#include "Skill.h"
#include "SkillManager.h"
//前方宣言
class ObjectManager;
class Item;

class Character;
class Enemy;


class SceneManager;
class DebugManager;
class ResourceManager;
class MapManager;
class Collision;
class EffectManager;
class Menu;
class SoundManager;
class Inventory;

class GameManager {
public:
	
	GameManager();
	~GameManager();

	//StartPositionPlace
	enum class SetStartPositionType {
		PLAYER,//プレイヤー
		STAIR, //階段
		ENEMY, //敵
		ITEM   //アイテム
	};

	//ダンジョン種類(以下の列挙型の各値がuint32_t型として扱われる)
	//基底型 : uint32_t型
	enum class DungeonType : uint32_t {
		TOWER,		//0
		FOREST,		//1
		WATERWAY,	//2
		DUNGEONMAX	//3
	};

	enum class ScaleMode : uint32_t {
		NOMAL,
		ENLAEGE		//拡大
	};

	//************************変数*******************************

	const int MAPHEIGHT = 48;
	const int MAPWIDTH = 60;
	//マップチップの一枚の大きさ
	const int GRAPHICSIZE = 20;

	
	//************************関数*******************************
	
	void update(float delta_time);
	
	void Draw();
	//MiniMapをブレンドして描画
	void DrawBlendedMiniMap();


	//GameManagerの初期化
	void InitGameManager();

	void DungeonReCreate();
	inline void ForceInventoryChange(int& value) {
		value--;
		is_delete_invantory = false;
	}
	
	//ショップのアイテムをプレイヤーのインベントリに追加する関数
	void AddItemFromShop(std::shared_ptr<Item>shop_item);
	//インベントリにアイテムを追加する関数
	void AddItemToInventory(const std::string name, std::vector<Inventory*>& inventories, int& inventory_value);
	//インベントリからアイテムを削除する関数
	void EraseItemFromInventory(const int now_inventory_id);
	//ドロップアイテムリストから特定のアイテムを削除する関数
	bool DetectDropItem(std::shared_ptr<Item> item, std::list<std::shared_ptr<Item>>& it_list);
	

	//ダンジョンの生成
	void GenerateDungeon(DungeonType dungeontype);
	//ダンジョンマップの生成
	void GenerateDungeonMap(DungeonType dungeontype);


	bool CheckIsThereEnemy(tnl::Vector3 pos);
	
	//引数の敵がプレイヤーと隣り合っているか確認
	bool CheckNearByPlayer(std::shared_ptr<Enemy> enemy);
	//敵が死んでいるか判定
	void CheckIsDeadCharactor(std::shared_ptr<Character> object,tnl::Vector3 pos);
	
	
	//ペアにしたObjectをどうしが合ったっているかを判定
	void UpdateHitCheck();

	//HPが0のキャラクターの当たり判定を削除する
	void EraseHitList();

	//フライウェイトパターン版LoadGraph
	int LoadGraphEx(std::string_view graph);

	int CheckIsThere(tnl::Vector3 pos);

	//描画座標からマップ座標に変換
	tnl::Vector3 WorldToLocalPos(tnl::Vector3 pos);
	//マップ座標から描画座標に変換
	tnl::Vector3 LocalToWorldPos(int map_x, int map_y);

	//描画座標をローカル座標に変換
	tnl::Vector3 WorldToMap(const int world_x, const int world_y);
	//ローカル座標を描画座標に変換
	tnl::Vector3 MapToWorld(const int map_x, const int map_y);
	
	//画面の拡大縮小を変更する関数
	void ScaleChange();
	//スケールチェンジ時のポジション修正
	void ModifyScalePos();

	//MiniMapが描画できるかどうかを判定
	bool CanDrawMiniMap(tnl::Vector3 localpos);
	//MiniMapの更新
	void RefreshMiniMap(tnl::Vector3 localpos);
	
	
	//経路上の中間地点の座標をリサイズする
	//引数で渡された部屋の数に1を足した値を、waypointベクターのサイズに設定
	void InitWayPointVector(const int initroomnum);


	//================ダメージ関数==================
	// 
	//ダメージ計算処理
	void InflictDamageToTarget(Character* object, tnl::Vector3 pos);
	//ダメージ計算処理
	float CalculationDamage(int atk, int def);
	void CalculationSkillDamageToTarget(Character* object, const tnl::Vector3 pos, Skill* now_use_skill);
	float CalculationSkillDamage(const float skill_rate, const int atk, const int def);
	//ダメージ反映処理
	void AffectDamage(float damage, std::shared_ptr<Character>object);
	
	//カメラの中心座標をプレイヤーの座標に設定する
	void CameraReset();


	//==================================ゲッター========================================

	//現在座標の指定の方向の１つ先の座標を取得する
	inline const tnl::Vector3& GetFirstDirVector(int dir) const {
		return dir_vectors[dir];
	}

	//ダンジョンタイプが格納されている配列から名前を取得する
	inline std::string GetDungeonType(DungeonType dungeontype) {
		//underlying_type(列挙型の基底型を取得するための型特性)
		//static_castとは違い、型変換ではなく型の情報の提供を行うもの(データの損失を起こすことを防げる)
		return DUNGEONTYPE[std::underlying_type<DungeonType>::type(dungeontype)];
	}

	//現在のグラフィックサイズを返す
	float GetNowGraphSize() const {
		return now_gh_size;
	}

	//現在マップで買い物をしたフラグの取得
	bool GetDoneBuyInStores() const;

	//ダンジョンごとの画像を取得する
	std::vector<int>& GetGraphicsHandles(DungeonType dungeontype) const;
	//マップチップの情報を取得
	int GetMapChip(tnl::Vector3 mappos) const;

	//特定の座標の敵を取得する
	std::shared_ptr<Enemy> GetIsThereEnemyToDir(tnl::Vector3 pos);

	//引数に渡された部屋番号を座標から一番遠い出口を取得する関数
	tnl::Vector3 GetFarPoint(int room_id, tnl::Vector3 pos);

	//ランダムな値を返す関数
	inline int GetRandValue(int a, int b);

	int& GetInventoryValue() {
		return inventory_value;
	}

	std::vector<Inventory*>& GetInventorys() {
		return inventories;
	}

	bool GetIsDeleteInventotry() const {
		return is_delete_invantory;
	}

	bool GetIsDebug() const {
		return is_debug;
	}

	//現在の画像倍率を取得
	double GetRatio() const {
		return ratio;
	}

	//現在の描画状態を取得
	ScaleMode GetNowScale() const {
		return now_scale;
	}

	std::shared_ptr<Collision> GetCollision() {
		return collision;
	}

	std::shared_ptr<SoundManager> GetSoundManager() {
		return sound_manager;
	}

	std::shared_ptr<ResourceManager> GetResourceManager() {
		return re_manager;
	}

	std::shared_ptr<MapManager> GetMapManager() {
		return map_manager;
	}

	std::shared_ptr<ObjectManager> GetObjectManager() {
		return object_manager;
	}

	//==================================セッター========================================

	//現在のグラフィックサイズをセットする
	void SetNowGraphSize(int new_size) {
		now_gh_size = new_size;
	}

	//現在のマップで買い物をしたフラグの更新
	void SetDoneBuyInStores() const;

	//当たり判定をチェックするObjectどうしをペアにする
	void SetObjectSHitCheck();

	//通路生成時に
	void SetRoomWayPoint(const tnl::Vector3 pos, const int roomid);

	//プレイヤーの部屋番号を変更する
	void SetPlayerRoomValue(int nowroomvalue);

	//マップ内のランダムな部屋を取得
	//部屋の中のランダムな座標を取得
	//座標を描画座標に変換して返す
	tnl::Vector3 SetStartPosition(SetStartPositionType type);

	void SetIsDeleteInventory(bool now_is_delete) {
		is_delete_invantory = now_is_delete;
	}

	bool SetIsDebug(bool now_debug) {
		is_debug = now_debug;
	}

	//画像倍率を更新
	void SetRatio(double now_ratio) {
		ratio = now_ratio;
	}

	//スケール状態を変更
	ScaleMode SetNowScale(ScaleMode new_scale) {
		now_scale = new_scale;
	}
	//===========================================
	
	//インベントリ数を1加算する
	void AddInventoryValue() {
		inventory_value++;
	}
	//インベントリ数を1減算する
	void SubtractionInventoryValue() {
		inventory_value--;
	}

private:
 
	enum class Vec {
		DOWN,
		LEFT,
		RIGHT,
		UP
	};

	std::shared_ptr<ObjectManager> object_manager = nullptr;
	std::shared_ptr<MapManager> map_manager = nullptr;
	std::shared_ptr<ResourceManager> re_manager = nullptr;
	std::shared_ptr<SoundManager>sound_manager = nullptr;
	std::shared_ptr<Collision> collision = nullptr;
	DebugManager* debug_manager = nullptr;
	Inventory* inventory = nullptr;
	Inventory* bankinventory = nullptr;


	//========================変数======================================


	//これはObjectが持ってもいいかも
	//animation描画座標
	tnl::Vector3 dir_vectors[4]{
		tnl::Vector3(0,1,0),	//down
		tnl::Vector3(-1,0,0),	//left
		tnl::Vector3(1,0,0),	//right
		tnl::Vector3(0,-1,0)	//up

	};

	//インベントリが削除されたかどうかのflag
	bool is_delete_invantory = false;

	//プレイヤー変数一時保管用
	tnl::Vector3 player_pos_buff = {};

	std::vector<std::vector<tnl::Vector3>> waypoint;
	//LoadGraphExで使用(画像を格納)
	std::unordered_map<std::string_view, int> map_graph;

	DungeonType nowdungeon;
	//ダンジョンタイプ
	const std::string DUNGEONTYPE[3] = { "塔","森","水" };


	//アイテムを追加する際の格納可能なインベントリ配列番号
	int inventory_value = 0;
	//UIインベントリに描画するリスト
	std::vector<Inventory*>inventories;
	//campシーンの倉庫インベントリ配列
	std::vector<Inventory*>bank_inventroy_list;

	//現在のグラフィックサイズ
	float now_gh_size = GRAPHICSIZE;

	//倍率(主にGraphicsで使用)
	double ratio = 1.0;
	const float scale[2] = { 20.0f,40.0f };
	//現在の描画方法
	ScaleMode now_scale = ScaleMode::NOMAL;

	//debug切り替え
	bool is_debug = false;

};

