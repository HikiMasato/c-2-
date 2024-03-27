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
//コンストラクタ
SkillManager::SkillManager(std::vector<std::vector<int>>& re_effect_int, std::vector<std::vector<std::string>>& re_effect_string, GameManager* game_manager_)
{
	game_manager = game_manager_;
	//re_graphicに格納されているstring型の数値は使用しない
	//引数で渡された2種類のvector配列を1つにまとめる	
	effe_info.resize(re_effect_int.size());
	for (size_t i = 0; i < re_effect_int.size(); i++) {

		effe_info[i] = std::make_pair(re_effect_int[i], re_effect_string[i]);

	}
	skill_list.resize(effe_info.size());

	LoadSkill();
}

//------------------------------------------------------------------------------------------------------------
//skillリストにスキルを追加
void SkillManager::LoadSkill()
{
	//データの数だけ回す
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

		//UseEffectTypeの列挙子の値と同じ数値の配列番号で1次元目を分けている
		//一次元目の配列番号
		//0…PLAYER
		//1…ENEMY
		//2…ITEM
		//3…OTHERS
		//4…BOTHCHARACTOR
		skill_list[effe_info[i - 1].first[0]].emplace_back(skill);
	}
	tnl::DebugTrace("\nスキルロード完了\n");
}

//------------------------------------------------------------------------------------------------------------
//skillリストから特定の要素を取得する
Skill* SkillManager::GetSkill(UseEffectType effect_type_, EffectName skill_name_)
{
	//UseEffectTypeの列挙子の値と同じ数値の配列番号で1次元目を分けている
	//EffectNameの列挙子を選択することで列挙子に割り振られた値が2次元目の配列番号になる
	return skill_list[(int)effect_type_][(int)skill_name_];
}
