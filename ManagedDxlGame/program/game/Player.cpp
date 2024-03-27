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
#include "ObjectManager.h"
#include "GameManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "Character.h"
#include "MyCamera.h"
#include "Player.h"
//-------------------------------------------------------
//Effect
#include "ActionEffect.h" 
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "MapChip.h"




//------------------------------------------------------------------------------------------------------------
//コンストラクタ
Player::Player(tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, GameManager* game_manager_)
{
	game_manager = game_manager_;

	SetCharaPos(start_pos);
	SetCharaStetus(Character::Stetus::HP, hp);
	SetCharaStetus(Character::Stetus::MP, mp);
	SetCharaStetus(Character::Stetus::ATK, attack);
	SetCharaStetus(Character::Stetus::DEF, defence);
	SetCharaStetus(Character::Stetus::EXP, exp);
	SetCharaStetus(Character::Stetus::LEVEL, level);
	SetLeftOverExp(need_level_up_exp);

	LoadDivGraph(chara_ph.c_str(), chra_chip_all, 3, 4, chra_chip_size, chra_chip_size, const_cast<int*>(GetCharaChipArray()));
	
	SetCharaInPos(game_manager->WorldToLocalPos(GetCharaPos()));
	
	//最大HPを更新
	SetPlayerMaxHp(hp);

}

//------------------------------------------------------------------------------------------------------------
Player::~Player() 
{
	tnl::DebugTrace("\n===================Playerデストラクタ処理===================\n");
}

//------------------------------------------------------------------------------------------------------------
//毎フレーム実行
void Player::Update(float delta_time) {

	//デバッグHp変動関数
	DebugHpFluctuation();
	Move();
}

//------------------------------------------------------------------------------------------------------------
//キャラクターを描画座標からマップ座標に変換
void Player::SetPlayerLocalPos()
{
	SetCharaInPos(game_manager->WorldToLocalPos(GetCharaInPos()));
}

//------------------------------------------------------------------------------------------------------------
//WASDキーによってそれぞれの方向に移動する関数
void Player::Move()
{
	tnl::Vector3 pos = GetCharaPos();
	tnl::Vector3 pl_now_pos = game_manager->WorldToLocalPos(pos);

	if (tnl::Input::IsKeyDown(eKeys::KB_W)) {
		SetDir((int)MoveDir::UP);
		//移動関数
		SetMoveCharaPos(MoveDir::UP);
		if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::STARTMAP) {
			//移動関数
			SetMoveCharaPos(MoveDir::UP);

		}

		
	}
	else if (tnl::Input::IsKeyDown(eKeys::KB_S)) {
		SetDir((int)MoveDir::DOWN);
		//移動関数
		SetMoveCharaPos(MoveDir::DOWN);
		if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::STARTMAP) {
			//移動関数
			SetMoveCharaPos(MoveDir::DOWN);
			
		}
	}
	else if (tnl::Input::IsKeyDown(eKeys::KB_A)) {
		SetDir((int)MoveDir::LEFT);
		//移動関数
		SetMoveCharaPos(MoveDir::LEFT);
		if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::STARTMAP) {
			//移動関数
			SetMoveCharaPos(MoveDir::LEFT);

		}
	}
	else if (tnl::Input::IsKeyDown(eKeys::KB_D)) {
		SetDir((int)MoveDir::RIGHT);
		//移動関数
		SetMoveCharaPos(MoveDir::RIGHT);
		if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::STARTMAP) {
			//移動関数
			SetMoveCharaPos(MoveDir::RIGHT);

		}
	}

	if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::DUNGEON) {
		//移動後の部屋に変更する
		game_manager->SetPlayerRoomValue(game_manager->CheckIsThere(pl_now_pos));

	}
	
}


//------------------------------------------------------------------------------------------------------------
//アイテム使用時にプレイヤーのステータスを変動させる
//stetustype:0->HP,stetustype:1->ATK,stetustype:2->DEF,stetustype:3->GOLD,stetustype:4->DIAMOND
void Player::EffectToHp(Stetus stetustype, int stetus)
{
	switch (stetustype)
	{
	case Character::Stetus::HP:
		//Hpを加算する
		SetCharaStetus(Character::Stetus::HP, stetus);
		break;
	case Character::Stetus::ATK:
		//Attackを上昇する
		SetCharaStetus(Character::Stetus::ATK, stetus);//(WIP)
		break;
	case Character::Stetus::DEF:
		//Defenceを上昇する
		SetCharaStetus(Character::Stetus::DEF, stetus);
		break;
	case Character::Stetus::GOLD:
		//所持金を加算する
		SetCharaStetus(Character::Stetus::GOLD, stetus);
		break;
	case Character::Stetus::DIAMOND:
		//所持ダイアモンドを増やす
		SetCharaStetus(Character::Stetus::DIAMOND, stetus);
		break;
	default:
		tnl::DebugTrace("\nステータス例外\n");
		break;
	}
}

//------------------------------------------------------------------------------------------------------------
//プレイヤーの経験値を加算する関数
//経験値が最大になればレベルが上がる
bool Player::AddExp(int add_exp)
{
	//プレイヤーの現在取得経験値を更新
	SetCharaStetus(Character::Stetus::EXP, add_exp);

	//プレイヤーの現在の経験値を取得
	int get_pl_exp = GetCharaStetus(Character::Stetus::EXP);
	//取得した経験値を加算
	get_pl_exp += add_exp;

	//レベルアップまでの必要経験値を計算
	leftover_exp = (need_level_up_exp - get_pl_exp);
	//必要経験値を更新
	SetLeftOverExp(leftover_exp);

	//現在の経験値が必要経験値以上ならレベルをあげる
	if (get_pl_exp >= need_level_up_exp) {
		
		//必要経験値以上なら
		if (get_pl_exp > need_level_up_exp) {
			//過剰取得経験値を計算
			over_need_exp = (need_level_up_exp - get_pl_exp);
		}
		
		//レベルを1上げる
		SetAffectToCharaStetus(Character::Stetus::LEVEL, +1);

		//次のレベルに必要な経験値を計算する
		NeedUpExp();

		//ステータスをランダムに強化する
		ReinforceStetus();

		//現在HPを取得
		int now_hp = GetCharaStetus(Character::Stetus::HP);
		//最大HPより現在HPのほうが多ければ
		if (now_hp > max_hp) {
			//最大HPを更新
			SetPlayerMaxHp(now_hp);
		}
		

		tnl::DebugTrace("\n============レベルが上がった=============\n");
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------
//必要経験値を増加させる関数
void Player::NeedUpExp()
{
	//現在のプレイヤーのレベルを取得
	int now_level = GetCharaStetus(Character::Stetus::LEVEL);
	
	//必要経験値 = (現在のレベル * 1.5) + 50
	//レベルが上がるごとに線形に必要経験値が上がっていく
	need_level_up_exp = (now_level * EXPINCREASERATE) + BASENEEDEXP;

	//次のレベルまでの残り経験値を更新
	SetLeftOverExp(need_level_up_exp);
	tnl::DebugTrace("\n============必要経験値計算終了(NeedUpExp)=============\n");

}

//------------------------------------------------------------------------------------------------------------
void Player::DebugHpFluctuation()
{
	if (tnl::Input::IsKeyDown(eKeys::KB_P)) {
		SetAffectToCharaStetus(Character::Stetus::HP, -5);
	}
	if (tnl::Input::IsKeyDown(eKeys::KB_O)) {
		SetAffectToCharaStetus(Character::Stetus::HP, +5);
		//現在HPを取得
		int hp = GetCharaStetus(Character::Stetus::HP);
		
		//最大HPより現在HPのほうが多ければ
		if (max_hp < hp) {
			//最大HP更新
			SetPlayerMaxHp(GetCharaStetus(Character::Stetus::HP));
		}
		
	}
}
