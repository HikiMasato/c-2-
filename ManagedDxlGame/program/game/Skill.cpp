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
#include "Stetus.h"
#include "Skill.h"


//------------------------------------------------------------------------------------------------------------
//コンストラクタ
Skill::Skill(int effect_type_, float damege_rate_, float heal_rate_,int all_num_, int x_num_, int y_num_, int x_size_, int y_size_, int act_speed_, 
	         std::string skill_type_, std::string skill_name_, std::string gh_handles_,GameManager* game_manager)
{
	//引数のデータを渡す
	effect_type = effect_type_;
	
	damage_rate = damege_rate_;
	heal_rate = heal_rate_;

	float_data[0] = damage_rate;
	float_data[1] = heal_rate;

	//文字列
	//skill_type = skill_type_;
	skill_name = skill_name_;
	gh_handles = gh_handles_;

	game_manager->GetResourceManager()->LoadDivGraphEx(gh_handles, all_num_, x_num_, y_num_, x_size_, y_size_, anim_gh);

	graphic_all_num = all_num_;
	x_num = x_num_;
	y_num = y_num_;

	x_size = x_size_;
	y_size = y_size_;

	anim_speed = act_speed_;

}
