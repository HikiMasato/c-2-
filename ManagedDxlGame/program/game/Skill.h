///***************Description***************
///スキルに関するクラス
///プレイヤーとエネミー双方が持つことが出来るスキルの情報,処理を保持する
///*****************************************
#pragma once

class GameManager;

class Skill {
public:

	Skill(){}
	Skill(int effect_type_, float damege_rate_, float heal_rate_, int all_num_,int x_num_, int y_num_, int x_size_, int y_size_, int act_speed_, 
		  std::string skilltype_, std::string skill_name_, std::string gh_handles_, GameManager* game_manager);

	//ダメージ、回復量を取得
	inline const float* GetSkillFloat() {
		return float_data;
	}
	//スキル名を取得
	inline const std::string& GetSkillName() {
		return skill_name;
	}
	//アニメーション画像ハンドルの総数を取得
	inline const int& GetGraphicsAllNum() {
		return graphic_all_num;
	}
	//アニメーション画像ハンドルvectorを取得する
	inline const std::vector<int>& GetGraphicsVector() {
		return anim_gh;
	}
	//アニメーションスピードを取得
	inline const int& GetAnimSpeed() {
		return anim_speed;
	}


private:

	//スキルId
	int effect_type = 0;
	//スキルタイプ
	std::string skill_type;
	//スキル名
	std::string skill_name;

	//以下2つをまとめたもの
	float float_data[2];
	//攻撃力にかかる倍率
	float damage_rate = 0;
	//ダメージにかかる倍率
	float heal_rate = 0;
	
	//フォルダパス
	std::string gh_handles;
	//DivGraphで分割した画像ハンドル
	std::vector<int>anim_gh;

	//画像の情報
	int graphic_all_num = 0;
	int x_num = 0;
	int y_num = 0;

	int x_size = 0;
	int y_size = 0;

	int anim_speed = 0;

};