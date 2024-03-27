///*****************************************
/// Enemy�̐�����X�e�[�^�X�̏������Ȃǂ��s���N���X
/// ���̃N���X��Enemy�𐶐����Ă���
/// 
///***************************************** 
#pragma once
#include "MyCamera.h"

class Charactor;
class Enemy;
class GameManager;

class EnemyManager {
public:
	enum class EnemyName {
		SKELETON,
		POISONSKELETON,
		BRACKSKELETON,
		MAGICIAN,
		KING,
	};
	//�f�t�H���g�R���X�g���N�^
	EnemyManager(){}
	//�L�����X�e�[�^�X���i�[���ꂽvector�ƁA�摜�n���h�����i�[���ꂽvector�������Ɏ󂯎��
	//
	EnemyManager(std::vector<std::vector<int>>& re_stetus, std::vector<std::vector<std::string>>& re_graphic, GameManager* game_manager_);

	
	
	//�G�̃X�e�[�^�X�Ɩ��O�A�摜�n���h�������vector�ɂ܂Ƃ߂�
	std::vector<std::pair<std::vector<int>, std::vector<std::string>>> ene_info;

	void Update(float delta_time);
	
	//Hp��0�ɂȂ����G��alive_enemies�����������֐�
	void DeadEnemyErase();


private:

	GameManager* game_manager;
};