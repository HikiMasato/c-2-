//-------------------------------------------------------
//既存のもの
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "GameManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "EffectManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "Character.h"
#include "MyCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Debug.h"



//------------------------------------------------------------------------------------------------------------
void Character::Update(float delta_time)
{
}

//------------------------------------------------------------------------------------------------------------
void Character::Draw(const hm::Camera& camera)
{
	Animation(chra_chip_array, obj_speed, maxindex, drawph);

	tnl::Vector3 now_pos = GetCharaPos();
	tnl::Vector3 new_pos = { now_pos.x - camera.cameraPos.x, now_pos.y - 10 - camera.cameraPos.y, 0 };

	//敵のhpバーの描画
	tnl::Vector3 hp_pos = { now_pos.x - camera.cameraPos.x, now_pos.y - 20 - camera.cameraPos.y,0 };
	
	int draw_hp = GetCharaStetus(Stetus::HP);



	//キャラチップを描画
	//描画座標を調整
	DrawRotaGraph(new_pos.x, new_pos.y, 1.5f, 0, drawph, true);
	
	//hpberの描画
	DrawHpbarCharactor(draw_hp, hp_pos);

}

//------------------------------------------------------------------------------------------------------------
//キャラクターアニメーションを実行
void Character::Animation(int* graph, int speed, int maxindex, int& drawph)
{
	//(優先度 : 中)
	//ここは可変長数値で計算するべき
	//(実行するデバイスによって計算速度が変化するので、アニメーション再生時間が変わってくる)
	if (--obj_wait <= 0) {
		chra_chip_index++;
		obj_wait = speed;
		chra_chip_index %= maxindex;
	}
	drawph = graph[chra_chip_index + 3 * (int)my_dir];

}

//------------------------------------------------------------------------------------------------------------
//キャラクターのステータス強化値
//階層移動関数内でのみ呼び出す
//キャラレベルアップでも呼び出す
void Character::ReinforceStetus()
{
	//ステータスごとにランダムに強化される
	//ベースの変数にランダムに加算されていく
	//そこに、csvファイルに書き込んである基礎値を足していくため階層を移動するごとに強くなっていく仕組み
	chara_hp += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN, RAINFORCE_MAX);
	chara_mp += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN, RAINFORCE_MAX);
	chara_attack += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN, RAINFORCE_MAX);
	chara_defence += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN, RAINFORCE_MAX);

	//ステータス上昇するキャラが敵の場合
	if (GetObjectType() == Object::ObjectType::ENEMY) {
		chara_hp += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN + 2, RAINFORCE_MAX + 9);
		chara_mp += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN + 10, RAINFORCE_MAX + 10);
		chara_attack += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN + 3, RAINFORCE_MAX + 6);
		chara_defence += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN + 5, RAINFORCE_MAX + 5);
	}

}

//------------------------------------------------------------------------------------------------------------
//アニメーション描画座標
void Character::SetAnimationPos(tnl::Vector3 pos)
{
	//この関数を呼び出したキャラの現在座標の取得
	tnl::Vector3 now_pos = GetCharaPos();
	//effect座標を算出(現在座標 + 現在の方向の1つ先の座標)
	tnl::Vector3 set_pos = SceneTitle::game_manager->WorldToLocalPos(now_pos) + SceneTitle::game_manager->GetFirstDirVector((int)my_dir);
	//エフェクト再生座標
	effect_pos = SceneTitle::game_manager->LocalToWorldPos(set_pos.x, set_pos.y);
	//セット
	SetEffectPos(effect_pos);
}

//------------------------------------------------------------------------------------------------------------
//引数で指定したステータスを返す関数
int Character::GetCharaStetus(Stetus stetus) const
{
	switch (stetus)
	{
	case Stetus::HP:
		return chara_hp;
		break;
	case Stetus::ATK:
		return chara_attack;
		break;
	case Stetus::DEF:
		return chara_defence;
		break;
	case Stetus::GOLD:
		return have_gold;
		break;
	case Stetus::DIAMOND:
		return have_diamond;
		break;
	case Stetus::EXP:
		return chara_exp;
		break;
	case Stetus::LEVEL:
		return chara_level;
		break;
	default:
		tnl::DebugTrace("\nStetusエラー\n");
		break;
	}
}

//------------------------------------------------------------------------------------------------------------
//キャラステータスをセットする関数
void Character::SetCharaStetus(Stetus stetus, int set_value)
{
	switch (stetus)
	{
	case Stetus::HP:
		chara_hp =  set_value;
		break;
	case Stetus::MP:
		chara_mp = set_value;
		break;
	case Stetus::ATK:
		chara_attack = set_value;
		break;
	case Stetus::DEF:
		chara_defence = set_value;
		break;
	case Stetus::GOLD:
		have_gold = (have_gold += set_value);
		break;
	case Stetus::DIAMOND:
		have_diamond = (have_diamond += set_value);
		break;
	case Stetus::EXP:
		chara_exp = set_value;;
		break;
	case Stetus::LEVEL:
		chara_level = set_value;
		break;
	default:
		tnl::DebugTrace("\nStetusエラー(SetCharaStetus)\n");
		break;
	}
}

//------------------------------------------------------------------------------------------------------------
//キャラのステータスを変動させる関数
//ダメージ、アイテム使用時などに使用
void Character::SetAffectToCharaStetus(Stetus stetus, int affect_value)
{
	switch (stetus)
	{
	case Stetus::HP:
		chara_hp = (chara_hp + affect_value);
		break;
	case Stetus::MP:
		chara_mp = (chara_mp + affect_value);
		break;
	case Stetus::ATK:
		chara_attack = (chara_attack + affect_value);
		break;
	case Stetus::DEF:
		chara_defence = (chara_defence + affect_value);
		break;
	case Stetus::GOLD:
		have_gold = (have_gold += affect_value);
		break;
	case Stetus::DIAMOND:
		have_diamond = (have_diamond += affect_value);
		break;
	case Stetus::EXP:
		chara_exp = (chara_exp += affect_value);;
		break;
	case Stetus::LEVEL:
		chara_level = (chara_level += affect_value);
		break;
	default:
		tnl::DebugTrace("\nStetusエラー(SetAffectToCharaStetus)\n");
		break;
	}
}


//------------------------------------------------------------------------------------------------------------
//キャラ攻撃関数(WIP)
void Character::Attack()
{
	tnl::Vector3 get_chara_pos = GetCharaPos();

	//目の前のActorのポジション
	tnl::Vector3 front;
	front = SceneTitle::game_manager->WorldToLocalPos(get_chara_pos) + SceneTitle::game_manager->GetFirstDirVector((int)my_dir);

	//目の前の対象を取得
	//ダメージ計算処理
	SceneTitle::game_manager->InflictDamageToTarget(this, front);


}

//------------------------------------------------------------------------------------------------------------
//キャラスキル攻撃関数
void Character::SkillAttack(Skill* used_skill)
{
	tnl::Vector3 get_chara_pos = GetCharaPos();

	tnl::Vector3 front;

	front = SceneTitle::game_manager->WorldToLocalPos(get_chara_pos) + SceneTitle::game_manager->GetFirstDirVector((int)my_dir);

	SceneTitle::game_manager->CalculationSkillDamageToTarget(this, front, used_skill);

}

//------------------------------------------------------------------------------------------------------------
//キャラクターのHpbarの描画
void Character::DrawHpbarCharactor(int hp, const tnl::Vector3& pos) {

	static float width = 0;

	//widthはHPバーの最大幅を表す
	width = hp;
	if (width <= 0)width = 0;
	int fs = GetFontSize();

	SetFontSize(10);
	int x = (int)pos.x;
	int y = (int)pos.y;
	DrawBox(x, y, x + width + 1, y + 10, 0, true);
	DrawBox(x + 1, y + 1, x + width + 1, y + 9, -1, false);
	DrawBox(x + 2, y + 2, x + width, y + 8, 0xff77ff77, true);
	SetFontSize(fs);
}
