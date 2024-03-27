///*******************Description********************
///�I�u�W�F�N�g���N���X���p�������L�����N�^�[�N���X
///���ׂẴL�����N�^�[�̊��N���X�ɂ�����N���X
///�L�����N�^�[�ɋ��ʂ�����̂����� 
///************************************************** 
#pragma once


class Object;

class Character : public Object{
public:

	Character(){}
	virtual ~Character() {};

	//-------------------------------------------------------
	//�L�����N�^�[�N���X
	enum class Stetus {
		HP,
		MP,
		ATK,
		DEF,
		GOLD,
		DIAMOND,
		EXP,
		LEVEL
	};

	

	void Update(float delta_time)override;

	//�L�����N�^�[�̕`��
	void Draw(const hm::Camera& camera)override;

	//�A�j���[�V���������s
	void Animation(int* grapharray, int speed, int maxindex, int& drawph);

	//�L�����N�^�[�̃X�e�[�^�X�����l
	void ReinforceStetus();

	
	//--------------------------------------------------------------------------------------------------
	// �L�����X�e�[�^�X�̃Q�b�^�[
	// 
	
	//�L������Dir�̃Q�b�^�[
	MoveDir GetCharaDir() const {
		return my_dir;
	}
	
	//�L�����N�^�[�̃}�b�v�����W
	inline tnl::Vector3 GetCharaInPos() const {
		return chara_in_map;
	}

	//�G�t�F�N�g���Đ�������W�Q�b�^�[
	inline tnl::Vector3 GetEffectPos() const {
		return effect_pos;
	}
	
	//�����^�̃L�����̃X�e�[�^�X��Ԃ��Q�b�^�[
	int GetCharaStetus(Stetus stetus) const;

	//�L�����N�^�[�̑��`�b�v��
	int GetCharaChipAll() const {
		return chra_chip_all;
	}
	//�L�����N�^�[�`�b�v�̃C���f�b�N�X��
	int GetCharaChipIndex() const {
		return chra_chip_index;
	}
	
	// �L�����`�b�v�z��̃Q�b�^�[�֐�(�ǂݎ���p(Read Only))
	const int* GetCharaChipArray() const {
		return chra_chip_array;
	}

	//���݂̎��g�̕����ԍ���Ԃ�
	int GetNowRoomValue() const {
		return nowroomvalue;
	}
	//�U���ς݂��ǂ����̃t���O��Ԃ�
	bool GetIsAttacked() const {
		return is_attack;
	}
	int GetObjectSpeed() const {
		return obj_speed;
	}
	int GetCharaExp() const {
		return chara_exp;
	}

	//--------------------------------------------------------------------------------------------------
	//�L�����X�e�[�^�X�̃Z�b�^�[
	
	
	//�`����W�ł̃L�����̃|�W�V�����̃Z�b�^�[
	void SetCharaInPos(tnl::Vector3 pos) {
		chara_in_map = pos;
	}
	//�A�j���[�V����(�X�L��)�`����W�Z�b�^�[
	void SetEffectPos(tnl::Vector3 pos) {
		effect_pos = pos;
	}
	//Dir�̃Z�b�^�[
	void SetDir(const int dir) {
		my_dir = dirs[dir];
	}
	//���݂̒n�k�̕����ԍ��Z�b�^�[
	void SetNowRoomValue(int now_room) {
		nowroomvalue = now_room;
	}
	//�U���ςݔ�����Z�b�g����
	void SetIsAttacked(bool now_attacked) {
		is_attack = now_attacked;
	}
	//�L�����o���l���X�V����
	void SetCharaExp(int now_exp) {
		chara_exp = now_exp;
	}
	//�L�������x���̍X�V
	void SetCharaLevel(int now_level) {
		chara_level = now_level;
	}

	//�A�j���[�V�������W(�G�t�F�N�g�Đ����W)
	void SetAnimationPos(tnl::Vector3 pos);
	
	//�����^�̃v���C���[�̒l���󂯎��w�肳�ꂽ�X�e�[�^�X�ւƃZ�b�g�����
	void SetCharaStetus(Stetus stetus, int set_value);

	//�L�����̃X�e�[�^�X��ϓ�������֐�()
	//�_���[�W�A�A�C�e���g�p���ȂǂɎg�p
	void SetAffectToCharaStetus(Stetus stetus, int affect_value);


	//-----------------------------------------------------
	//�L�����N�^�[ 
	//�U���֐�
	virtual void Attack();
	virtual void SkillAttack(Skill* used_skill);
	
	//�����X�L�����擾����
	inline std::vector<Skill*>& GetSkillList() {
		return my_skill;
	}
	
	//�L������Hpbar
	void DrawHpbarCharactor(int hp, const tnl::Vector3& pos);

	//�L�����`�b�v�̃C���f�b�N�X
	int chra_chip_index = 0;

	//�L�����`�b�v�̑���
	int chra_chip_all = 12;
	

private:

	//�����l�K�{
	MoveDir my_dir = MoveDir::DOWN;

	//==================���W=======================
	 
	
	//map���L�������W
	tnl::Vector3 chara_in_map = { 0,0,0 };
	//effect�`����W
	tnl::Vector3 effect_pos;



	//=================stetus====================== 
	 
	//�L����hp
	int chara_hp;
	//�L����mp
	int chara_mp;
	//�L�����U����
	int chara_attack;
	//�L�����h���
	int chara_defence;
	//������//GetGold
	int have_gold = 1000;
	//�����_�C�A�����h
	int have_diamond = 0;

	//�o���l
	int chara_exp = 0;
	//�L�������x��
	int chara_level = 0;

	//================�摜�f�[�^===================
	 
	

	//�L�����`�b�v���i�[����z��
	int chra_chip_array[12];

	//============�A�j���[�V�����f�[�^=============
	
	//�ő�v�f��(�L�����`�b�v)
	int maxindex = 3;
	
	//�C���^�[�o��(�A�j���[�V�����Đ�)
	int obj_speed = 20;
	float obj_wait = obj_speed;



	//=============�L�����ڍ׏��==================
	//�L�����N�^�[�̕����ԍ�
	int nowroomvalue;
	//�U���������ǂ����̃t���O(�U���ς݁ctrue)
	bool is_attack = false;
	//�����X�L���ꗗ//�L�����N�^�[
	std::vector<Skill*> my_skill = {};



	//=============�X�e�[�^�X������=================
	const int RAINFORCE_MIN = 5;
	const int RAINFORCE_MAX = 10;

};
