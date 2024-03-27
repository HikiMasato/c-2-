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


	//effect���g���g�p�҂̎��
	enum class UseEffectType : int {
		PLAYER,
		ENEMY,
		ITEM,		//(WIP)
		OTHERS,
		BOTHCHARACTOR
	};

	//Effect�̖��O(���O��I�����邱�Ƃ�2�����z��̔z��ԍ��̎w��ɂȂ�)
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

	//skill�̃f�[�^�����ׂĊi�[����Ă���
	std::vector<std::vector<Skill*>>skill_list;

	//effect�̃X�e�[�^�X�Ɩ��O�A�摜�n���h�������vector�ɂ܂Ƃ߂�
	std::vector<std::pair<std::vector<int>, std::vector<std::string>>> effe_info;

	std::vector<std::vector<Skill*>>skill;

};