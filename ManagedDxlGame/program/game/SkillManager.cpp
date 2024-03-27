//-------------------------------------------------------
//�����̂���
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//�l�I�ɒǉ������@�\
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
#include "ResourceManager.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "SkillManager.h"
#include "MapManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "Player.h"
#include "Enemy.h"
#include "MyCamera.h"
//-------------------------------------------------------
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//others
#include "Stetus.h"
#include "Skill.h"
#include "Debug.h"


//------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
SkillManager::SkillManager(std::vector<std::vector<int>>& re_effect_int, std::vector<std::vector<std::string>>& re_effect_string, GameManager* game_manager_)
{
	game_manager = game_manager_;
	//re_graphic�Ɋi�[����Ă���string�^�̐��l�͎g�p���Ȃ�
	//�����œn���ꂽ2��ނ�vector�z���1�ɂ܂Ƃ߂�	
	effe_info.resize(re_effect_int.size());
	for (size_t i = 0; i < re_effect_int.size(); i++) {

		effe_info[i] = std::make_pair(re_effect_int[i], re_effect_string[i]);

	}
	skill_list.resize(effe_info.size());

	LoadSkill();
}

//------------------------------------------------------------------------------------------------------------
//skill���X�g�ɃX�L����ǉ�
void SkillManager::LoadSkill()
{
	//�f�[�^�̐�������
	for (int i = 1; i < effe_info.size(); i++) {
		Skill* skill = (new Skill(
			effe_info[i - 1].first[0],	//EffectType
			effe_info[i - 1].first[1],	//DamageRate
			effe_info[i - 1].first[2],	//HealRate
			effe_info[i - 1].first[3],	//AllNum
			effe_info[i - 1].first[4],	//XNum
			effe_info[i - 1].first[5],	//YNum
			effe_info[i - 1].first[6],	//XSize
			effe_info[i - 1].first[7],	//YSize
			effe_info[i - 1].first[8],	//ActSpeed
			effe_info[i].second[9],		//SkillType
			effe_info[i].second[10],	//SkillName
			effe_info[i].second[11],	//SkillHnandle
			game_manager
		));	

		//UseEffectType�̗񋓎q�̒l�Ɠ������l�̔z��ԍ���1�����ڂ𕪂��Ă���
		//�ꎟ���ڂ̔z��ԍ�
		//0�cPLAYER
		//1�cENEMY
		//2�cITEM
		//3�cOTHERS
		//4�cBOTHCHARACTOR
		skill_list[effe_info[i - 1].first[0]].emplace_back(skill);
	}
	tnl::DebugTrace("\n�X�L�����[�h����\n");
}

//------------------------------------------------------------------------------------------------------------
//skill���X�g�������̗v�f���擾����
Skill* SkillManager::GetSkill(UseEffectType effect_type_, EffectName skill_name_)
{
	//UseEffectType�̗񋓎q�̒l�Ɠ������l�̔z��ԍ���1�����ڂ𕪂��Ă���
	//EffectName�̗񋓎q��I�����邱�Ƃŗ񋓎q�Ɋ���U��ꂽ�l��2�����ڂ̔z��ԍ��ɂȂ�
	return skill_list[(int)effect_type_][(int)skill_name_];
}
