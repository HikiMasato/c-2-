///******************Description************************
///プレイヤークラス
/// 移動処理や、装備時のステータス変動などを管理(現在未実装)
/// 
/// 
///***************************************************** 
#pragma once
#include "Object.h"
#include "Character.h"
class Object;
class Character;
class GameManager;
class hm::Camera;

class Player final : public Character {
public:

	//デフォルトコンストラクタ
	Player(){}

	//arg1…プレイヤーのスタートポジションをランダムに設定する
	//arg2…hp
	//arg3…mp
	//arg4…defence
	//arg5…名前
	//arg6…画像パス
	Player(tnl::Vector3 start_pos,int hp, int mp, int attack, int defence,std::string name ,std::string chara_ph, int exp, int level, GameManager* game_manager_);
	//デストラクタ
	~Player();
	
	

	void Update(float delta_time) override;

	//プレイヤー移動
	void Move();

	//アイテム使用時にプレイヤーのステータスを変動させる
	//stetustype:0->HP,stetustype:1->ATK,stetustype:2->DEF,stetustype:3->GOLD,stetustype:4->DIAMOND
	void EffectToHp(Stetus stetustype,int stetus);

	//プレイヤーの経験値を加算する関数
	//経験値が最大になればレベルが上がる
	bool AddExp(int add_exp);
	//================ゲッター===================

	ObjectType GetObjectType() const override {
		return ObjectType::PLAYER;
	}

	//プレイヤーのスターとマップポジションを返す
	tnl::Vector3 GetStartPostion() const {
		return start_pl_pos;
	}

	int GetNeedExp() const {
		return need_level_up_exp;
	}

	int GetLeftOverExp() const {
		return leftover_exp;
	}

	int GetPlayerMaxHp() const {
		return max_hp;
	}

	//================セッター===================
	
	//キャラクターを描画座標からマップ座標に変換
	void SetPlayerLocalPos();

	void SetNeedExp(int now_exp) {
		need_level_up_exp = now_exp;
	}

	//レベルアップまでの残り経験値を更新する
	void SetLeftOverExp(int now_exp) {
		leftover_exp = now_exp;
	}

	void SetPlayerMaxHp(int new_max_hp) {
		max_hp = new_max_hp;
	}

private:
	GameManager* game_manager;

	//プレイヤーのスターとマップポジション
	tnl::Vector3 start_pl_pos = { 700, 600, 0 };

	//レベルアップに必要な経験値(1lv上がるごとに必要経験値も上がっていく)
	int need_level_up_exp = 100;
	//レベルアップに必要な残り経験値
	int leftover_exp = 0;
	//必要経験値を超えた取得経験値を保存する
	int over_need_exp = 0;
	//必要経験値を計算する際に使用するベース必要経験値
	const int BASENEEDEXP = 55;
	//経験値増加率
	const float EXPINCREASERATE = 10.5; 

	//プレイヤーの最大HP
	int  max_hp = 0;

	//必要経験値を増加させる関数
	void NeedUpExp();

	//debug用にHpの増減処理
	void DebugHpFluctuation();
};