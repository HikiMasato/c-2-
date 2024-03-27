//-------------------------------------------------------
//�����̂���
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include <variant>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//�l�I�ɒǉ������@�\
#include <string_view>
#include <random>         
#include <iostream>  
#include <variant>
//-------------------------------------------------------
//Object
#include "Factory.h"
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "GameManager.h"
#include "SceneManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
//-------------------------------------------------------
//Others
#include "Debug.h"



//------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
ResourceManager::ResourceManager(GameManager* game_manager_) {
	//vector�z��̏�����
	game_manager = game_manager_;
	dungeon_handls.resize(3);
	effect_graphics.resize(15);
	icon_handles.resize(13);
	sound_handle.resize(16);
	item_handle.resize(11);
	
	//csv�̓ǂݍ���
	LoadCharaCsv();
	LoadAttackeffectCsv();
	LoadIconHandleCsv();
	LoadMapChipCsv();
	LoadSoundHnadleCsv();
	LoadItemHandleCsv();
}

//------------------------------------------------------------------------------------------------------------
//�f�X�g���N�^
ResourceManager::~ResourceManager() {
}

//------------------------------------------------------------------------------------------------------------
//�L������񂪏�������ł���csv��ǂݍ��ފ֐�
void ResourceManager::LoadCharaCsv() {
	//�L�����f�[�^��������csv��ǂݍ���
	chara_deta = tnl::LoadCsv<std::string>("csv/chara.csv");
	//�����T�C�Y���m��
	chara_stetus.resize(chara_deta.size());

	for (int i = 1; i < chara_deta.size(); i++) {
		chara_stetus[i - 1].emplace_back(chara_deta[i][0]);//name
		chara_stetus[i - 1].emplace_back(chara_deta[i][1]);//hp
		chara_stetus[i - 1].emplace_back(chara_deta[i][2]);//mp
		chara_stetus[i - 1].emplace_back(chara_deta[i][3]);//attack
		chara_stetus[i - 1].emplace_back(chara_deta[i][4]);//defence
		chara_stetus[i - 1].emplace_back(chara_deta[i][5]);//graphicpath
		chara_stetus[i - 1].emplace_back(chara_deta[i][6]);//exp
		chara_stetus[i - 1].emplace_back(chara_deta[i][7]);//level

	}

}

//------------------------------------------------------------------------------------------------------------
//csv����ǂݍ��񂾃X�e�[�^�X�̒��̉摜�p�X��Ԃ��Q�b�^�[
std::string ResourceManager::GetCharaGraphicPath() {

	std::string pl_ph = chara_deta[1][1];

	return pl_ph;
}

//------------------------------------------------------------------------------------------------------------
//�����̘A�Ȃ����摜��vector�z��Ɋi�[����
void ResourceManager::LoadDivGraphEx(const std::string graph, const int allchip, const int wigthchip, const int heightchip, int xsize, int ysize, std::vector<int>& ghvector) {

	int* buff = new int[allchip];
	LoadDivGraph(graph.c_str(), allchip, wigthchip, heightchip, xsize, ysize, buff);
	
	for (int i = 0; i < allchip; i++) {
		ghvector.emplace_back(buff[i]);
	}
	delete[] buff;

}

//------------------------------------------------------------------------------------------------------------
//csv�t�@�C���̉摜�n���h����vector�z��Ɋi�[����
void ResourceManager::LoadMapChipCsv() {

	//csv�t�@�C���ɏ�����GraphicPath��ǂݍ���
	dungeon_map_chip = tnl::LoadCsv<std::string>("csv/resource.csv");
	
	for (int i = 0; i < 2; i++) {
		for (int k = i * 6 + 1; k < i * 6 + 7; k++) {
			std::string chip = dungeon_map_chip[k][1];
			dungeon_handls[i].emplace_back(game_manager->LoadGraphEx(chip));
		}
	}
	
}

//------------------------------------------------------------------------------------------------------------
//csv�t�@�C���̉摜�n���h����vector�z��Ɋi�[����
void ResourceManager::LoadAttackeffectCsv()
{
	//�G�t�F�N�g�f�[�^���܂Ƃ߂��Ă���csv��ǂݍ���
	effect_graphics = tnl::LoadCsv<std::string>("csv/effect.csv");
}

//------------------------------------------------------------------------------------------------------------
//icon�n���h����vector�z��Ɋi�[����
void ResourceManager::LoadIconHandleCsv()
{
	//icon�n���h�����܂Ƃ߂��Ă���csv��ǂݍ���
	icon_handles = tnl::LoadCsv<std::string>("csv/icon.csv");
}

//------------------------------------------------------------------------------------------------------------
//sound�n���h����vector�z��Ɋi�[����
void ResourceManager::LoadSoundHnadleCsv()
{
	sound_handle = tnl::LoadCsv<std::string>("csv/sound.csv");
}

//------------------------------------------------------------------------------------------------------------
//item�n���h����vector�z��Ɋi�[����
void ResourceManager::LoadItemHandleCsv()
{
	item_handle = tnl::LoadCsv<std::string>("csv/item.csv");
}
