///******************Description************************
///�v���C���[�N���X
/// �ړ�������A�������̃X�e�[�^�X�ϓ��Ȃǂ��Ǘ�(���ݖ�����)
/// 
/// 
///***************************************************** 
#pragma once
#include "Object.h"
#include "Character.h"
class Object;
class Character;
class GameManager;
class hm::Camera;

class Player final : public Character {
public:

	//�f�t�H���g�R���X�g���N�^
	Player(){}

	//arg1�c�v���C���[�̃X�^�[�g�|�W�V�����������_���ɐݒ肷��
	//arg2�chp
	//arg3�cmp
	//arg4�cdefence
	//arg5�c���O
	//arg6�c�摜�p�X
	Player(tnl::Vector3 start_pos,int hp, int mp, int attack, int defence,std::string name ,std::string chara_ph, int exp, int level, GameManager* game_manager_);
	//�f�X�g���N�^
	~Player();
	
	

	void Update(float delta_time) override;

	//�v���C���[�ړ�
	void Move();

	//�A�C�e���g�p���Ƀv���C���[�̃X�e�[�^�X��ϓ�������
	//stetustype:0->HP,stetustype:1->ATK,stetustype:2->DEF,stetustype:3->GOLD,stetustype:4->DIAMOND
	void EffectToHp(Stetus stetustype,int stetus);

	//�v���C���[�̌o���l�����Z����֐�
	//�o���l���ő�ɂȂ�΃��x�����オ��
	bool AddExp(int add_exp);
	//================�Q�b�^�[===================

	ObjectType GetObjectType() const override {
		return ObjectType::PLAYER;
	}

	//�v���C���[�̃X�^�[�ƃ}�b�v�|�W�V������Ԃ�
	tnl::Vector3 GetStartPostion() const {
		return start_pl_pos;
	}

	int GetNeedExp() const {
		return need_level_up_exp;
	}

	int GetLeftOverExp() const {
		return leftover_exp;
	}

	int GetPlayerMaxHp() const {
		return max_hp;
	}

	//================�Z�b�^�[===================
	
	//�L�����N�^�[��`����W����}�b�v���W�ɕϊ�
	void SetPlayerLocalPos();

	void SetNeedExp(int now_exp) {
		need_level_up_exp = now_exp;
	}

	//���x���A�b�v�܂ł̎c��o���l���X�V����
	void SetLeftOverExp(int now_exp) {
		leftover_exp = now_exp;
	}

	void SetPlayerMaxHp(int new_max_hp) {
		max_hp = new_max_hp;
	}

private:
	GameManager* game_manager;

	//�v���C���[�̃X�^�[�ƃ}�b�v�|�W�V����
	tnl::Vector3 start_pl_pos = { 700, 600, 0 };

	//���x���A�b�v�ɕK�v�Ȍo���l(1lv�オ�邲�ƂɕK�v�o���l���オ���Ă���)
	int need_level_up_exp = 100;
	//���x���A�b�v�ɕK�v�Ȏc��o���l
	int leftover_exp = 0;
	//�K�v�o���l�𒴂����擾�o���l��ۑ�����
	int over_need_exp = 0;
	//�K�v�o���l���v�Z����ۂɎg�p����x�[�X�K�v�o���l
	const int BASENEEDEXP = 55;
	//�o���l������
	const float EXPINCREASERATE = 10.5; 

	//�v���C���[�̍ő�HP
	int  max_hp = 0;

	//�K�v�o���l�𑝉�������֐�
	void NeedUpExp();

	//debug�p��Hp�̑�������
	void DebugHpFluctuation();
};