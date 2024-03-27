///*****************Description********************
/// ResourceManager�œǂݍ��񂾃L�����X�e�[�^�X��A�L�����̖��O
/// �L�����摜�p�X��ێ�����N���X
/// effect�A�A�j���[�V�����摜���ێ�����
/// csv�f�[�^�ێ��N���X
///************************************************
#pragma once

class Stetus {
public:
	Stetus(){}

	//csv����擾�������l�̌^�̗񋓌^
	enum class CharaStetus {
		PLAYERINT,
		PLAYERSTRING
	};
	enum class EffectDataType {
		INT,
		STRING
	};
	//������ResourceManager����󂯎��
	//�L�����̃X�e�[�^�X���i�[����Ă���vector���󂯎��
	Stetus(std::vector<std::vector<std::string>>& re_stetus, std::vector<std::vector<std::string>>& effect_csv);

	//�L�����̃X�e�[�^�X���i�[����vector�z��
	std::vector<std::vector<std::string>>chara_stetus;
	//�L�����X�e�[�^�X���i�[����
	std::vector<std::vector<int>> stetus_;
	//�L��������L�����摜�p�X���i�[����
	std::vector<std::vector<std::string>> pl_ph;

	//csv����ǂݍ��񂾃X�e�[�^�X�̃Q�b�^�[
	std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>> GetCharaStetus(CharaStetus type, int pathnum);
	std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>> GetEffect(EffectDataType type);

	std::vector<std::vector<std::string>> effectcsv;
	std::vector<std::vector<int>>effect_int_data;
};