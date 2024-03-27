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
//�R���X�g���N�^
Enemy::Enemy(tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, GameManager* game_manager_)
{
	game_manager = game_manager_;
	SetCharaPos(start_pos);

	//��b�l + �����l
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
//�f�X�g���N�^
Enemy::~Enemy()
{
	tnl::DebugTrace("\n===================Enemy�f�X�g���N�^����===================\n");
}

//------------------------------------------------------------------------------------------------------------
//���t���[�����s
void Enemy::Update(float delta_time)
{
	EnemyMove();
}

//------------------------------------------------------------------------------------------------------------
//�G�s���֐�
bool Enemy::EnemyMove()
{
	tnl::Vector3 pos = GetCharaPos();

	//�G�̈ʒu���}�b�v��̂ǂ��������
	ene_now_pos = game_manager->WorldToLocalPos(pos);

	//�G�������̂ǂ����ɂ���Ȃ畔���ԍ����擾
	ene_now_in_room = game_manager->GetMapManager()->CheckIsThere(ene_now_pos.x, ene_now_pos.y);

	//�G�������O�ł͂Ȃ��A���v���C���[�ƕ����ԍ��������ł���Ȃ�
	if (game_manager->GetObjectManager()->factory->GetPlayer()->GetNowRoomValue() == ene_now_in_room && ene_now_in_room != -1) {
		same_room = true;
	}
	//��L�̏����Ɉ�v���Ȃ����
	else {
		same_room = false;
	}

	//�v���C���[�𔭌����Ă�����
	if (pl_discovery) {
		tnl::Vector3 player_pos = game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
		//�ړI�n�̍��W�Ƀv���C���[�̍��W��ݒ�
		destination_pos = game_manager->WorldToLocalPos(player_pos);
		//�o�H�T��
		MoveDestination();

		return true;
	}
	//�ړI�n���Z�b�g����Ă����
	if (other_discovery && !same_room) {
		//�o�H�T��
		MoveDestination();
		return true;
	}
	//�v���C���[�Ɠ��������̏ꍇ�͖ړI�n�̓v���C���[�̂͂�
	else if (other_discovery && same_room) {
		tnl::Vector3 player_pos = game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();

		//�ړI�n�̍��W�Ƀv���C���[�̍��W��ݒ�
		destination_pos = game_manager->WorldToLocalPos(player_pos);
		other_discovery = true;
		pl_discovery = true;
		//�o�H�T��
		MoveDestination();

	}
	//�����ɂ���ꍇ
	if (ene_now_in_room != -1) {
		//���̕����̏o���̒������ԉ����o�����擾����
		tnl::Vector3 way_point = game_manager->GetFarPoint(ene_now_in_room, ene_now_pos);
		//�ړI�n�̃��Z�b�g
		destination_pos = (0);
		//�擾�����o����ړI�n�ɂ���
		destination_pos = way_point;
		//�v���C���[����
		pl_discovery = true;
		//�o�H�T��
		MoveDestination();
		return true;
	}
	//�ʘH�ɂ���ꍇ
	else {
		//���g�̕����ɐi�߂邩�ǂ����̃t���O
		bool can_move = MoveToDir(GetCharaDir(), ene_now_pos);
		//���bool�܂ł͒ʂ�̊m�F�ł���
		//�ʘH�ɐN������ƓG�������Ȃ��Ȃ�
		//�ʘH�i�����ɂ����Ɛi�ނ悤�ɂ���
		if (!can_move) {
			//�������E�ɐi�߂�Ȃ�
			if (CheckCanMoveToDir(GetCharaDir(), ene_now_pos, MoveDir::LEFT) && CheckCanMoveToDir(GetCharaDir(), ene_now_pos, MoveDir::RIGHT)) {

				if (rand() % 2 == 0)SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::LEFT));
				else SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::RIGHT));
			}
			//���ɐi�߂邩�m�F
			else if (CheckCanMoveToDir(GetCharaDir(), ene_now_pos, MoveDir::LEFT))SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::LEFT));
			//���������Ȃ���ΉE���m�F
			else if (CheckCanMoveToDir(GetCharaDir(), ene_now_pos, MoveDir::RIGHT))SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::RIGHT));
			//���E�������Ȃ����
			else {
				SolyMoveToDir(GetDir(GetCharaDir(), MoveDir::DOWN));
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------
//�ړI�n�Ɍ������֐�(�o�H�T���֐�)
void Enemy::MoveDestination()
{

	//�_�C�N�X�g���@
	//A*�@(A�X�^�[�@)
	//�q���[���X�e�B�b�N�֐�
	   
	//�G������Ƃ��ɂ��̃m�[�h��ʉ߂ł��Ȃ��悤�ɂ���
	//game_manager->GetMapChip(myNowPos + v[i]) == 0 �Ƃ���������ǉ�����
	for (int i = 0; i < (int)MoveDir::DIRMAX; i++) {
		//�G�̌��ݍ��W���炷�ׂĂ̕���(4����)�̃}�b�v�^�C�v�𒲂ׂĕǂł͂Ȃ��A�����ׂĂ̕���(4����)�ɓG�����Ȃ����true������
		is_not_wall[i] = game_manager->GetMapChip(ene_now_pos + vec[i]) != MapChip::MapType::WALL
			&& !game_manager->CheckIsThereEnemy(ene_now_pos + vec[i]);
	}

	//�R�X�g���ŏ��ɂȂ�悤�ɐi��
	//�q���[���X�e�B�b�N�֐��̒l���R�X�g�ɉ��Z����
	//�ŏ��R�X�g�����߂邽�߂ɁA�R�X�g�̔z���p�ӂ���
	for (int i = 0; i < (int)MoveDir::DIRMAX; i++) cost[i] = FLT_MAX;

	int min_dir = -1;

	float min_cost = FLT_MAX;
	for (int i = 0; i < (int)MoveDir::DIRMAX; i++) {
		if (is_not_wall[i]) {
			//�R�X�g���v�Z����
			cost[i] = game_manager->GetMapChip(ene_now_pos + vec[i]) + Heuristic(ene_now_pos + vec[i], destination_pos);
			//�ŏ��R�X�g���X�V����
			if (cost[i] < min_cost) {
				min_cost = cost[i];
				min_dir = i;
			}
		}
	}

	//���͂Ƀv���C���[������ꍇ
	if (CheckIsTherePlayer(GetCharaDir(), ene_now_pos)) {
		//ResetDistination();
		tnl::DebugTrace("\n�U��\n");
		//�G�̍U���I��
		//EnemyAttack();
		return;
	}
	//�v���C���[�ȊO�̖ړI�n�̏ꍇ
	//�ړI�n�ɓ��B�����Ƃ��ɉ�������
	else if (ene_now_pos.x == destination_pos.x && ene_now_pos.y == destination_pos.y) {
		//�ړI�n�������_���ɕύX����
		tnl::DebugTrace("\n�ړI�n�ɓ��B\n");
		ResetDistination();

		//���݂Ƃ͕ʂ̕����ɖړI�n��ݒ肵�Ĉړ�����̂��悳��
		 
		//�v���C���[�ƓG�����������ł͂Ȃ��ꍇ
		if (game_manager->GetObjectManager()->factory->GetPlayer()->GetNowRoomValue() != ene_now_in_room) {
			ResetDistination();
			return;
		}
		return;
	}
	
	//�v���C���[�������ێ��͈͓����`�F�b�N
	ResetPlayerDiscovery();

	//�v���C���[�ȊO�̖ړI�n�ɒ������Ƃ��Ƀu���u������̂���������
	//�ŏ��R�X�g�̕����ɐi��
	if (min_dir != -1) {
		tnl::Vector3 pos = GetCharaPos();
		pos += vec[min_dir] * 1;
		SetCharaPos(pos);
		SetDir(min_dir);
		return;
	}

	

}

//------------------------------------------------------------------------------------------------------------
//�����̑̂̌����ɐi�ފ֐�
bool Enemy::MoveToDir(const MoveDir dir, tnl::Vector3 pos)
{
	if (!CheckCanMove(dir, pos))return false;

	tnl::Vector3 pos_ = GetCharaPos();
	pos_ += vec[(int)dir] * 1;
	SetCharaPos(pos_);

	//���ꂼ��̕����̒P�ʃx�N�g���𑫂����킹�Ă���
	GetCharaPos() += vec[(int)dir] * 1;

	return true;
}

//------------------------------------------------------------------------------------------------------------
//�����̂ЂƂ܂��悪�ړ��\�ȏꏊ���𒲂ׂ�
bool Enemy::CheckCanMove(const MoveDir dir, const tnl::Vector3 pos)
{

	tnl::Vector3 next_pos(0);

	//���ꂼ��̕����̒P�ʃx�N�g���𑫂����킹�Ă���
	next_pos = pos + vec[(int)dir] * 1;

	//�ǂ��G�Ȃ�false��Ԃ�(�v���C���[�̏ꍇ�̏������t�������K�v���邩���H)
	if (game_manager->GetMapChip(next_pos) == MapChip::MapType::WALL
		|| game_manager->CheckIsThereEnemy(next_pos)) {
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
//�����̌��݌����Ă�������������̕����֍s���邩�ǂ������`�F�b�N����֐�
//arg1�c���݌����Ă������
//arg3�c���������݌����Ă����������s���邩���ׂ�������
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
		tnl::DebugTrace("\n�����O�G���[\n");
		break;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//EnemyAttack�Ŋ֐��Ń����_���A���̎��̏�Ԃɂ���Ĕ�������X�L����ύX����
std::shared_ptr<EffectManager> Enemy::EnemyAttack(Skill* last_skill, std::list<std::shared_ptr<EffectManager>> draw_effect_list, std::shared_ptr<Enemy>enemy_)
{
	
	//�����_�����l���擾(�X�L���̃C���f�b�N�X�ԍ�)
	int rand_ene_attack_index = game_manager->GetRandValue(0, 1);
	
	last_skill = GetSkillList()[rand_ene_attack_index];

	//�A�j���[�V�����`����W����
	tnl::Vector3 ef_pos = GetCharaPos();
	SetAnimationPos(ef_pos);
	int ene_eff_index = ene_eff_index = last_skill->GetGraphicsAllNum();

	tnl::Vector3 ene_eff_pos = GetEffectPos();
			
	tnl::DebugTrace("\n�G�̃G�t�F�N�g�ǉ�\n");

	return std::make_shared<EffectManager>(last_skill->GetGraphicsVector(), ene_eff_pos, last_skill->GetAnimSpeed(), ene_eff_index);

}

//------------------------------------------------------------------------------------------------------------
//EnemyAnimation��EnemyAttac�Ō��܂����X�L�����󂯎��A���̃X�L����Animation��߂�l�ŕԂ���悤�ɂ���
std::shared_ptr<EffectManager> Enemy::EnemyEffect(std::vector<int> skill_list, tnl::Vector3 pos, int anim_speed, int index)
{
	return std::make_shared<EffectManager>(skill_list, pos, anim_speed, index);
}

//------------------------------------------------------------------------------------------------------------
//���͂Ƀv���C���[�����邩�ǂ������`�F�b�N����
bool Enemy::CheckIsTherePlayer(MoveDir dir, tnl::Vector3 pos)
{
	//�v���C���[�̍��W���ꎞ�ۑ�
	//tnl::Vector3 pl_pos = ptr->map_manager->factory->player->chara_pos;
	//�����œn���ꂽ�G�̌��݂̌����ƍ��W���󂯎��
	//�G�̌��ݍ��W�ƕ����̒P�ʃx�N�g���𑫂����킹�ĕۑ�
	tnl::Vector3 next_pos = (pos + vec[(int)dir]) * 1;

	//pl_pos��next_pos����v���Ă���Ȃ�true��Ԃ�
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
		//���݂̌�������
	case MoveDir::UP:
		//���͍���
		if (getdir == MoveDir::LEFT)return MoveDir::LEFT;
		//�E�͉E��
		else if (getdir == MoveDir::RIGHT)return MoveDir::RIGHT;
		else if (getdir == MoveDir::DOWN)return MoveDir::DOWN;
		break;
		//���݂̌������E
	case MoveDir::RIGHT:
		//���͏�
		if (getdir == MoveDir::LEFT)return MoveDir::UP;
		//�E�͉���
		else if (getdir == MoveDir::RIGHT)return MoveDir::DOWN;
		else if (getdir == MoveDir::DOWN)return MoveDir::LEFT;
		break;
		//���݂̌�������
	case MoveDir::LEFT:
		//���͉���
		if (getdir == MoveDir::LEFT)return MoveDir::DOWN;
		//�E�͉E��
		else if (getdir == MoveDir::RIGHT)return MoveDir::UP;
		else if (getdir == MoveDir::DOWN)return MoveDir::RIGHT;
		break;
		//���݂̌�������
	case MoveDir::DOWN:
		//���͍���
		if (getdir == MoveDir::LEFT)return MoveDir::RIGHT;
		//�E�͉E��
		else if (getdir == MoveDir::RIGHT)return MoveDir::LEFT;
		else if (getdir == MoveDir::DOWN)return MoveDir::UP;
		break;
	default:
		break;
	}
	return MoveDir(-1);
}

//------------------------------------------------------------------------------------------------------------
//�ړI�n�����Z�b�g����
void Enemy::ResetDistination()
{
	other_discovery = false;
}

//------------------------------------------------------------------------------------------------------------
//�v���C���[�Ƃ̋��������ꂷ�����甭���t���O�������֐�
void Enemy::ResetPlayerDiscovery()
{
	tnl::Vector3 pos = game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
	tnl::Vector3 pl_local_pos = game_manager->WorldToLocalPos(pos);
	
	float x = abs(pl_local_pos.x - ene_now_pos.x);
	float y = abs(pl_local_pos.y - ene_now_pos.y);
	
	//�����ێ��͈͊O�ɂȂ�����false�ɕύX
	if (x + y > DISCOVERYRANGE)pl_discovery = false;
}

//------------------------------------------------------------------------------------------------------------
//dir�����ɂ����i�ފ֐�
//<���ӎ���>
//���̊֐��̎g�p���͈ړ��\�Ȃ��Ƃ�ۏ؂��邱��
//���̊֐����g�͈ړ��\������s���Ă��Ȃ�
//
void Enemy::SolyMoveToDir(const MoveDir dir)
{
	tnl::Vector3 pos = GetCharaPos();
	pos += vec[(int)dir] * 1;
	SetCharaPos(pos);
	SetDir((int)dir);

}



