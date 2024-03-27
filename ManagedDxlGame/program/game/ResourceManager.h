///************************************************
///	�Q�[���Ŏg���摜�n���h�������N���X
/// csv��ǂݍ��ފ֐��Ȃǂ�����
/// 
/// 
///************************************************ 
#pragma once
#include<variant>

class ResourceManager {
public:
	ResourceManager(GameManager* game_manager_);
	~ResourceManager();
	
	//csv�ɏ�������ł���L������HP,ATTACK,DEFENCE,GraphicPath�Ȃǂ�vector�z��Ɋi�[
	std::vector<std::vector<std::string>>chara_deta;
	//�L�����̃X�e�[�^�X���i�[����vector�z��
	std::vector<std::vector<std::string>>chara_stetus;

	std::vector<std::vector<std::string>> effect_graphics;

	std::vector<std::vector<std::string>> dungeon_map_chip;
	std::vector<std::vector<int>> dungeon_handls;

	//icon_handle���i�[����Ă���
	std::vector<std::vector<std::string>> icon_handles;
	//sound_handle���i�[����Ă���
	std::vector<std::vector<std::string>> sound_handle;
	//item_handle���i�[����Ă���
	std::vector<std::vector<std::string>> item_handle;
	//�L������񂪏�������ł���csv��ǂݍ��ފ֐�
	void LoadCharaCsv();
	//csv����ǂݍ��񂾃X�e�[�^�X�̒��̉摜�p�X��Ԃ��Q�b�^�[
	std::string GetCharaGraphicPath();

	//�������̘A�Ȃ����摜��vector�z��Ɋi�[����֐�
	//arg1�c�摜�p�X
	//arg2�cchip�̑���
	//arg3�c����chip��
	//arg4�c�c��chip��
	//arg5�cchip�̉��T�C�Y
	//arg6�cchip�̏c�T�C�Y
	//arg7�cvector�z��
	void LoadDivGraphEx(const std::string graph, const int allchip, 
						const int wigthchip, const int heightchip, 
						int xsize, int ysize, std::vector<int>& ghvector);

	//�摜�n���h����vector�z��Ɋi�[����
	void LoadMapChipCsv();
	//attackeffect��vector�z��Ɋi�[����
	void LoadAttackeffectCsv();
	//icon�n���h����vector�z��Ɋi�[����
	void LoadIconHandleCsv();
	//sound�n���h����vector�z��Ɋi�[����
	void LoadSoundHnadleCsv();
	//item�n���h����vector�z��Ɋi�[����
	void LoadItemHandleCsv();
	
private:

	GameManager* game_manager;
};

