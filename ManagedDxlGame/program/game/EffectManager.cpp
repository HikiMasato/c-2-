//-------------------------------------------------------
//既存のもの
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//個人的に追加した機能
#include <string_view>
#include <random>         
#include <iostream>  
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "SceneManager.h"
#include "GameManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
#include "SkillManager.h"
#include "EffectManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MyCamera.h"
#include "Player.h"
//-------------------------------------------------------
//Effect
#include "ActionEffect.h" 
//-------------------------------------------------------
//Others
#include "Skill.h"
#include "Stetus.h"


//------------------------------------------------------------------------------------------------------------
//コンストラクタ
EffectManager::EffectManager(std::vector<std::vector<std::string>>& effect_csv)
{
	all_effct.resize(effect_csv.size());
	all_effct = effect_csv;
}

//------------------------------------------------------------------------------------------------------------
//レベルアップ用コンストラクタ
EffectManager::EffectManager(std::string path, tnl::Vector3 pos)
{
	LoadDivGraph(path.c_str(), 10, 5, 2, 120, 120, levelup_gh_array);

	effect_pos = pos;
	speed = 10;
	max_motion_index = 10;
	atk_type = AttackType::LEVEL;

}

//------------------------------------------------------------------------------------------------------------
//スキルエフェクト用コンストラクタ
EffectManager::EffectManager(std::vector<int> anim, tnl::Vector3 anim_pos, int act_speed, int max_index)
{
	anim_gh = anim;
	
	effect_pos = anim_pos;
	speed = act_speed;
	max_motion_index = max_index;
	atk_type = AttackType::ATTACK;
}

//------------------------------------------------------------------------------------------------------------
//毎フレーム実行　
void EffectManager::Update(float delta_time)
{

	if (--effect_wait <= 0) {
		effect_index++;
		effect_wait = speed;
		effect_index %= max_motion_index;
	}
	else return;
	
	if (atk_type == AttackType::ATTACK) {
		
		effect_gh = anim_gh[effect_index];
	}
	else {

		effect_gh = levelup_gh_array[effect_index];

	}

	if (atk_type == AttackType::ATTACK) {
		if (effect_gh == anim_gh[max_motion_index - 1]) {
			anim_is_alive = false;

		}
	}
	else {
		if (effect_gh == levelup_gh_array[max_motion_index - 1]) {
			anim_is_alive = false;
		}
	}
}

//------------------------------------------------------------------------------------------------------------
//描画
void EffectManager::Draw(const hm::Camera& camera)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawRotaGraph(effect_pos.x - camera.cameraPos.x, effect_pos.y - camera.cameraPos.y, 1.0f, 0, effect_gh, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}
