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
//�R���X�g���N�^
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
	
	//�ő�HP���X�V
	SetPlayerMaxHp(hp);

}

//------------------------------------------------------------------------------------------------------------
Player::~Player() 
{
	tnl::DebugTrace("\n===================Player�f�X�g���N�^����===================\n");
}

//------------------------------------------------------------------------------------------------------------
//���t���[�����s
void Player::Update(float delta_time) {

	//�f�o�b�OHp�ϓ��֐�
	DebugHpFluctuation();
	Move();
}

//------------------------------------------------------------------------------------------------------------
//�L�����N�^�[��`����W����}�b�v���W�ɕϊ�
void Player::SetPlayerLocalPos()
{
	SetCharaInPos(game_manager->WorldToLocalPos(GetCharaInPos()));
}

//------------------------------------------------------------------------------------------------------------
//WASD�L�[�ɂ���Ă��ꂼ��̕����Ɉړ�����֐�
void Player::Move()
{
	tnl::Vector3 pos = GetCharaPos();
	tnl::Vector3 pl_now_pos = game_manager->WorldToLocalPos(pos);

	if (tnl::Input::IsKeyDown(eKeys::KB_W)) {
		SetDir((int)MoveDir::UP);
		//�ړ��֐�
		SetMoveCharaPos(MoveDir::UP);
		if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::STARTMAP) {
			//�ړ��֐�
			SetMoveCharaPos(MoveDir::UP);

		}

		
	}
	else if (tnl::Input::IsKeyDown(eKeys::KB_S)) {
		SetDir((int)MoveDir::DOWN);
		//�ړ��֐�
		SetMoveCharaPos(MoveDir::DOWN);
		if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::STARTMAP) {
			//�ړ��֐�
			SetMoveCharaPos(MoveDir::DOWN);
			
		}
	}
	else if (tnl::Input::IsKeyDown(eKeys::KB_A)) {
		SetDir((int)MoveDir::LEFT);
		//�ړ��֐�
		SetMoveCharaPos(MoveDir::LEFT);
		if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::STARTMAP) {
			//�ړ��֐�
			SetMoveCharaPos(MoveDir::LEFT);

		}
	}
	else if (tnl::Input::IsKeyDown(eKeys::KB_D)) {
		SetDir((int)MoveDir::RIGHT);
		//�ړ��֐�
		SetMoveCharaPos(MoveDir::RIGHT);
		if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::STARTMAP) {
			//�ړ��֐�
			SetMoveCharaPos(MoveDir::RIGHT);

		}
	}

	if (game_manager->GetObjectManager()->factory->GetPlayerSpawn() == Factory::PlayerSpawn::DUNGEON) {
		//�ړ���̕����ɕύX����
		game_manager->SetPlayerRoomValue(game_manager->CheckIsThere(pl_now_pos));

	}
	
}


//------------------------------------------------------------------------------------------------------------
//�A�C�e���g�p���Ƀv���C���[�̃X�e�[�^�X��ϓ�������
//stetustype:0->HP,stetustype:1->ATK,stetustype:2->DEF,stetustype:3->GOLD,stetustype:4->DIAMOND
void Player::EffectToHp(Stetus stetustype, int stetus)
{
	switch (stetustype)
	{
	case Character::Stetus::HP:
		//Hp�����Z����
		SetCharaStetus(Character::Stetus::HP, stetus);
		break;
	case Character::Stetus::ATK:
		//Attack���㏸����
		SetCharaStetus(Character::Stetus::ATK, stetus);//(WIP)
		break;
	case Character::Stetus::DEF:
		//Defence���㏸����
		SetCharaStetus(Character::Stetus::DEF, stetus);
		break;
	case Character::Stetus::GOLD:
		//�����������Z����
		SetCharaStetus(Character::Stetus::GOLD, stetus);
		break;
	case Character::Stetus::DIAMOND:
		//�����_�C�A�����h�𑝂₷
		SetCharaStetus(Character::Stetus::DIAMOND, stetus);
		break;
	default:
		tnl::DebugTrace("\n�X�e�[�^�X��O\n");
		break;
	}
}

//------------------------------------------------------------------------------------------------------------
//�v���C���[�̌o���l�����Z����֐�
//�o���l���ő�ɂȂ�΃��x�����オ��
bool Player::AddExp(int add_exp)
{
	//�v���C���[�̌��ݎ擾�o���l���X�V
	SetCharaStetus(Character::Stetus::EXP, add_exp);

	//�v���C���[�̌��݂̌o���l���擾
	int get_pl_exp = GetCharaStetus(Character::Stetus::EXP);
	//�擾�����o���l�����Z
	get_pl_exp += add_exp;

	//���x���A�b�v�܂ł̕K�v�o���l���v�Z
	leftover_exp = (need_level_up_exp - get_pl_exp);
	//�K�v�o���l���X�V
	SetLeftOverExp(leftover_exp);

	//���݂̌o���l���K�v�o���l�ȏ�Ȃ烌�x����������
	if (get_pl_exp >= need_level_up_exp) {
		
		//�K�v�o���l�ȏ�Ȃ�
		if (get_pl_exp > need_level_up_exp) {
			//�ߏ�擾�o���l���v�Z
			over_need_exp = (need_level_up_exp - get_pl_exp);
		}
		
		//���x����1�グ��
		SetAffectToCharaStetus(Character::Stetus::LEVEL, +1);

		//���̃��x���ɕK�v�Ȍo���l���v�Z����
		NeedUpExp();

		//�X�e�[�^�X�������_���ɋ�������
		ReinforceStetus();

		//����HP���擾
		int now_hp = GetCharaStetus(Character::Stetus::HP);
		//�ő�HP��茻��HP�̂ق����������
		if (now_hp > max_hp) {
			//�ő�HP���X�V
			SetPlayerMaxHp(now_hp);
		}
		

		tnl::DebugTrace("\n============���x�����オ����=============\n");
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------
//�K�v�o���l�𑝉�������֐�
void Player::NeedUpExp()
{
	//���݂̃v���C���[�̃��x�����擾
	int now_level = GetCharaStetus(Character::Stetus::LEVEL);
	
	//�K�v�o���l = (���݂̃��x�� * 1.5) + 50
	//���x�����オ�邲�Ƃɐ��`�ɕK�v�o���l���オ���Ă���
	need_level_up_exp = (now_level * EXPINCREASERATE) + BASENEEDEXP;

	//���̃��x���܂ł̎c��o���l���X�V
	SetLeftOverExp(need_level_up_exp);
	tnl::DebugTrace("\n============�K�v�o���l�v�Z�I��(NeedUpExp)=============\n");

}

//------------------------------------------------------------------------------------------------------------
void Player::DebugHpFluctuation()
{
	if (tnl::Input::IsKeyDown(eKeys::KB_P)) {
		SetAffectToCharaStetus(Character::Stetus::HP, -5);
	}
	if (tnl::Input::IsKeyDown(eKeys::KB_O)) {
		SetAffectToCharaStetus(Character::Stetus::HP, +5);
		//����HP���擾
		int hp = GetCharaStetus(Character::Stetus::HP);
		
		//�ő�HP��茻��HP�̂ق����������
		if (max_hp < hp) {
			//�ő�HP�X�V
			SetPlayerMaxHp(GetCharaStetus(Character::Stetus::HP));
		}
		
	}
}
