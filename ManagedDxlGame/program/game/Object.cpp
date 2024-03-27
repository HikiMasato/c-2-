//-------------------------------------------------------
//�����̂���
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
//���t���[�����s
void Object::Update(float delta_time)
{
}

//------------------------------------------------------------------------------------------------------------
//�`��
void Object::Draw(const hm::Camera& camera)
{
}

//------------------------------------------------------------------------------------------------------------
//�w���Dir�ɂ���ĉ�������|�W�V������ύX����(�L�����N�^�[�̈ړ��Ɏg�p)
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
		tnl::DebugTrace("\n�w��͈͊O�w��(SetMoveCharaPos)\n");
		break;
	}


}





