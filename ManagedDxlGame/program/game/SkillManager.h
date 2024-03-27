///************Description***************
///
/// 
/// 
///**************************************
#pragma once

#include "Skill.h"
class GameManager;

class SkillManager {
public:

	SkillManager() {}
	SkillManager(std::vector<std::vector<int>>& re_effect_int, std::vector<std::vector<std::string>>& re_effect_string,GameManager* game_manager_);


	//effectを使う使用者の種類
	enum class UseEffectType : int {
		PLAYER,
		ENEMY,
		ITEM,		//(WIP)
		OTHERS,
		BOTHCHARACTOR
	};

	//Effectの名前(名前を選択することで2次元配列の配列番号の指定になる)
	enum class EffectName : int {

		NOMALATTCK = 0,
		BRACKMAGIC = 0,
		RECOVERY = 1,
		WATER = 1,
		BUFF = 2,
		DROP = 2,
		WIND = 2,
		DEBUFF = 3,
		LEVEL = 3,
		ROCK = 3,
		FIRE = 4,
		THUNDER = 5

	};


	void LoadSkill();
	Skill* GetSkill(UseEffectType effect_type_, EffectName skill_name_);


private:
	GameManager* game_manager;

	//skillのデータがすべて格納されている
	std::vector<std::vector<Skill*>>skill_list;

	//effectのステータスと名前、画像ハンドルを一つのvectorにまとめる
	std::vector<std::pair<std::vector<int>, std::vector<std::string>>> effe_info;

	std::vector<std::vector<Skill*>>skill;

};