#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//�l�I�ɒǉ������@�\
#include <string_view>
#include <random>         
#include <iostream>  
#include <variant>
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "SceneStartMap.h"
#include "ScenePlay.h"
#include "DungeonScene.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "ObjectManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "SkillManager.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "ItemManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "Player.h"
#include "Enemy.h"
#include "MyCamera.h"
#include "MenuWindow.h"
#include "Item.h"
//-------------------------------------------------------
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//effect
#include "ActionEffect.h"
//-------------------------------------------------------
//others
#include "Stetus.h"
#include "Debug.h"
#include "Skill.h"
#include "Inventory.h"



//------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
SceneStartMap::SceneStartMap()
{
	//�X�^�[�g�}�b�v���C���X�^���X��
	SceneTitle::game_manager->GetObjectManager()->GenerateOrdersToStartMapChip();
	
	//�X�^�[�ƃV�[��������
	InitSceneStartMap();
}

//------------------------------------------------------------------------------------------------------------
//�f�X�g���N�^
SceneStartMap::~SceneStartMap()
{
	//BGM���I��
	StopSoundMem(SceneTitle::game_manager->GetSoundManager()->sound_csv[9]);
	//�v���C���[�̍ŏI���W��ۑ�
	player_pos_buff = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetStartPostion();
}

//------------------------------------------------------------------------------------------------------------
//SceneStartMap������
void SceneStartMap::InitSceneStartMap() 
{
	//�t�H���g�T�C�Y��߂�
	SetFontSize(16);
	//BGM���Đ�
	SceneTitle::game_manager->GetSoundManager()->ChosePlayBGMSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[9]);
	
	//�X�^�[�}�b�v�p�ɃX�P�[����ύX
	if (SceneTitle::game_manager->GetNowScale() != GameManager::ScaleMode::NOMAL) {
		SceneTitle::game_manager->ScaleChange();
	}

	//�X�^�[�g�}�b�v�Ƀv���C���[�𐶐�
	SceneTitle::game_manager->GetObjectManager()->GenerateOrders(SceneTitle::game_manager->GetObjectManager()->factory->GetPlayerSpawn());

	//���j���[�E�B���h�E�𐶐�
	MenuWindow::MenuType_t* menu_usable = new MenuWindow::MenuType_t[]{
		{310,330,"�_���W�����ɓ���܂���?",0},
		{310,360,"��߂�",2}
	};
	map_in_ui = new MenuWindow(300, 300, 230, 150, "using_graphics/window_ui.png", menu_usable, 2, 0.15);



}

//------------------------------------------------------------------------------------------------------------
//�_���W�����ɓ��邩�ǂ�����Ui���o��������`�F�b�N����֐�
bool SceneStartMap::CheckDungeonInUi(tnl::Vector3 pos)
{	
	//��ԉE�̎R
	tnl::Vector3 open_ui_pos = { 260,600,0 };
		

	if (open_ui_pos.x == pos.x && open_ui_pos.y == pos.y) {
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------
//�_���W�����ɓ���UI���I�[�v����Ԃ̏ꍇ�ɃL�[�`�F�b�N����
void SceneStartMap::ActiveKeyCheck(bool open_in)
{
	//�_���W�����ɓ���Ui�t���O������Ă�����return
	if (!open_in)return;
	//������������ꍇ
	if (map_in_ui->select_value == 0 && tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		auto mgr = SceneManager::GetInstance();
		mgr->ChangeScene(new ScenePlay);
	}
	//��߂���������ꍇ
	else if (map_in_ui->select_value == 1 && tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		map_in_ui->manage_select_flag = false;
		map_in_ui_open = false;
	}

}

//------------------------------------------------------------------------------------------------------------
//���t���[������
void SceneStartMap::Update(float delta_time)
{
	//���ׂĂ�Object��Update���Ă�(�C���X�^���X�����m�ۂ������̂̂�)
	SceneTitle::game_manager->GetObjectManager()->Update(delta_time);

	//�v���C���[�̃|�W�V�������擾(���ꔻ��Ɏg�p)
	tnl::Vector3 pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
	player_pos_buff = pos;

	//�����̃`�F�b�N���m�F���悤
	//�v���C���[�ƃ_���W�����ɓ���UI��\����������W����v���Ă��邩����
	if (CheckDungeonInUi(SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos())) {
		map_in_ui->MenuOpen();
		map_in_ui_open = true;
	}

	//�_���W�����ɓ��邩�ǂ����̑I�����m�F����
	ActiveKeyCheck(map_in_ui_open);
}

//------------------------------------------------------------------------------------------------------------
//�`��
void SceneStartMap::Draw()
{
	//startmap�̕`��(basemap)(layer1)
	for (auto chip : SceneTitle::game_manager->GetObjectManager()->GetStartMapChipList()) {
		chip->StartMapDraw(camera);

	}
	//startmap�̕`��(decoration_map)(layer2)
	for (auto second_chip : SceneTitle::game_manager->GetObjectManager()->GetStartMapSecondChipList()) {
		second_chip->StartMapDraw(camera);
	}
	//�_���W�����ɓ��鎞��UI
	if (map_in_ui_open) {
		map_in_ui->manage_select_flag = true;
		map_in_ui->MenuAll();
	}

	//�v���C���[�̕`��
	SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->Draw(camera);
}


