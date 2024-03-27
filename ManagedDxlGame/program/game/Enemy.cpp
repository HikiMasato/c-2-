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
#include <list>
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "DungeonScene.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "SceneManager.h"
#include "GameManager.h"
#include "ObjectManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
#include "EnemyManager.h"
#include "EffectManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "Character.h"
#include "MyCamera.h"
#include "Enemy.h"
#include "Player.h"
#include "MapChip.h"
//-------------------------------------------------------
//Collision
#include "Collision.h"
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Debug.h"



//------------------------------------------------------------------------------------------------------------
//コンストラクタ
Enemy::Enemy(tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, GameManager* game_manager_)
{
	game_manager = game_manager_;
	SetCharaPos(start_pos);

	//基礎値 + 強化値
	SetCharaStetus(Character::Stetus::HP, hp);
	SetCharaStetus(Character::Stetus::MP, mp);
	SetCharaStetus(Character::Stetus::ATK, attack);
	SetCharaStetus(Character::Stetus::DEF, defence);
	SetCharaStetus(Character::Stetus::EXP, exp);
	SetCharaStetus(Character::Stetus::LEVEL, level);

	LoadDivGraph(chara_ph.c_str(), chra_chip_all, 3, 4, chra_chip_size, chra_chip_size, const_cast<int*>(GetCharaChipArray()));
	
	SetCharaInPos(game_manager->WorldToLocalPos(GetCharaPos()));
}

//------------------------------------------------------------------------------------------------------------
//デストラクタ
Enemy::~Enemy()
{
	tnl::DebugTrace("\n===================Enemyデストラクタ処理===================\n");
}

//------------------------------------------------------------------------------------------------------------
//毎フレーム実行
void Enemy::Update(float delta_time)
{
	EnemyMove();
}

//------------------------------------------------------------------------------------------------------------
//敵行動関数
bool Enemy::EnemyMove()
{
	tnl::Vector3 pos = GetCharaPos();

	//敵の位置がマップ上のどこかを特定
	ene_now_pos = game_manager->WorldToLocalPos(pos);

	//敵が部屋のどこかにいるなら部屋番号を取得
	ene_now_in_room = game_manager->GetMapManager()->CheckIsThere(ene_now_pos.x, ene_now_pos.y);

	//敵が部屋外ではない、かつプレイヤーと部屋番号が同じであるなら
	if (game_manager->GetObjectManager()->factory->GetPlayer()->GetNowRoomValue() == ene_now_in_room && ene_now_in_room != -1) {
		same_room = true;
	}
	//上記の条件に一致しなければ
	else {
		same_room = false;
	}

	//プレイヤーを発見していたら
	if (pl_discovery) {
		tnl::Vector3 player_pos = game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
		//目的地の座標にプレイヤーの座標を設定
		destination_pos = game_manager->WorldToLocalPos(player_pos);
		//経路探索
		MoveDestination();

		return true;
	}
	//目的地がセットされていれば
	if (other_discovery && !same_room) {
		//経路探索
		MoveDestination();
		return true;
	}
	//プレイヤーと同じ部屋の場合は目的地はプレイヤーのはず
	else if (other_discovery && same_room) {
		tnl::Vector3 player_pos = game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();

		//目的地の座標にプレイヤーの座標を設定
		destination_pos = game_manager->WorldToLocalPos(player_pos);
		other_discovery = true;
		pl_discovery = true;
		//経路探索
		MoveDestination();

	}
	//部屋にいる場合
	if (ene_now_in_room != -1) {
		//その部屋の出口の中から一番遠い出口を取得する
		tnl::Vector3 way_point = game_manager->GetFarPoint(ene_now_in_room, ene_now_pos);
		//目的地のリセット
		destination_pos = (0);
		//取得した出口を目的地にする
		destination_pos = way_point;
		//プレイヤー発見
		pl_discovery = true;
		//経路探索
		MoveDestination();
		return true;
	}
	//通路にいる場合
	else {
		//自身の方向に進めるかどうかのフラグ
		bool can_move = MoveToDir(GetCharaDir(), ene_now_pos);
		//上のboolまでは通るの確認できた
		//通路に侵入すると敵が動かなくなる
		//通路進入時にちゃんと進むようにする
		if (!can_move) {
			//もし左右に進めるなら
			if (CheckCanMoveToDir(GetCharaDir(), ene_now_pos, MoveDir::LEFT) && CheckCanMoveToDir(GetCharaDir(), ene_now_pos, MoveDir::RIGHT)) {

				if (rand() % 2 == 0)SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::LEFT));
				else SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::RIGHT));
			}
			//左に進めるか確認
			else if (CheckCanMoveToDir(GetCharaDir(), ene_now_pos, MoveDir::LEFT))SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::LEFT));
			//左がいけなければ右も確認
			else if (CheckCanMoveToDir(GetCharaDir(), ene_now_pos, MoveDir::RIGHT))SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::RIGHT));
			//左右がいけなければ
			else {
				SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::DOWN));
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------
//目的地に向かう関数(経路探索関数)
void Enemy::MoveDestination()
{

	//ダイクストラ法
	//A*法(Aスター法)
	//ヒューリスティック関数
	   
	//敵がいるときにそのノードを通過できないようにする
	//game_manager->GetMapChip(myNowPos + v[i]) == 0 という条件を追加する
	for (int i = 0; i < (int)MoveDir::DIRMAX; i++) {
		//敵の現在座標からすべての方向(4方向)のマップタイプを調べて壁ではない、かつすべての方向(4方向)に敵がいなければtrueが入る
		is_not_wall[i] = game_manager->GetMapChip(ene_now_pos + vec[i]) != MapChip::MapType::WALL
			&& !game_manager->CheckIsThereEnemy(ene_now_pos + vec[i]);
	}

	//コストが最小になるように進む
	//ヒューリスティック関数の値をコストに加算する
	//最小コストを求めるために、コストの配列を用意する
	for (int i = 0; i < (int)MoveDir::DIRMAX; i++) cost[i] = FLT_MAX;

	int min_dir = -1;

	float min_cost = FLT_MAX;
	for (int i = 0; i < (int)MoveDir::DIRMAX; i++) {
		if (is_not_wall[i]) {
			//コストを計算する
			cost[i] = game_manager->GetMapChip(ene_now_pos + vec[i]) + Heuristic(ene_now_pos + vec[i], destination_pos);
			//最小コストを更新する
			if (cost[i] < min_cost) {
				min_cost = cost[i];
				min_dir = i;
			}
		}
	}

	//周囲にプレイヤーがいる場合
	if (CheckIsTherePlayer(GetCharaDir(), ene_now_pos)) {
		//ResetDistination();
		tnl::DebugTrace("\n攻撃\n");
		//敵の攻撃選択
		//EnemyAttack();
		return;
	}
	//プレイヤー以外の目的地の場合
	//目的地に到達したときに何かする
	else if (ene_now_pos.x == destination_pos.x && ene_now_pos.y == destination_pos.y) {
		//目的地をランダムに変更する
		tnl::DebugTrace("\n目的地に到達\n");
		ResetDistination();

		//現在とは別の部屋に目的地を設定して移動するのがよさげ
		 
		//プレイヤーと敵が同じ部屋ではない場合
		if (game_manager->GetObjectManager()->factory->GetPlayer()->GetNowRoomValue() != ene_now_in_room) {
			ResetDistination();
			return;
		}
		return;
	}
	
	//プレイヤーが発見維持範囲内かチェック
	ResetPlayerDiscovery();

	//プレイヤー以外の目的地に着いたときにブルブルするのを解決する
	//最小コストの方向に進む
	if (min_dir != -1) {
		tnl::Vector3 pos = GetCharaPos();
		pos += vec[min_dir] * 1;
		SetCharaPos(pos);
		SetDir(min_dir);
		return;
	}

	

}

//------------------------------------------------------------------------------------------------------------
//自分の体の向きに進む関数
bool Enemy::MoveToDir(const MoveDir dir, tnl::Vector3 pos)
{
	if (!CheckCanMove(dir, pos))return false;

	tnl::Vector3 pos_ = GetCharaPos();
	pos_ += vec[(int)dir] * 1;
	SetCharaPos(pos_);

	//それぞれの方向の単位ベクトルを足し合わせている
	GetCharaPos() += vec[(int)dir] * 1;

	return true;
}

//------------------------------------------------------------------------------------------------------------
//自分のひとます先が移動可能な場所かを調べる
bool Enemy::CheckCanMove(const MoveDir dir, const tnl::Vector3 pos)
{

	tnl::Vector3 next_pos(0);

	//それぞれの方向の単位ベクトルを足し合わせている
	next_pos = pos + vec[(int)dir] * 1;

	//壁か敵ならfalseを返す(プレイヤーの場合の条件も付け足す必要あるかも？)
	if (game_manager->GetMapChip(next_pos) == MapChip::MapType::WALL
		|| game_manager->CheckIsThereEnemy(next_pos)) {
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
//自分の現在向いている方向から特定の方向へ行けるかどうかをチェックする関数
//arg1…現在向いている方向
//arg3…自分が現在向いている方向から行けるか調べたい方向
bool Enemy::CheckCanMoveToDir(const MoveDir dir, const tnl::Vector3 pos, const MoveDir check_dir)
{
	switch (dir)
	{
	case MoveDir::DOWN:
		if (check_dir == MoveDir::LEFT) return CheckCanMove(MoveDir::DOWN,	pos);
		if (check_dir == MoveDir::RIGHT)return CheckCanMove(MoveDir::LEFT, pos);
		break;
	case MoveDir::LEFT:
		if (check_dir == MoveDir::LEFT) return CheckCanMove(MoveDir::DOWN, pos);
		if (check_dir == MoveDir::RIGHT)return CheckCanMove(MoveDir::UP, pos);
		break;
	case MoveDir::RIGHT:
		if (check_dir == MoveDir::LEFT) return CheckCanMove(MoveDir::UP, pos);
		if (check_dir == MoveDir::RIGHT)return CheckCanMove(MoveDir::DOWN, pos);
		break;
	case MoveDir::UP:
		if (check_dir == MoveDir::DOWN) return CheckCanMove(MoveDir::DOWN, pos);
		if (check_dir == MoveDir::LEFT) return CheckCanMove(MoveDir::LEFT, pos);
		if (check_dir == MoveDir::RIGHT)return CheckCanMove(MoveDir::RIGHT, pos);
		if (check_dir == MoveDir::UP)	return CheckCanMove(MoveDir::UP, pos);
		break;

	default:
		tnl::DebugTrace("\n条件外エラー\n");
		break;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//EnemyAttackで関数でランダム、その時の状態によって発動するスキルを変更する
std::shared_ptr<EffectManager> Enemy::EnemyAttack(Skill* last_skill, std::list<std::shared_ptr<EffectManager>> draw_effect_list, std::shared_ptr<Enemy>enemy_)
{
	
	//ランダム数値を取得(スキルのインデックス番号)
	int rand_ene_attack_index = game_manager->GetRandValue(0, 1);
	
	last_skill = GetSkillList()[rand_ene_attack_index];

	//アニメーション描画座標決定
	tnl::Vector3 ef_pos = GetCharaPos();
	SetAnimationPos(ef_pos);
	int ene_eff_index = ene_eff_index = last_skill->GetGraphicsAllNum();

	tnl::Vector3 ene_eff_pos = GetEffectPos();
			
	tnl::DebugTrace("\n敵のエフェクト追加\n");

	return std::make_shared<EffectManager>(last_skill->GetGraphicsVector(), ene_eff_pos, last_skill->GetAnimSpeed(), ene_eff_index);

}

//------------------------------------------------------------------------------------------------------------
//EnemyAnimationでEnemyAttacで決まったスキルを受け取り、そのスキルのAnimationを戻り値で返せるようにする
std::shared_ptr<EffectManager> Enemy::EnemyEffect(std::vector<int> skill_list, tnl::Vector3 pos, int anim_speed, int index)
{
	return std::make_shared<EffectManager>(skill_list, pos, anim_speed, index);
}

//------------------------------------------------------------------------------------------------------------
//周囲にプレイヤーがいるかどうかをチェックする
bool Enemy::CheckIsTherePlayer(MoveDir dir, tnl::Vector3 pos)
{
	//プレイヤーの座標を一時保存
	//tnl::Vector3 pl_pos = ptr->map_manager->factory->player->chara_pos;
	//引数で渡された敵の現在の向きと座標を受け取る
	//敵の現在座標と方向の単位ベクトルを足し合わせて保存
	tnl::Vector3 next_pos = (pos + vec[(int)dir]) * 1;

	//pl_posとnext_posが一致しているならtrueを返す
	if (destination_pos.x == next_pos.x && destination_pos.y == next_pos.y) {
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//
Character::MoveDir Enemy::GetDir(const MoveDir dir, const MoveDir getdir)
{
	switch (dir)
	{
		//現在の向きが上
	case MoveDir::UP:
		//左は左側
		if (getdir == MoveDir::LEFT)return MoveDir::LEFT;
		//右は右側
		else if (getdir == MoveDir::RIGHT)return MoveDir::RIGHT;
		else if (getdir == MoveDir::DOWN)return MoveDir::DOWN;
		break;
		//現在の向きが右
	case MoveDir::RIGHT:
		//左は上
		if (getdir == MoveDir::LEFT)return MoveDir::UP;
		//右は下側
		else if (getdir == MoveDir::RIGHT)return MoveDir::DOWN;
		else if (getdir == MoveDir::DOWN)return MoveDir::LEFT;
		break;
		//現在の向きが左
	case MoveDir::LEFT:
		//左は下側
		if (getdir == MoveDir::LEFT)return MoveDir::DOWN;
		//右は右側
		else if (getdir == MoveDir::RIGHT)return MoveDir::UP;
		else if (getdir == MoveDir::DOWN)return MoveDir::RIGHT;
		break;
		//現在の向きが下
	case MoveDir::DOWN:
		//左は左側
		if (getdir == MoveDir::LEFT)return MoveDir::RIGHT;
		//右は右側
		else if (getdir == MoveDir::RIGHT)return MoveDir::LEFT;
		else if (getdir == MoveDir::DOWN)return MoveDir::UP;
		break;
	default:
		break;
	}
	return MoveDir(-1);
}

//------------------------------------------------------------------------------------------------------------
//目的地をリセットする
void Enemy::ResetDistination()
{
	other_discovery = false;
}

//------------------------------------------------------------------------------------------------------------
//プレイヤーとの距離が離れすぎたら発見フラグを下す関数
void Enemy::ResetPlayerDiscovery()
{
	tnl::Vector3 pos = game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
	tnl::Vector3 pl_local_pos = game_manager->WorldToLocalPos(pos);
	
	float x = abs(pl_local_pos.x - ene_now_pos.x);
	float y = abs(pl_local_pos.y - ene_now_pos.y);
	
	//発見維持範囲外になったらfalseに変更
	if (x + y > DISCOVERYRANGE)pl_discovery = false;
}

//------------------------------------------------------------------------------------------------------------
//dir方向にただ進む関数
//<注意事項>
//この関数の使用時は移動可能なことを保証すること
//この関数自身は移動可能判定を行っていない
//
void Enemy::SolyMoveToDir(const MoveDir dir)
{
	tnl::Vector3 pos = GetCharaPos();
	pos += vec[(int)dir] * 1;
	SetCharaPos(pos);
	SetDir((int)dir);

}



