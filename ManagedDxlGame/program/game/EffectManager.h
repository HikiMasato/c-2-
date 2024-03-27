///*************Description*********************
/// effect再生を管理するクラス
/// animation描画を行っている
/// 
///*********************************************
#pragma once
class hm::Camera;

class EffectManager {
public:
	EffectManager(){}
	//effect_csv…[0]～[6]まで攻撃Effect
	//effect_csv…[7]からはゲームに関するEffect
	EffectManager(std::vector<std::vector<std::string>>& effect_csv);
	//レベルアップ用
	EffectManager(std::string path, tnl::Vector3 pos);
	//スキルエフェクト用コンストラクタ
	EffectManager(std::vector<int>anim,tnl::Vector3 anim_pos,int act_speed,int max_index);

	enum class AttackType {
		NONE,
		ATTACK,
		MAGIC,
		LEVEL
	};

	
	void Update(float delta_time);
	void Draw(const hm::Camera& camera);

	
	//アニメーションが終了してるかどうかのフラグを取得する関数
	inline bool GetAnimIsAlive() {
		return anim_is_alive;
	}

private:

	
	AttackType atk_type = AttackType::NONE;

	//変数宣言
	tnl::Vector3 effect_pos{ 0 };
	int effect_wait = 0;
	int speed = 20;
	int effect_index = 0;
	int effect_gh = 0;
	int max_motion_index = 0;
	int drawph = 0;

	bool anim_is_alive = true;

	//レベルアップ用配列
	int levelup_gh_array[10] = {};

	std::vector<int>anim_gh;
	std::vector<std::vector<std::string>> all_effct;

};