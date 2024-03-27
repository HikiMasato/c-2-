///*****************************************
/// 
/// 
///  
///***************************************** 
#pragma once

class Object;
class Character;
class EnemyManager;
class GameManager;
class EffectManager;

class Enemy final : public Character {
public:
	Enemy(){}
	Enemy(tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, GameManager* game_manager_);
	~Enemy();

	ObjectType GetObjectType() const override {
		return ObjectType::ENEMY;
	}

	void Update(float delta_time)override;

	//敵行動処理関数
	bool EnemyMove();
	//目的地に向かう関数
	void MoveDestination();

	//自分の体の向きに進む関数
	bool MoveToDir(const  MoveDir dir, tnl::Vector3 pos);

	//自分のひとます先が移動可能な場所かを調べる関数
	bool CheckCanMove(const MoveDir dir, const tnl::Vector3 pos);

	//自分の現在向いている方向から特定の方向へ行けるかどうかをチェックする関数
	//arg1…現在向いている方向
	//arg3…自分が現在向いている方向から行けるか調べたい方向
	bool CheckCanMoveToDir(const MoveDir dir, const tnl::Vector3 pos, const MoveDir check_dir);

	//EnemyAttackで関数でランダム、その時の状態によって発動するスキルを変更する
	std::shared_ptr<EffectManager> EnemyAttack(Skill* last_skill, std::list<std::shared_ptr<EffectManager>> draw_effect_list, std::shared_ptr<Enemy>enemy_);

	//EnemyAnimationでEnemyAttacで決まったスキルを受け取り、そのスキルのAnimationを戻り値で返せるようにする
	std::shared_ptr<EffectManager> EnemyEffect(std::vector<int> skill_list, tnl::Vector3 pos, int anim_speed, int index);


private:
	//目的地の座標
	tnl::Vector3 destination_pos;
	//現在座標
	tnl::Vector3 ene_now_pos;

	//部屋外であれば-1
	int ene_now_in_room = -1;

	//プレイヤーを発見しているかどうかのフラグ
	bool pl_discovery = false;
	//プレイヤーがいない場合他に目的地が設定されているかどうかのフラグ
	bool other_discovery = false;
	//同じ部屋かどうかのフラグ
	bool same_room = false;
	//
	bool is_not_wall[4];

	//方向を表す単位ベクトル
	tnl::Vector3 vec[4]{
		vec[(int)MoveDir::DOWN]  = { 0, 1, 0 },
		vec[(int)MoveDir::LEFT]	 = { -1, 0, 0 },
		vec[(int)MoveDir::RIGHT] = { 1, 0, 0 },
		vec[(int)MoveDir::UP]	 = { 0, -1, 0 }
	};

	//発見フラグ維持範囲
	const float  DISCOVERYRANGE = 9;
	//コスト
	float cost[(int)MoveDir::DIRMAX];

	//ヒューリスティック関数を定義する
	//ここではユークリッド距離を使う
	float Heuristic(tnl::Vector3 node, tnl::Vector3 goal) {
		return sqrt(pow(node.x - goal.x, 2) + pow(node.y - goal.y, 2));
	}

	//周囲にプレイヤーがいるかどうかをチェックする
	bool CheckIsTherePlayer(MoveDir dir, tnl::Vector3 pos);

	//dirから見て特定の座標がどの向きかを取得する
	MoveDir GetDir(const MoveDir dir, const MoveDir getdir);

	//目的地をリセットする関数
	void ResetDistination();
	//プレイヤーとの距離が離れすぎたら発見フラグを下す関数
	void ResetPlayerDiscovery();

	//dir方向にただ進む関数
	//<注意事項>
	//この関数の使用時は移動可能なことを保証すること
	//この関数自身は移動可能判定を行っていない
	void SolyMoveToDir(const MoveDir dir);
	
	GameManager* game_manager;
protected:
};