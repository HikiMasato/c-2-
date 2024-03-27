//-------------------------------------------------------
//�����̂���
#include <time.h>
#include <string>
#include <numbers>
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
#include "Character.h"
#include "MapChip.h"
#include "Player.h"
#include "Enemy.h"
#include "MyCamera.h"
#include "MenuWindow.h"
#include "Item.h"
//-------------------------------------------------------
//Collision
#include "Collision.h" 
//-------------------------------------------------------
//others
#include "FadeControl.h"
#include "Stetus.h"
#include "Debug.h"
#include "Skill.h"
#include "Inventory.h"


//------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
DungeonScene::DungeonScene()
{
	//�������֐�
	SceneTitle::game_manager->InitGameManager();
	InitDungeonScene();
	
	fade_control = std::make_shared<FadeControl>();

	//�_���W����BGM�Đ�
	SceneTitle::game_manager->GetSoundManager()->ChosePlayBGMSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[2]);

}

//------------------------------------------------------------------------------------------------------------
//�f�X�g���N�^
DungeonScene::~DungeonScene()
{
	StopSoundMem(SceneTitle::game_manager->GetSoundManager()->sound_csv[2]);
}

//------------------------------------------------------------------------------------------------------------
// ���t���[�����s����܂�
void DungeonScene::Update(float delta_time)
{
	ChangeMainSequence(delta_time);

	if (fade_sequence_type != FadeSequence::FADEDESC) {

		//�}�E�X�̃J�[�\���̍��W���擾
		GetMousePoint(&mouce_x, &mouce_y);

		//�v���C���[�̃|�W�V�������擾
		tnl::Vector3 pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
		player_pos = SceneTitle::game_manager->WorldToLocalPos(pos);

		//update
		SceneTitle::game_manager->update(delta_time);
		//�~�j�}�b�v�`�攻��
		SceneTitle::game_manager->RefreshMiniMap(player_pos);
		//�G�t�F�N�g�̖��t���[������
		EffectUpdate(delta_time);

		//�G�t�F�N�g�I������
		CheckEffectIsAlive();



		//===================�����蔻��======================
		//�A�C�e���Ƃ̏Փ˔���
		if (DetectItem()) {
			for (auto item : SceneTitle::game_manager->GetObjectManager()->GetAliveItem()) {
				//�������Ă���A�C�e����������continue
				if (item->GetIsAlive())continue;
				if (SceneTitle::game_manager->DetectDropItem(item, SceneTitle::game_manager->GetObjectManager()->GetAliveItem()))break;
			}
		}

		//�����蔻��`�F�b�N
		SceneTitle::game_manager->UpdateHitCheck();
		//�������Ă��Ȃ��L�����̓����蔻����폜����
		SceneTitle::game_manager->EraseHitList();

		//�K�i�ɏ��������enter�L�[���������ΊK�w���������
		CheckExtraOnTile();

	}
	
}
//------------------------------------------------------------------------------------------------------------
//�`��
void DungeonScene::Draw()
{
	//�t�F�[�h���łȂ����
	if (fade_sequence_type != FadeSequence::FADEDESC) {

		SceneTitle::game_manager->Draw();
		//�G�t�F�N�g�`��
		EffectDraw(SceneTitle::game_manager->GetObjectManager()->factory->camera);
		//UI�֘A�̕`��
		UIDraw();

	}
	//�t�F�[�h���̕����`��
	if (fade_sequence_type == DungeonScene::FadeSequence::FADEDESC) {
		SetFontSize(30);
		DrawFadeDesc();
		SetFontSize(16);
	}
}

//------------------------------------------------------------------------------------------------------------
//�_���W�����V�[���̏�����
void DungeonScene::InitDungeonScene()
{
	SetFontSize(16);

	//�I�u�W�F�N�g���i�[����Ă��郊�X�g�����ׂăN���A
	SceneTitle::game_manager->GetObjectManager()->AllListClear();
	
	//�v���C���[�̃X�|�[���^�C�v��ύX����
	SceneTitle::game_manager->GetObjectManager()->SetPlayerSpawnType(Factory::PlayerSpawn::DUNGEON);
	//�I�u�W�F�N�g����
	SceneTitle::game_manager->GetObjectManager()->GenerateOrders(Factory::PlayerSpawn::DUNGEON);
	SceneTitle::game_manager->GetObjectManager()->GenerateAllItem();

	//������
	icon_handle.resize(ICON_NUM);
	skill_set_index.resize(ICON_NUM);
	clicked_set_icon.resize(ICON_NUM);
	icon_list.resize(SceneTitle::game_manager->GetResourceManager()->icon_handles.size());
	
	
	//icon_list�ɃA�C�R���摜�n���h�����܂Ƃ߂�
	for (int i = 0; i < SceneTitle::game_manager->GetResourceManager()->icon_handles.size(); i++) {
		icon_list[i] = SceneTitle::game_manager->LoadGraphEx(SceneTitle::game_manager->GetResourceManager()->icon_handles[i][0]);
	}
	
	//������factory�Ɉڂ��Ă���������
	//----------------------------------------------------------------------------------
	//   �@�@�@****UI�C���X�^���X�̊m��****
	//
	//**********************Menu***********************

	stetus_ui = new Menu(base_skill_set_pos.x - 500, base_skill_set_pos.y + 150, 500, 170, "using_graphics/window_ui.png");
	gold_ui			 = new Menu(1060, 10, 220, 100, "using_graphics/window_ui.png");
	skill_ui		 = new Menu(base_skill_set_pos.x, base_skill_set_pos.y - 40, 90, 360, "using_graphics/window_ui.png");
	have_skill_ui	 = new Menu(base_skill_set_pos.x - 500, base_skill_set_pos.y - 150, 500, 300, "using_graphics/window_ui.png");
	alway_menu		 = new Menu(10, 260, 150, 150, "using_graphics/window_ui.png");
	option			 = new Menu(100, 300, 220, 220, "using_graphicse/window_ui.png");//(WIP)

	inventory		 = new Menu(380, 220, 440, 330, "using_graphics/window_ui.png");
	desc			 = new Menu(820, 400, 300, 150, "using_graphics/window_ui.png");

	dungeon_level_ui = new Menu(400, 400, 200, 200, "using_graphics/window_ui.png");

	shop_ui			 = new Menu(400, 400, 200, 200, "using_graphics/window_ui.png");
	shop_inve_ui	 = new Menu(410,120,320,340, "using_graphics/window_ui.png");
	shop_coin_ui	 = new Menu(720,280,300,200, "using_graphics/window_ui.png");
	shop_my_inve_ui	 = new Menu(90,120,320,340, "using_graphics/window_ui.png");


	//**********************MenuWindow**********************************
	//
	//�@�@�@�@�@�@�@�@�Z���N�g���ł���E�B���h�E
	//
	MenuWindow::MenuType_t* menu_usable = new MenuWindow::MenuType_t[]{
		{460,450,"�g��",0},
		{460,480,"��߂�",2}
	};
	use_usable = new MenuWindow(440, 440, 200, 200, "using_graphics/window_ui.png", menu_usable, 2, 0.15);

	MenuWindow::MenuType_t* first_menu_ = new MenuWindow::MenuType_t[]{
		{170,280,"������",0},
		{170,310,"�^�C�g���֖߂�",1},
		{170,340,"���j���[�����",2}
	};
	//tab�L�[���������Ƃ��̍ŏ��̃��j���[
	first_menu = new MenuWindow(160, 260, 220, 220, "using_graphics/window_ui.png", first_menu_, 3, 0.45);
	
	//�V���b�v�p�C���x���g��
	if (shop_pages.empty()) {
		//�Ȃ���΍��
		Inventory* new_shop_inven = new Inventory(shop_page_value + 1,SceneTitle::game_manager);
		//�V���b�v�y�[�W���X�g�ɒǉ�
		shop_pages.emplace_back(new_shop_inven);
	}
	

	//�����蔻��y�A����
	SceneTitle::game_manager->SetObjectSHitCheck();

}

//------------------------------------------------------------------------------------------------------------
//UI�֘A�̕`��
void DungeonScene::UIDraw()
{

	// �h���b�O�A���h�h���b�v��L���ɂ���
	SetDragFileValidFlag(TRUE);

	{//�v���C���[�X�e�[�^�X
		stetus_ui->MenuDraw();
		DrawStringEx(hpbar_postion.x, hpbar_postion.y - 45, GetColor(255, 255, 255), "�v���C���[�̃X�e�[�^�X");
		{//level
			DrawStringEx(hpbar_postion.x, hpbar_postion.y - 20, GetColor(255, 255, 255), "LEVEL %d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::LEVEL));
		}
		{//hp
			SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->DrawHpbarCharactor(SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::HP), { hpbar_postion.x, hpbar_postion.y + 20,0 });
			DrawRotaGraph(hpbar_postion.x - 20, hpbar_postion.y + 25, 1.0f, 0, icon_list[11], true);
			DrawStringEx(hpbar_postion.x , hpbar_postion.y, GetColor(255, 255, 255), "HP  %d/%d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::HP), SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetPlayerMaxHp());

		}
		{//attack
			DrawRotaGraph(hpbar_postion.x - 20, hpbar_postion.y + 50, 1.0f, 0, icon_list[1], true);
			DrawStringEx(hpbar_postion.x, hpbar_postion.y + 40, GetColor(255, 255, 255), "ATK %d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::ATK));
		}
		{//defence
			DrawRotaGraph(hpbar_postion.x - 20, hpbar_postion.y + 75, 1.0f, 0, icon_list[3], true);
			DrawStringEx(hpbar_postion.x, hpbar_postion.y + 65, GetColor(255, 255, 255), "DEF %d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::DEF));
		}
		{//exp
			DrawStringEx(hpbar_postion.x, hpbar_postion.y + 90, GetColor(255, 255, 255), "���̃��x���܂Ŏc�� %d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetLeftOverExp());
		}
	}

	{//������(���W��)UI
		gold_ui->MenuDraw();
		DrawRotaGraph(base_gold_icon_pos.x, base_gold_icon_pos.y, 1.0f, 0, icon_list[5], true);
		DrawStringEx(base_gold_icon_pos.x + 150, base_gold_icon_pos.y - 10, GetColor(255, 255, 255), "%5d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::GOLD));
	}

	{//�����_�C�������h
		DrawRotaGraph(base_gold_icon_pos.x, base_gold_icon_pos.y + 25, 1.0f, 0, icon_list[4], true);
		DrawStringEx(base_gold_icon_pos.x + 150, base_gold_icon_pos.y + 20, GetColor(255, 255, 255), "%5d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::DIAMOND));
	}
	
	{//�X�L�����Z�b�g����UI
		skill_ui->MenuDraw();
		
		// �X�L���̃A�C�R����z��ŊǗ�����
		int slot_icon_list[ICON_NUM] = { icon_list[6], icon_list[7], icon_list[8], icon_list[9], icon_list[10] };

		// �X�L�����Z�b�g����UI
		skill_ui->MenuDraw();

		// �c���z�u
		// ���[�v�ŃA�C�R����`�悷��
		for (int i = 0; i < ICON_NUM; i++) {
			//�X���b�g�X�L����`��
			DrawRotaGraph(base_skill_set_pos.x + 45, base_skill_set_pos.y + i * 70, 1.0f, 0, slot_icon_list[i], true);
		}

	}

	if (now_draw_uiwin == NowDrawUiWindow::SKILL) {

		have_skill_ui->MenuDraw();
		DrawStringEx(base_skill_set_pos.x - 450, base_skill_set_pos.y - 140, -1, "�����X�L���ꗗ");
		if (mouse_drag) {
			// �摜�̈ʒu���}�E�X�̈ʒu�ɍ��킹��
			icon_x[clicked_icon] = mouce_x - ICON_WIDTH / 2;
			icon_y[clicked_icon] = mouce_y - ICON_HEIGHT / 2;
			// �h���b�O���̉摜��`��
			DrawRotaGraph(icon_x[clicked_icon], icon_y[clicked_icon], 1.0f, 0, icon_handle[clicked_icon], true);

		}

		//�c���z�u
		//�����X�L���A�C�R���ꗗ��`�� 
		// �A�C�R���̍��W�ƃn���h��������������
		for (int i = 0; i < ICON_NUM; i++) {
			icon_x[i] = ICON_X + i * (ICON_WIDTH + ICON_SPACING);
			icon_y[i] = ICON_Y;
			icon_handle[i] = SceneTitle::game_manager->LoadGraphEx(SceneTitle::game_manager->GetResourceManager()->effect_graphics[5 + i][12]);
			
			// �z��Ɋ܂܂��C���f�b�N�X�̃X�L���͕`�悵�Ȃ�
			bool skip = false;
			for (int j = 0; j < ICON_NUM; j++) {
				if (drag_set_icon[j] == i) {
					skip = true;
					break;
				}
			}
			if (skip) continue;
			if (i != clicked_icon) {
				// �X�L����`�悷��
				DrawRotaGraph(icon_x[i], icon_y[i], 1.0f, 0, icon_handle[i], true);
			}
		}
		
	}
	// ���[�v�ŃX�L���Z�b�g�Ƀh���b�v�����A�C�R����`�悷��
	for (int i = 0; i < SKILL_SET_NUM; i++) {
		//�X�L���Z�b�g�Ƀh���b�v�����A�C�R����`��
		DrawRotaGraph(SKILL_SET_X, SKILL_SET_Y + i * 70, 1.0f, 0, skill_set_index[i], true);
	}
	

	first_menu->MenuAll();
	alway_menu->MenuDraw();
	DrawStringEx(alway_menu->menu_x + 10, alway_menu->menu_y + 50, -1, "Menu���J��");
	//optionwindow���J���Ă�����
	if (now_draw_uiwin == NowDrawUiWindow::OPTION) {
		option->MenuDraw();

		
	}

	//�C���x���g���I�[�v�����`��
	if (now_menu_type == MenuSequence::INVENTORY_OPEN || now_menu_type == MenuSequence::INVENTORY_USE) {
		//�C���x���g����UI�`��
		inventory->MenuDraw();

		DrawInventory(inventory->menu_x, inventory->menu_y);
		//�A�C�e���̎g�p���邵�Ȃ�UI
		if (now_draw_sec_uiwin == NowDrawSecondWindow::ITEMUSE) {
			use_usable->MenuAll();
		}
	}
	//�V���b�v���`��
	else if (now_draw_uiwin == NowDrawUiWindow::SHOP) {

		//�V���b�v�C���x���g���̕`��
		shop_inve_ui->MenuDraw();

		//�����T�C�Y�ύX
		SetFontSize(25);
		//�V���b�v�C���x���g�����e�̕`��
		shop_pages[draw_shop_page]->DrawInventory(shop_inve_ui->menu_x + 10, shop_inve_ui->menu_y + 10);
		//�����T�C�Y�ύX
		SetFontSize(16);

		//�K�v�R�C���`��
		shop_coin_ui->MenuDraw();
		shop_pages[draw_shop_page]->DrawNeedCoin(shop_coin_ui->menu_x, shop_coin_ui->menu_y);
		
		//�v���C���[�̃C���x���g����`��
		shop_my_inve_ui->MenuDraw();
		DrawInventory(shop_my_inve_ui->menu_x, shop_my_inve_ui->menu_y);

	}
	
	//�K�i�ɏ���Ă�ꍇ
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::STAIRS) {
		dungeon_level_ui->MenuDraw();

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 60, -1, "�K�i��������");

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 100, -1, "���̊K�֐i�݂܂����H");

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 140, -1, "Enter�Ŏ��̊K��");

	}

	//�V���b�v�ɏ���Ă�ꍇ
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::SHOP && !now_shop_in) {
		shop_ui->MenuDraw();

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 60, -1, "�V���b�v��������");

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 100, -1, "���������܂����H");

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 140, -1, "Enter�Ŕ�������");
	
	}
	
	
}

//------------------------------------------------------------------------------------------------------------
//�v���C���[�̃}�E�X�A�L�[����͂������`�F�b�N
bool DungeonScene::ActiveSkillCheck(const float delta_time)
{
	//1,2,3�L�[�̃N���b�N�ŃL�[�ɂ������G�t�F�N�g���Đ�
	if(sequence_.isStart()) { 
		tnl::DebugTrace("\======================ActiveSkillCheck==========================\n"); 
	}
	

	for (int i = 0; i < 6; i++) {
		if (tnl::Input::IsKeyDownTrigger(static_cast<tnl::Input::eKeys>(skill_key[i])) && drag_set[i]) {
			//�v���C���[�̃X�L�����X�g����ł͂Ȃ����
			if (!SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetSkillList().empty()) {
				//�L�[�ɑΉ������X�L�����擾
				last_skill = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetSkillList()[i];
				sequence_.change(&DungeonScene::SeqPlayerAttack);
				
				return true;
			}
			else {

				sequence_.change(&DungeonScene::SeqPlayerAttack);

				return true;
			}
		}

	}
	//tab�L�[�ŃI�v�V�����E�B���h�E���J��
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_TAB) && now_draw_uiwin == NowDrawUiWindow::NONE) {
		now_draw_uiwin = NowDrawUiWindow::OPTION;
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);

		first_menu->MenuOpen();

	}
	

	//-----------------------------------------------------
	// �A�C�R���̔��a
	const int icon_r = 25;
	// �A�C�R���̍��W��z��ŊǗ�����
	int slot_icon_x[ICON_NUM] = { base_skill_set_pos.x + 45, base_skill_set_pos.x + 45, base_skill_set_pos.x + 45, base_skill_set_pos.x + 45, base_skill_set_pos.x + 45 };
	int slot_icon_y[ICON_NUM] = { base_skill_set_pos.y, base_skill_set_pos.y + 70, base_skill_set_pos.y + 140, base_skill_set_pos.y + 210, base_skill_set_pos.y + 280 };

	// �}�E�X�̍��W�ƃA�C�R���̍��W�̍������a�ȉ��Ȃ�N���b�N���ꂽ�Ɣ��肷��
	for (int i = 0; i < ICON_NUM; i++) {
		//�A�C�R���̖ʐϓ����N���b�N�ŏ����X�L���E�B���h�E�̕\��
		if (abs(mouce_x - slot_icon_x[i]) <= icon_r && abs(mouce_y - slot_icon_y[i]) <= icon_r) {
			if (tnl::Input::IsMouseTrigger(tnl::Input::eMouseTrigger::OUT_RIGHT)) {
				now_draw_uiwin = NowDrawUiWindow::SKILL;
			}
			
		}
		//�X���b�g�A�C�R���ȊO�̏ꏊ���N���b�N�ŃX�L���E�B���h�E������
		if (abs(mouce_x - slot_icon_x[i]) >= icon_r && abs(mouce_y - slot_icon_y[i]) >= icon_r) {
			if (tnl::Input::IsMouseTrigger(tnl::Input::eMouseTrigger::OUT_RIGHT) && now_draw_uiwin == NowDrawUiWindow::SKILL) {
				SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);
				now_draw_uiwin = NowDrawUiWindow::NONE;
			}

		}
	}
	
	

	///---------------------------------------------------------------
	///			�@�@**********drag&drop����************			   ///
	///															   ///
	// �A�C�R���ƃ}�E�X�̋����̓����v�Z����
	for (int i = 0; i < ICON_NUM; i++) {
		dist_sq[i] = (mouce_x - icon_x[i]) * (mouce_x - icon_x[i]) + (mouce_y - icon_y[i]) * (mouce_y - icon_y[i]);
	}
	// �}�E�X�����{�^���ŉ����ꂽ�Ƃ�
	if (tnl::Input::IsMouseDown(tnl::Input::eMouse::LEFT))
	{
		// �}�E�X�������ꂽ���ǂ������L�^
		mouse_down = true;
		dropped = false;
		// �}�E�X���摜�͈͓̔��ŉ����ꂽ�Ƃ�
		for (int i = 0; i < ICON_NUM; i++) {
			if (dist_sq[i] <= icon_r * icon_r) {
				// �}�E�X���h���b�O�����ǂ������L�^
				mouse_drag = true;
				// �N���b�N���ꂽ�A�C�R���̃C���f�b�N�X���L�^
				clicked_icon = i;
				break;
			}
		}
		

	}
	
	// �}�E�X�����{�^���𗣂����Ƃ�
	else if (tnl::Input::IsMouseTrigger(tnl::Input::eMouseTrigger::OUT_LEFT))
	{
		// �}�E�X���h���b�O���łȂ����Ƃ��L�^
		mouse_drag = false;

		// �h���b�v�����A�C�R�����ǂ̃X�L���Z�b�g�ɓ����Ă��邩�𔻒肷��
		for (int i = 0; i < SKILL_SET_NUM; i++) {
			// �X�L���Z�b�g�͈̔͂��v�Z����
			int skill_set_x1 = SKILL_SET_X;
			int skill_set_y1 = SKILL_SET_Y + i * (70); // 10�̓X�L���Z�b�g�Ԃ̗]��
			int skill_set_x2 = skill_set_x1 + SKILL_SET_WIDTH;
			int skill_set_y2 = skill_set_y1 + SKILL_SET_HEIGHT;

			// �A�C�R���̒��S���X�L���Z�b�g�͈͓̔��ɂ��邩�ǂ������`�F�b�N����
			if (abs(mouce_x - slot_icon_x[i]) <= icon_r && abs(mouce_y - slot_icon_y[i]) <= icon_r) {
				//�X���b�g�ɃA�C�R�����Z�b�g���鉹���Đ�
				skill_set_index[i] = icon_handle[clicked_icon];
				SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[14]);

				// �N���b�N���ꂽ�A�C�R���̃C���f�b�N�X��I���ςݔz��Ɋi�[
				SetDragSetIcon(clicked_icon, clicked_icon);
				//�Z�b�g�����X���b�g��ture�ɕύX����
				//i + 1�̓L�[���͂�6����A�X���b�g��5�Ȃ̂Ő������킹�邽��(�ʏ�U���̃L�[���͂����邽�� + 1���Z����,������C��)
				SetDragSet(i + 1, true);
				break;

			}
			

		}
		
		
	}

	//�Z�b�g���ꂽ�X�L�������ׂă��Z�b�g����
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_X)) {
		for (int i = 0; i < ICON_NUM; i++) {
			ResetDragSetIcon(i);
		}
	}
	
	// �}�E�X�J�[�\���̈ʒu����ʂɕ\������
	//DrawFormatString(0, 0, GetColor(255, 255, 255), "X: %d, Y: %d", mouce_x, mouce_y);
	return false;

	
}

//------------------------------------------------------------------------------------------------------------
//�v���C���[�̃L�[���͂ɂ���ē���̃A�j���[�V�������Đ������
bool DungeonScene::SeqPlayerAttack(const float delta_time)
{
	if (sequence_.isStart()) { 
		tnl::DebugTrace("\==========================SeqPlayerAttack=============================\n"); 
	}
		
	if (last_skill != nullptr) {
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SkillAttack(last_skill);
	}
	else {
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->Attack();
	}
	
	if (sequence_.isStart()) {
		//
		tnl::Vector3 player_pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
		//�A�j���[�V�����`����W����
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAnimationPos(player_pos);
		//�U��SE�Đ�
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[6]);
		
		//last_skill��null�łȂ����
		if (last_skill != nullptr) {
			//�Đ��G�t�F�N�g�̑��摜���擾
			int ef_index = last_skill->GetGraphicsAllNum();
			//�`����W�擾
			tnl::Vector3 ef_pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetEffectPos();
			//�C���X�^���X��
			std::shared_ptr<EffectManager> effet = std::make_shared<EffectManager>(last_skill->GetGraphicsVector(), ef_pos, last_skill->GetAnimSpeed(), ef_index);
			//�Đ����X�g�ɃC���X�^���X������effect��ǉ�
			draw_effect_list.emplace_back(effet);
			//�V�[�P���X�ړ�
			sequence_.change(&DungeonScene::SeqCheckDeadEnemy);

		}
		if (draw_effect_list.empty()) {
			sequence_.change(&DungeonScene::SeqCheckDeadEnemy);
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------
//�G���S�`�F�b�N
bool DungeonScene::SeqCheckDeadEnemy(const float delta_time)
{
	for (auto& enemy : SceneTitle::game_manager->GetObjectManager()->GetAliveEnemyList()) {
		//�G�̐������X�g��Hp��0�ȉ��̓G������ꍇ�A���̓G������alivefalg��������
		if (enemy->GetCharaStetus(Character::Stetus::HP) <= 0) {
			//�������x���A�b�v������
			//AddExp�֐��̓��x�����オ���true���Ԃ�
			if (SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->AddExp(enemy->GetCharaStetus(Character::Stetus::EXP))) {
				
				//���x���A�b�vSE
				SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[17]);

				//���x���A�b�v�G�t�F�N�g�`��
				//�C���X�^���X��
				std::shared_ptr<EffectManager> effet = std::make_shared<EffectManager>("using_graphics/effect/level_up.png", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos());
				//�Đ����X�g�ɃC���X�^���X������effect��ǉ�
				draw_effect_list.emplace_back(effet);

			}
			//�G�̃h���b�v�A�C�e���𐶐�
			SceneTitle::game_manager->GetObjectManager()->GenerateDropItem(enemy->GetCharaPos());
			
			enemy->SetIsAlive(false);
		
		}

	}


	if (last_skill != nullptr) {
		last_skill = nullptr;
	}
	
	//HP���O�̓G�𐶑����X�g����폜
	SceneTitle::game_manager->GetObjectManager()->EraseCharaList();

	sequence_.change(&DungeonScene::ActiveSkillCheck);

	return true;
}


//------------------------------------------------------------------------------------------------------------
//�U���\�ȓG�����X�g�Ɋi�[����
bool DungeonScene::SeqActiveEnemyAttack(const float delta_time)
{
	tnl::DebugTrace("\===========================SeqActiveEnemyAttack=================================\n");
	
	if (enemy_sequence_.isStart()) {
		
		
		//�������Ă���G�̐�����
		for (auto& alive_enemy : SceneTitle::game_manager->GetObjectManager()->GetAliveEnemyList()) {

			if (SceneTitle::game_manager->CheckNearByPlayer(alive_enemy)) {

				// std::find���g�p���āAalive_enemy��can_attack_enemy�Ɋ��ɑ��݂��邩���`�F�b�N
				auto it = std::find(can_attack_enemy.begin(), can_attack_enemy.end(), alive_enemy);

				// alive_enemy�����X�g�ɑ��݂��Ȃ��ꍇ�̂݁A���X�g�ɒǉ�����
				if (it == can_attack_enemy.end()) {
					can_attack_enemy.emplace_back(alive_enemy);
				}

			}
		}

	}
	//�G�͍U�����[�V�����Ɉڂ�
	enemy_sequence_.change(&DungeonScene::SeqEnemyAttack);
	
	return true;
}

//------------------------------------------------------------------------------------------------------------
//�����_���ɓG�̍U�������肷��
bool DungeonScene::SeqEnemyAttack(const float delta_time)
{
	tnl::DebugTrace("\n=======================SeqEnemyAttack============================\n");

	//EnemyAttack�Ŋ֐��Ń����_���A���̎��̏�Ԃɂ���Ĕ�������X�L����ύX����
	//EnemyAttack��EnemyAttac�Ō��܂����X�L�����󂯎��A���̃X�L����Animation��߂�l�ŕԂ���悤�ɂ���
	//EnemyAttack�̖߂�l��EffectManager�ɂ���draw_effect_list�ɒǉ����āAEffect�����s����

	if (enemy_sequence_.isStart()) {
	
		//�U���\�ȓG�̐�����
		for (auto& atk_ene : can_attack_enemy) {
			
			//�����_���ȍU����I������
			//std::shared<EffectManager>�^���߂�l�Ƃ��ċA���Ă���
			//�`�惊�X�g�ɒǉ�
			if (!atk_ene->GetIsAttacked()) {
				//damage
				if (last_skill != nullptr) {
					atk_ene->SkillAttack(last_skill);
				}
				else {
					atk_ene->Attack();
				}

				//�U���T�E���h�Đ�
				SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[8]);
				//�U������G�������_���ɍU����I��������ɉ������G�t�F�N�g��Ԃ��B
				//���̂܂܍Đ����X�g�ɒǉ�
				draw_effect_list.emplace_back(atk_ene->EnemyAttack(last_skill, draw_effect_list, atk_ene));

				tnl::DebugTrace("\n===============�U�������I(SeqEnemyAttack)================\n");

				//�U���t���O���X�V
				atk_ene->SetIsAttacked(true);
				
			}

		}
		
	}

	
	//�V�[�P���X�ړ�
	if (draw_effect_list.empty()) {

		//�U���ςݔ���
		CheckIsEnemyAttacked();

		enemy_sequence_.change(&DungeonScene::SeqActiveEnemyAttack);
		
		return true;

	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//���݂̏��ȊO�̃^�C���ɏ���Ă���ꍇ�̓��ꏈ��(�K�i�A���X�Ȃ�etc...)
void DungeonScene::CheckExtraOnTile()
{
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::PASSWAY) {
		return;
	}
	//�����K�i�̏�ɂ�����
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::STAIRS) {

		if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
			SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);

			//�v���C���[�̌��ݍ��W���擾
			tnl::Vector3 pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
			
			player_pos = SceneTitle::game_manager->WorldToLocalPos(pos);
			//�t�F�[�h�A�E�g�Ɉړ�
			ChangeFadeSequence(FadeSequence::FADEOUT);

		}
	}
	//�����V���b�v�̏�ɂ�����
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::SHOP) {

		if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
			//�V���b�v���t���O�𗧂Ă�
			now_shop_in = true;
			//�\��UI��SHOP�ɕύX
			now_draw_uiwin = NowDrawUiWindow::SHOP;
			//SE�Đ�
			SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);
			

		}
	}
}

//------------------------------------------------------------------------------------------------------------
//���̊K�w�Ɉړ����鎞�̏���
void DungeonScene::MoveLevel(int add_level)
{
	//�v���C���[�ȊO�̃I�u�W�F�N�g�����ׂč폜	
	SceneTitle::game_manager->GetObjectManager()->AllListClear(false);
	SceneTitle::game_manager->GetObjectManager()->AllChipListClear();

	//�����蔻��̃y�A���X�g�����ׂč폜����
	SceneTitle::game_manager->GetCollision()->ClearIntersectList();
	SceneTitle::game_manager->GetCollision()->ClearIntersectMap();

	dungeon_level += add_level;

	//�_���W�����Đ���
	SceneTitle::game_manager->DungeonReCreate();
	tnl::DebugTrace("\n======================�_���W�����Đ���======================\n");

	//�I�u�W�F�N�g����
	SceneTitle::game_manager->GetObjectManager()->GenerateOrders(Factory::PlayerSpawn::DUNGEON,true);

	//�K�w�ړ����ɑS�L�����N�^�[����
	SceneTitle::game_manager->GetObjectManager()->RainForceCharaStetus();

	//�����蔻��y�A�쐬
	SceneTitle::game_manager->SetObjectSHitCheck();
	tnl::DebugTrace("\n======================�����蔻��y�A�쐬======================\n");


}

//------------------------------------------------------------------------------------------------------------
//�G���U���ς݂����肵�A�U���ς݂Ȃ�can_attack_enemy����폜����֐�
void DungeonScene::CheckIsEnemyAttacked()
{
	auto atk_ene = can_attack_enemy.begin();
	while (atk_ene != can_attack_enemy.end()) {
		//�G���ƂɎ����Ă���U���I�����肪ture�Ȃ�폜����
		if ((*atk_ene)->GetIsAttacked()) {
			(*atk_ene)->SetIsAttacked(false);
			atk_ene = can_attack_enemy.erase(atk_ene);
			continue;
		}
		atk_ene++;
	}
}

//------------------------------------------------------------------------------------------------------------
// ���t���[�����s����܂�
void DungeonScene::EffectUpdate(float delta_time)
{

	if (draw_effect_list.empty()) {
		return;
	}
	for (auto ef : draw_effect_list) {
		ef->Update(delta_time);
	}
}

//------------------------------------------------------------------------------------------------------------
//�G�t�F�N�g�`��֐�
void DungeonScene::EffectDraw(const hm::Camera& camera)
{
	//�`�惊�X�g����Ȃ珈���X�L�b�v
	if (draw_effect_list.empty()) {
		return;
	}
	auto ef = draw_effect_list.front();
	ef->Draw(camera);

}

//------------------------------------------------------------------------------------------------------------
//�A�j���[�V�����I���t���O�`�F�b�N�֐�
void DungeonScene::CheckEffectIsAlive()
{
	auto ef = draw_effect_list.begin();
	while (ef != draw_effect_list.end()) {
		if (!(*ef)->GetAnimIsAlive()) {
			ef = draw_effect_list.erase(ef);
			continue;
		}
		ef++;
	}
}

//------------------------------------------------------------------------------------------------------------
//first_menu�̃L�[�`�F�b�N�V�[�P���X
bool DungeonScene::SeqFirstMenu(const float delta_time)
{
	//first_menu��0�Ԗڂ�I��������Ԃ�enter���������Ƃ�
	//�C���x���g�����J��
	if (now_draw_uiwin == NowDrawUiWindow::OPTION && first_menu->select_value == 0 && tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		//�I������SE
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);
		first_menu->manage_select_flag = false;
		tnl::DebugTrace("\n=========================INVENTORY=======================\n");
		//MenuSequence��SeqInventoryOpen�Ɉړ�����
		ChangeMenuSequence(MenuSequence::INVENTORY_OPEN);

		return true;
	}

	//first_menu��1�Ԗڂ�I��������Ԃ�enter���������Ƃ�
	//����Ń��j���[�����
	if (first_menu->select_value == 2 && tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);

		first_menu->menu_alive = false;
		now_draw_uiwin = NowDrawUiWindow::NONE;
		tnl::DebugTrace("\n==========================first_menu�����=========================\n");

		//����ȏ�O�̃V�[�P���X���Ȃ��̂Ŏ������g�ɖ߂�
		ChangeMenuSequence(MenuSequence::NONE);
		return false;

	}

	//Esc�L�[�ł�fistmenu������
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_ESCAPE)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);

		first_menu->menu_alive = false;
		now_draw_uiwin = NowDrawUiWindow::NONE;
		tnl::DebugTrace("\n==========================first_menu�����=========================\n");

		//����ȏ�O�̃V�[�P���X���Ȃ��̂Ŏ������g�ɖ߂�
		ChangeMenuSequence(MenuSequence::NONE);
		return false;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//firstmenu�Ŏ��������J�����ꍇ�Ɏ��s
bool DungeonScene::SeqInventoryOpen(const float delta_time)
{
	//�C���x���g���I�[�v���V�[����Esc�L�[�������ƍŏ��̃��j���[�ɕ���
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_ESCAPE)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);

		//first_menu�̃Z���N�g�ړ����\�ɂ���
		first_menu->manage_select_flag = true;
		//now_draw_uiwin = NowDrawUiWindow::OPTION;//����͂����炭����Ȃ��Ȃ�
		ChangeMenuSequence(MenuSequence::FIRSTMENU);
		return true;
	}
	//�C���x���g���J�[�\���ړ�
	ChangeInventory();
	//�C���x���g������Ȃ瑁��return
	//��ԏ�ɏ����ƃC���x���g������鏈�������Ă��܂����炱���ɏ����Ă���
	if (SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->inventory_list.empty())return true;
	//���݂̃J�[�\���ʒu���擾
	int select_value = SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->GetCursorValue();

	//�����C���x���g�����J���Ă���Ƃ���enter�L�[�����͂��ꂽ��
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);
		//���݂̃J�[�\���̈ʒu�̃A�C�e�����擾
		auto item = SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->inventory_list.begin();
		for (int i = 0; i < select_value; i++) {
			item++;
		}

		//�A�C�e����null�Ȃ瑁�����^�[��
		if ((*item) == nullptr)return true;
		//�擾�����A�C�e���̃|�C���^���擾,���ꎞ�ۊ�(�N���X�Ŏg����������)
		item_buff = (*item);
		
		//�I�������A�C�e�����擾
		std::string item_name = item_buff->GetItemName();;

		now_draw_sec_uiwin = NowDrawSecondWindow::ITEMUSE;

		//�g���A��߂�UI��\��������
		use_usable->MenuOpen();
		//�Z���N�g�ł���悤�ɂ���
		use_usable->manage_select_flag = true;
		
		//�A�C�e���g�p�V�[�P���X		
		ChangeMenuSequence(MenuSequence::INVENTORY_USE);

		return true;
		
	}
	return false;
}

bool DungeonScene::SeqInventoryUse(const float delta_time)
{
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_ESCAPE))
	{
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);
		use_usable->manage_select_flag = false;
		use_usable->menu_alive = false;
		//�ЂƂO��menu�ɖ߂�
		ChangeMenuSequence(MenuSequence::INVENTORY_OPEN);
		return true;
	}


	//�g�p����A�C�e���̖��O���擾
	std::string name = item_buff->GetItemName();

	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {

		//�����͂ǂ��ɂ����ꂢ�ɏ�������(�d�v�x : ��)
		//�L�����N�^�[�ɑ΂��ĉe��������A�C�e�����Ƃ�id�Ȃǂŕ�����Ə����X�����ɂȂ邩��
		if (name == "�����S") {
			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "��") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "�h���") {
			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "Gold") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "�_�C�A�����h") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		//(WIP)
		else if (name == "�h���b�v�A�C�e��") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		//(WIP)
		else if (name == "�|�[�V����") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "�U����") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		//�����ɓ����item���������擾�ł��Ă��Ȃ�
		else {
			tnl::DebugTrace("\n�A�C�e���̖��O����O(SeqInventoryUse)\n");
			//�����̊֐��𔲂��Ȃ��悤�ɂ���
			return false;
		}

	}


	return false;
}

//------------------------------------------------------------------------------------------------------------
//�g���A��߂�I��
void DungeonScene::SelectItemUseMenu(int select_value, std::string name)
{
	//�A�C�e����null�Ȃ�return����B����������null�ɂȂ邱�Ƃ͂����炭�Ȃ�(�擾�ɖ�肪���邱�ƂɂȂ�)
	//if (item_buff == nullptr)return;

	//�g����enter���������ꍇ
	if (select_value == 0) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);
		//�A�C�e���̎g�p
		ItemUse(draw_inventory_page,name);
		//�A�C�e�����g�p����UI��\��off
		now_draw_sec_uiwin = NowDrawSecondWindow::NONE;
		use_usable->menu_alive = false;
		//�g�p�����A�C�e����null�ɂ���
		item_buff = nullptr;
		//���j���[�V�[�P���X�������A�C�e���ꗗ�ɂ���
		ChangeMenuSequence(MenuSequence::INVENTORY_OPEN);
	}
	//��߂��enter���������ꍇ
	else if (select_value == 1) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);
		use_usable->menu_alive = false;
		item_buff = nullptr;
		//���j���[�V�[�P���X�������A�C�e���ꗗ�ɂ���
		ChangeMenuSequence(MenuSequence::INVENTORY_OPEN);
	}

}

//------------------------------------------------------------------------------------------------------------
//�A�C�e�����g�p����֐�
void DungeonScene::ItemUse(int inventory_page,std::string name)
{
	//�A�C�e����null�Ȃ�return����B����������null�ɂȂ邱�Ƃ͂����炭�Ȃ�(�擾�ɖ�肪���邱�ƂɂȂ�)
	if (item_buff == nullptr) {
		tnl::DebugTrace("\n=====================Item���g�p�ł��܂���B(ItemUse)============================\n");
		return;
	}

	//�v���C���[�̌���HP���擾
	int now_player_hp = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::HP);
	//�v���C���[�̍ő�HP���擾
	int max_player_hp = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetPlayerMaxHp();
	//�ő�HP�ƌ���HP�̍����擾
	int difference = max_player_hp - now_player_hp;

	//�g�p�����A�C�e���ɂ���ĕύX����X�e�[�^�X���������򂷂�(�����͂��������H�v������)
	if (name == "�����S") {
		//�񕜗�
		int move_hp = item_buff->GetItemStetusEfficacy();
		
		//�v���C���[��HP���ő�HP�ȉ��Ȃ�
		if (max_player_hp > now_player_hp) {
			//�񕜗ʂ̂ق����������
			if (move_hp > difference){
				//�񕜗ʂ������ɂ���(HP�����^���ɂȂ�)
				move_hp = difference;
			}
			//�X�e�[�^�X�ϓ�
			SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::HP, move_hp);
			SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
			IsInventoryDelete();

		}
		
	}
	if (name == "��") {
		int move_hp = item_buff->GetItemStetusEfficacy();
		//�v���C���[��HP���ő�HP�ȉ��Ȃ�
		if (max_player_hp > now_player_hp) {
			//�񕜗ʂ̂ق����������
			if (move_hp > difference) {
				//�񕜗ʂ������ɂ���(HP�����^���ɂȂ�)
				move_hp = difference;
			}
			SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::HP, move_hp);
			SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
			IsInventoryDelete();
		}
		
	}
	if (name == "�h���") {
		int move_defence = item_buff->GetItemStetusEfficacy();
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::DEF, move_defence);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
	if (name == "�_�C�A�����h") {
		int move_diamond = item_buff->GetItemStetusEfficacy();
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::DIAMOND,move_diamond);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
	if (name == "Gold") {
		int move_gold = item_buff->GetItemStetusEfficacy();
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::GOLD, move_gold);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
	//(WIP)
	if (name == "�h���b�v�A�C�e��") {
		//�����_���ŃA�C�e���̖��O���擾����
		std::string item_name = SceneTitle::game_manager->GetObjectManager()->GenerateShopItem();
		//�C���x���g���ɃA�C�e����ǉ�
		SceneTitle::game_manager->AddItemToInventory(item_name, SceneTitle::game_manager->GetInventorys(), shop_page);
		
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
	//(WIP)
	if (name == "�|�[�V����") {
		//�񕜗�
		int move_potion_hp = item_buff->GetItemStetusEfficacy();
		//�U���͏㏸��
		int move_potion_atk = item_buff->GetItemStetusEfficacy();
		//�h��͏㏸��
		int move_potion_def = item_buff->GetItemStetusEfficacy();

		//�v���C���[��HP���ő�HP�ȉ��Ȃ�
		if (max_player_hp > now_player_hp) {
			//�񕜗ʂ̂ق����������
			if (move_potion_hp > difference) {
				//�񕜗ʂ������ɂ���(HP�����^���ɂȂ�)
				move_potion_hp = difference;
			}
			//HP�X�e�[�^�X�ϓ�
			SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::HP, move_potion_hp);
		}
		//�U���͕ϓ�
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::ATK, move_potion_atk);
		//�h��͏㏸
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::DEF, move_potion_def);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();

	}
	if (name == "�U����") {
		int move_potion = item_buff->GetItemStetusEfficacy();
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::ATK, move_potion);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
}

//------------------------------------------------------------------------------------------------------------
bool DungeonScene::IsInventoryDelete()
{
	//�y�[�W���̃A�C�e�������ׂď���ăy�[�W�����������ゾ������
	if (SceneTitle::game_manager->GetIsDeleteInventotry()) {
		//�y�[�W���ŏ��̃y�[�W���������߂�
		if (draw_inventory_page == 0) {

			SceneTitle::game_manager->SetIsDeleteInventory(false);
		}
		else if (draw_inventory_page == SceneTitle::game_manager->GetInventoryValue() + 1) {
			draw_inventory_page--;
		}

	}

	return SceneTitle::game_manager->GetIsDeleteInventotry();
}

//------------------------------------------------------------------------------------------------------------
//�C���x���g�����̕`��
void DungeonScene::DrawInventory(int x, int y)
{
	if (SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->inventory_list.empty())return;
	//�����T�C�Y�ύX
	SetFontSize(25);
	SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->DrawInventory(x, y);

	//�V���b�v�E�B���h�E�ł̓C���x���g�����̃A�C�e���f�[�^�͕\�����Ȃ�
	if (now_draw_uiwin != NowDrawUiWindow::SHOP) {
		//�C���x���g���y�[�W����`��
		DrawStringEx(x + 300, y + 10, -1, "�y�[�W:%d", SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->GetInventoryValue());
		//�A�C�e�������`��
		desc->MenuDraw();
		SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->DrawItemDesc(desc->menu_x + 10, desc->menu_y + 10);
		//��������
	}
	SetFontSize(16);

}

//------------------------------------------------------------------------------------------------------------
//�C���x���g���̕ʃy�[�W�Ɉړ�����֐�
void DungeonScene::ChangeInventory()
{

	//�J�[�\���̏㉺�ړ�
	SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->CursorMove();

	//�C���x���g����1�����Ȃ�return
	if (SceneTitle::game_manager->GetInventoryValue() == 0)return;
	//�C���x���g����؂�ւ���
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RIGHT)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);

		//���y�[�W��+1�Ŋ������]������߂�B�Ō�̃y�[�W����ŏ��̃y�[�W�ɖ߂邱�Ƃ��ł���
		draw_inventory_page = (draw_inventory_page + 1) % (SceneTitle::game_manager->GetInventoryValue() + 1);
		//�C���x���g����؂�ւ��Ă����Ƃ��ɃJ�[�\������ԏ�Ƀ��Z�b�g����
		SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->CursorReset();
	}
	//���L�[�������ꂽ�Ƃ�
	else if (tnl::Input::IsKeyDownTrigger(eKeys::KB_LEFT)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);

		//���y�[�W��+1�Ŋ������]������߂�B�ŏ��̃y�[�W����Ō�̃y�[�W�ɖ߂邱�Ƃ��ł���
		draw_inventory_page = (draw_inventory_page + (SceneTitle::game_manager->GetInventoryValue())) % (SceneTitle::game_manager->GetInventoryValue() + 1);
		//�C���x���g����؂�ւ��Ă����Ƃ��ɃJ�[�\������ԏ�Ƀ��Z�b�g����
		SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->CursorReset();
	}
	if (SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->inventory_list.empty())return;
}

//------------------------------------------------------------------------------------------------------------
//�A�C�e���擾����
bool DungeonScene::DetectItem()
{
	//�A�C�e���Ƃ̓����蔻��
	for (auto item : SceneTitle::game_manager->GetObjectManager()->GetAliveItem()) {
		if (item->DeleteCheckOnPlayer(player_pos)) {
			SceneTitle::game_manager->AddItemToInventory(item->GetItemName(), SceneTitle::game_manager->GetInventorys(), SceneTitle::game_manager->GetInventoryValue());
			item->SetIsAliveFalse();
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//�V���b�v�ŃA�C�e�����w������
//���̃V�[�P���X���ł���΁A���̑��̏����͈ꎞ�X�g�b�v
bool DungeonScene::SeqBuyInStores(const float delta_time)
{
	//1�t���[�����s
	if (sequence_.isStart()) {
		
		//���݂̃}�b�v�Ŕ����������Ă����瑁�����^�[��
		if (SceneTitle::game_manager->GetDoneBuyInStores()) {

			now_draw_uiwin = NowDrawUiWindow::NONE;
			now_shop_in = false;

			return true;
		}

		//�V���b�v�y�[�W���Ȃ����
		if (shop_pages.empty()) {
			//�C���X�^���X��
			Inventory* new_inventory = new Inventory(shop_page + 1, SceneTitle::game_manager);

			//�C���X�^���X�������V���b�v�y�[�W���i�[
			shop_pages.emplace_back(new_inventory);
		
		}
		
		//�w��񐔃V���b�v�C���x���g���ɃA�C�e���������_���ɒǉ�
		SetShopItem(5);

	}

	//ESC�L�[�ŃV���b�v�����
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_ESCAPE)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);

		//�V���b�v���p�t���O���X�V
		SceneTitle::game_manager->SetDoneBuyInStores();

		//�V���b�v�C���x���g���̃J�[�\�������Z�b�g
		shop_pages[shop_page]->CursorReset();

		now_draw_uiwin = NowDrawUiWindow::NONE;
		now_shop_in = false;

		return true;
	}

	//�y�[�W���̃A�C�e������Ȃ�return
	if (shop_pages[draw_shop_page]->inventory_list.empty()) {
		
		now_draw_uiwin = NowDrawUiWindow::NONE;
		now_shop_in = false;

		return true;
	}

	//�V���b�v�A�C�e�����̑I���ړ�
	shop_pages[draw_shop_page]->CursorMove();

	//============================================================================
	// Enter���������Ƃ�
	//
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);

		//�J�[�\���̈ʒu���擾
		int cursor_value = shop_pages[draw_shop_page]->GetCursorValue();
	
		//�\�����̃C���x���g���̐擪�̃A�C�e�����擾
		auto now_item = shop_pages[draw_shop_page]->inventory_list.begin();
		
		//�I�����ꂽ�A�C�e���܂ŃC�e���[�^�[�ړ�
		for (int i = 0; i < cursor_value; i++) {
			now_item++;
		}
		//�A�C�e���̒l�i
		int item_price = 0;
			
		//�A�C�e���̉��i���擾
		item_price = now_item->get()->GetItemPrice();
	
		//������������Ȃ��ꍇ
		//���݂̏����������i�ȉ��Ȃ�
		if (SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::GOLD) < item_price) {
			
			tnl::DebugTrace("\n==================������������Ȃ�================\n");
			
			now_draw_uiwin = NowDrawUiWindow::NONE;
			now_shop_in = false;

			return true;
		}

		//�v���C���[�C���x���g���ɃA�C�e����ǉ�
		SceneTitle::game_manager->AddItemFromShop((*now_item));
		
		//�Q�ƃJ�E���^�����Z�b�g
		//�V���b�v�ɕ���ł���I�������A�C�e�����폜
		//�J�[�\���ʒu�𒲐�
		(*now_item).reset();
		now_item = shop_pages[draw_shop_page]->inventory_list.erase(now_item);
		shop_pages[draw_shop_page]->SetCursorValue(-1);

		//�������̍X�V
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::GOLD, -item_price);
	
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
//�V���b�v�ɃA�C�e�����Z�b�g����֐�
void DungeonScene::SetShopItem(int set_value)
{
	//�w��񐔎��s
	for (int i = 0; i < set_value; i++) {

		//�����_���ŃA�C�e���̖��O���擾����
		std::string item_name = SceneTitle::game_manager->GetObjectManager()->GenerateShopItem();

		//�V���b�v�C���x���g���ɃA�C�e����ǉ�
		SceneTitle::game_manager->AddItemToInventory(item_name, shop_pages, shop_page);

	}
}

//------------------------------------------------------------------------------------------------------------
bool DungeonScene::SeqFadeIn(const float delta_time)
{
	if (fade_control->GetDoneFade()) {
		fade_control->FadeIn();
		return true;
	}
	else {
		ChangeFadeSequence(FadeSequence::NONE);
		return true;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool DungeonScene::SeqFadeOut(const float delta_time)
{
	if (!fade_control->GetDoneFade()) {
		fade_control->FadeOut();
		return true;
	}
	else {
		MoveLevel(1);
		ChangeFadeSequence(FadeSequence::FADEDESC);
		return true;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------
//�K�w�ړ����ɕ�����`�悷��V�[�P���X
bool DungeonScene::SeqDescFade(const float delta_time)
{
	//�����`�掞�Ԃ��I���܂Ńt�F�[�h�A�E�g���n�߂Ȃ�

	if (fade_control->GetDoneFade() && desc_fade_count < DESCFADETIME) {
		fade_control->FadeIn();
		return true;
	}

	//�����`�掞�Ԃ̍X�V
	desc_fade_count += delta_time;

	if (desc_fade_count < DESCFADETIME) {
		return true;
	}

	if (!fade_control->GetDoneFade()) {
		fade_control->FadeOut();
		return true;
	}
	else {
		ChangeFadeSequence(FadeSequence::FADEIN);
		desc_fade_count = 0;
		return true;
	}

	return true;

}

//------------------------------------------------------------------------------------------------------------
//�K�w�ړ����̃t�F�[�h���ɕ�����`�悷��
void DungeonScene::DrawFadeDesc()
{
	DrawStringEx((DXE_WINDOW_WIDTH / 2) - 100, (DXE_WINDOW_HEIGHT / 2) - 50, -1, "�_���W����%d�K", dungeon_level);
}

//------------------------------------------------------------------------------------------------------------
void DungeonScene::ChangeMainSequence(float delta_time)
{
	//�v���C���[�V�[�P���X
	sequence_.update(delta_time);
	//�G�V�[�P���X
	enemy_sequence_.update(delta_time);
	//���j���[�V�[�P���X
	menu_seq_.update(delta_time);
	//�t�F�[�h�V�[�P���X
	fade_seq_.update(delta_time);

	//�V���b�v�ɓ����Ă���΃V���b�v�V�[�P���X�̂ݏ���
	if (now_shop_in && now_draw_uiwin == DungeonScene::NowDrawUiWindow::SHOP) {
		sequence_.change(&DungeonScene::SeqBuyInStores);
	}
	//�V���b�v�łȂ���΂��̑��̃V�[�P���X����
	else {
		//�v���C���[�̃L�[�`�F�b�N
		ActiveSkillCheck(delta_time);
		//�G�̃X�L�������`�F�b�N
		SeqActiveEnemyAttack(delta_time);
	}
	
}

//------------------------------------------------------------------------------------------------------------
//���j���[�V�[�P���X��ύX����֐�
void DungeonScene::ChangeMenuSequence(MenuSequence next_menu)
{
	now_menu_type = next_menu;
	//menu�����ɉ����I������Ă��Ȃ���΂��̂܂ܑ������^�[��
	if (next_menu == MenuSequence::NONE)return;
	//tab�L�[�������Ƃ���ɂȂ�
	if (next_menu == MenuSequence::FIRSTMENU) {
		menu_seq_.change(&DungeonScene::SeqFirstMenu);
	}
	//tab->������enter�ł���
	else if (next_menu == MenuSequence::INVENTORY_OPEN) {
		menu_seq_.change(&DungeonScene::SeqInventoryOpen);
	}
	//tab->������enter->item�g���ł���
	else if (next_menu == MenuSequence::INVENTORY_USE) {
		menu_seq_.change(&DungeonScene::SeqInventoryUse);
	}

}

//------------------------------------------------------------------------------------------------------------
void DungeonScene::ChangeFadeSequence(FadeSequence next_fade)
{
	fade_sequence_type = next_fade;
		
	if (next_fade == FadeSequence::FADEIN) {
		fade_seq_.change(&DungeonScene::SeqFadeIn);
	}
	else if (next_fade == FadeSequence::FADEOUT) {
		fade_seq_.change(&DungeonScene::SeqFadeOut);
	}
	else if (next_fade == FadeSequence::FADEDESC) {
		fade_seq_.change(&DungeonScene::SeqDescFade);
	}
	else if (next_fade == FadeSequence::NONE) {
		tnl::DebugTrace("\nFadeNone\n");
	}
}


