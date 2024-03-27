///*************Description*********************
/// effect�Đ����Ǘ�����N���X
/// animation�`����s���Ă���
/// 
///*********************************************
#pragma once
class hm::Camera;

class EffectManager {
public:
	EffectManager(){}
	//effect_csv�c[0]�`[6]�܂ōU��Effect
	//effect_csv�c[7]����̓Q�[���Ɋւ���Effect
	EffectManager(std::vector<std::vector<std::string>>& effect_csv);
	//���x���A�b�v�p
	EffectManager(std::string path, tnl::Vector3 pos);
	//�X�L���G�t�F�N�g�p�R���X�g���N�^
	EffectManager(std::vector<int>anim,tnl::Vector3 anim_pos,int act_speed,int max_index);

	enum class AttackType {
		NONE,
		ATTACK,
		MAGIC,
		LEVEL
	};

	
	void Update(float delta_time);
	void Draw(const hm::Camera& camera);

	
	//�A�j���[�V�������I�����Ă邩�ǂ����̃t���O���擾����֐�
	inline bool GetAnimIsAlive() {
		return anim_is_alive;
	}

private:

	
	AttackType atk_type = AttackType::NONE;

	//�ϐ��錾
	tnl::Vector3 effect_pos{ 0 };
	int effect_wait = 0;
	int speed = 20;
	int effect_index = 0;
	int effect_gh = 0;
	int max_motion_index = 0;
	int drawph = 0;

	bool anim_is_alive = true;

	//���x���A�b�v�p�z��
	int levelup_gh_array[10] = {};

	std::vector<int>anim_gh;
	std::vector<std::vector<std::string>> all_effct;

};