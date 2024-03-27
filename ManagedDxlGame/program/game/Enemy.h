///*****************************************
/// 
/// 
///  
///***************************************** 
#pragma once

class Object;
class Character;
class EnemyManager;
class GameManager;
class EffectManager;

class Enemy final : public Character {
public:
	Enemy(){}
	Enemy(tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, GameManager* game_manager_);
	~Enemy();

	ObjectType GetObjectType() const override {
		return ObjectType::ENEMY;
	}

	void Update(float delta_time)override;

	//�G�s�������֐�
	bool EnemyMove();
	//�ړI�n�Ɍ������֐�
	void MoveDestination();

	//�����̑̂̌����ɐi�ފ֐�
	bool MoveToDir(const  MoveDir dir, tnl::Vector3 pos);

	//�����̂ЂƂ܂��悪�ړ��\�ȏꏊ���𒲂ׂ�֐�
	bool CheckCanMove(const MoveDir dir, const tnl::Vector3 pos);

	//�����̌��݌����Ă�������������̕����֍s���邩�ǂ������`�F�b�N����֐�
	//arg1�c���݌����Ă������
	//arg3�c���������݌����Ă����������s���邩���ׂ�������
	bool CheckCanMoveToDir(const MoveDir dir, const tnl::Vector3 pos, const MoveDir check_dir);

	//EnemyAttack�Ŋ֐��Ń����_���A���̎��̏�Ԃɂ���Ĕ�������X�L����ύX����
	std::shared_ptr<EffectManager> EnemyAttack(Skill* last_skill, std::list<std::shared_ptr<EffectManager>> draw_effect_list, std::shared_ptr<Enemy>enemy_);

	//EnemyAnimation��EnemyAttac�Ō��܂����X�L�����󂯎��A���̃X�L����Animation��߂�l�ŕԂ���悤�ɂ���
	std::shared_ptr<EffectManager> EnemyEffect(std::vector<int> skill_list, tnl::Vector3 pos, int anim_speed, int index);


private:
	//�ړI�n�̍��W
	tnl::Vector3 destination_pos;
	//���ݍ��W
	tnl::Vector3 ene_now_pos;

	//�����O�ł����-1
	int ene_now_in_room = -1;

	//�v���C���[�𔭌����Ă��邩�ǂ����̃t���O
	bool pl_discovery = false;
	//�v���C���[�����Ȃ��ꍇ���ɖړI�n���ݒ肳��Ă��邩�ǂ����̃t���O
	bool other_discovery = false;
	//�����������ǂ����̃t���O
	bool same_room = false;
	//
	bool is_not_wall[4];

	//������\���P�ʃx�N�g��
	tnl::Vector3 vec[4]{
		vec[(int)MoveDir::DOWN]  = { 0, 1, 0 },
		vec[(int)MoveDir::LEFT]	 = { -1, 0, 0 },
		vec[(int)MoveDir::RIGHT] = { 1, 0, 0 },
		vec[(int)MoveDir::UP]	 = { 0, -1, 0 }
	};

	//�����t���O�ێ��͈�
	const float  DISCOVERYRANGE = 9;
	//�R�X�g
	float cost[(int)MoveDir::DIRMAX];

	//�q���[���X�e�B�b�N�֐����`����
	//�����ł̓��[�N���b�h�������g��
	float Heuristic(tnl::Vector3 node, tnl::Vector3 goal) {
		return sqrt(pow(node.x - goal.x, 2) + pow(node.y - goal.y, 2));
	}

	//���͂Ƀv���C���[�����邩�ǂ������`�F�b�N����
	bool CheckIsTherePlayer(MoveDir dir, tnl::Vector3 pos);

	//dir���猩�ē���̍��W���ǂ̌��������擾����
	MoveDir GetDir(const MoveDir dir, const MoveDir getdir);

	//�ړI�n�����Z�b�g����֐�
	void ResetDistination();
	//�v���C���[�Ƃ̋��������ꂷ�����甭���t���O�������֐�
	void ResetPlayerDiscovery();

	//dir�����ɂ����i�ފ֐�
	//<���ӎ���>
	//���̊֐��̎g�p���͈ړ��\�Ȃ��Ƃ�ۏ؂��邱��
	//���̊֐����g�͈ړ��\������s���Ă��Ȃ�
	void SolyMoveToDir(const MoveDir dir);
	
	GameManager* game_manager;
protected:
};