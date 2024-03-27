///*************Description*********************
/// �S�I�u�W�F�N�g�̊��N���X
/// �I�u�W�F�N�g���ʂ̕ϐ���񋓌^�Ȃǂ����N���X
/// Actor�N���X�ɂ��ׂ�������(���ȓ_)
/// 
///*********************************************
#pragma once

class Skill;

class Object  {
public:
	Object(){}
	virtual ~Object() {};

	//�I�u�W�F�N�g�^�C�v
	enum class ObjectType {
	    PLAYER,
		ENEMY,
		ITEM,
		MAP
	};	
	//�I�u�W�F�N�g�̌���(�L�����N�^�[�̌�����\���񋓎q)
	enum class MoveDir : int {
		DOWN,
		LEFT,
		RIGHT,
		UP,
		DIRMAX
	};

	MoveDir dirs[4] = { MoveDir::DOWN,MoveDir::LEFT,MoveDir::RIGHT,MoveDir::UP };

	//=======================�Q�b�^�[===============================
	//ObjectType�̃Q�b�^�[
	//���̊֐���Object�̎�ނ�Ԃ������ŁAObject���̂ɂ͉e���������Ȃ�
	virtual ObjectType GetObjectType() const = 0;

	//�L�����̍��W��Ԃ�
	inline tnl::Vector3 GetCharaPos() const {
		return chara_pos;
	}
	//�}�b�v�`�b�v�̕`����W�Q�b�^�[
	inline tnl::Vector3 GetChipPos() const {
		return chip_pos;
	}
	//�}�b�v�`�b�v�̌����W(�f�o�C�X���W�n)�Q�b�^�[
	inline tnl::Vector3 GetOldChipPos() const {
		return old_chip_pos;
	}

	//�L�����N�^�[�`�b�v�̃T�C�Y�Q�b�^�[
	int GetCharaChipSize() const {
		return chra_chip_size;
	}

	//�}�b�v�`�b�v1�c������̃T�C�Y
	inline int GetMapChipSize()const {
		return SIZE;
	}

	//�I�u�W�F�N�g�̐����t���O�Q�b�^�[
	inline bool GetIsAlive() const {
		return is_alive;
	}

	//=======================�Z�b�^�[===============================
	
	//�L�����N�^�[�̈ړ��Ɋւ���ړ��֐�
	void SetMoveCharaPos(MoveDir dir);


	//�f�o�C�X���W�n�ł̃L�����̃|�W�V�����̃Z�b�^�[
	void SetCharaPos(tnl::Vector3 pos) {
		chara_pos = pos;
	}
	//�����蔻��Ŏg�p
	tnl::Vector3& SetCharaPosToCollistion() {
		return chara_pos;
	}
	//�}�b�v�`�b�v�̂��ƍ��W
	tnl::Vector3& SetOldMapChipPos() {
		return old_chip_pos;
	}

	//�}�b�v�`�b�v�̕`����W�Z�b�^�[
	void SetChipPos(tnl::Vector3 pos) {
		chip_pos = pos;
	}
	//�}�b�v�`�b�v�̂��ƍ��W�Z�b�^�[(�f�o�C�X���W�n)
	void SetOldChipPos(tnl::Vector3 pos) {
		old_chip_pos = pos;
	}
	
	//�����t���O���Z�b�g����
	void SetIsAlive(bool now_alive) {
		is_alive = now_alive;
	}

	virtual void Update(float delta_time);
	virtual void Draw(const hm::Camera& camera);


	//�}�b�v�`�b�v�L�����`�b�v�̃T�C�Y
	const int chra_chip_size = 32;

	

	//�摜�n���h��
	int drawph = 0;
private:

	//�p�����Ă���Object�̍��W
	tnl::Vector3 chara_pos = { 0,0,0 };

	//mapchip���W
	tnl::Vector3 chip_pos;

	//�`�b�v�̂��ƍ��W
	tnl::Vector3 old_chip_pos;

	//�`�b�v��̑傫��
	const int SIZE = 20;

	//�����t���O
	bool is_alive = true;

};