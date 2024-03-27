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
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Debug.h"



//------------------------------------------------------------------------------------------------------------
//毎フレーム実行
void Object::Update(float delta_time)
{
}

//------------------------------------------------------------------------------------------------------------
//描画
void Object::Draw(const hm::Camera& camera)
{
}

//------------------------------------------------------------------------------------------------------------
//指定のDirによって加減するポジションを変更する(キャラクターの移動に使用)
void Object::SetMoveCharaPos(Object::MoveDir dir)
{
	switch (dir)
	{
	case Object::MoveDir::DOWN:
		chara_pos.y += 2;
		SetCharaPos(chara_pos);
		SceneTitle::game_manager->CameraReset();


		break;
	case Object::MoveDir::LEFT:
		chara_pos.x -= 2;
		SetCharaPos(chara_pos);
		SceneTitle::game_manager->CameraReset();

		break;
	case Object::MoveDir::RIGHT:
		chara_pos.x += 2;
		SetCharaPos(chara_pos);
		SceneTitle::game_manager->CameraReset();

		break;
	case Object::MoveDir::UP:
		chara_pos.y -= 2;
		SetCharaPos(chara_pos);
		SceneTitle::game_manager->CameraReset();

		break;
	default:
		tnl::DebugTrace("\n指定範囲外指定(SetMoveCharaPos)\n");
		break;
	}


}





