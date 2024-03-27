///***************Description***************
///�X�L���Ɋւ���N���X
///�v���C���[�ƃG�l�~�[�o���������Ƃ��o����X�L���̏��,������ێ�����
///*****************************************
#pragma once

class GameManager;

class Skill {
public:

	Skill(){}
	Skill(int effect_type_, float damege_rate_, float heal_rate_, int all_num_,int x_num_, int y_num_, int x_size_, int y_size_, int act_speed_, 
		  std::string skilltype_, std::string skill_name_, std::string gh_handles_, GameManager* game_manager);

	//�_���[�W�A�񕜗ʂ��擾
	inline const float* GetSkillFloat() {
		return float_data;
	}
	//�X�L�������擾
	inline const std::string& GetSkillName() {
		return skill_name;
	}
	//�A�j���[�V�����摜�n���h���̑������擾
	inline const int& GetGraphicsAllNum() {
		return graphic_all_num;
	}
	//�A�j���[�V�����摜�n���h��vector���擾����
	inline const std::vector<int>& GetGraphicsVector() {
		return anim_gh;
	}
	//�A�j���[�V�����X�s�[�h���擾
	inline const int& GetAnimSpeed() {
		return anim_speed;
	}


private:

	//�X�L��Id
	int effect_type = 0;
	//�X�L���^�C�v
	std::string skill_type;
	//�X�L����
	std::string skill_name;

	//�ȉ�2���܂Ƃ߂�����
	float float_data[2];
	//�U���͂ɂ�����{��
	float damage_rate = 0;
	//�_���[�W�ɂ�����{��
	float heal_rate = 0;
	
	//�t�H���_�p�X
	std::string gh_handles;
	//DivGraph�ŕ��������摜�n���h��
	std::vector<int>anim_gh;

	//�摜�̏��
	int graphic_all_num = 0;
	int x_num = 0;
	int y_num = 0;

	int x_size = 0;
	int y_size = 0;

	int anim_speed = 0;

};