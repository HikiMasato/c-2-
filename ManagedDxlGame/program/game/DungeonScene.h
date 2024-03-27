///*****************Discription*******************
///�_���W�������ł̃V�[�P���X�����N���X
/// 
/// 
///*********************************************** 
#pragma once
#include "MyCamera.h"
class ObjectManager;
class Charactor;
class Enemy;
class Skill;
class EffectManager;
class hm::Camera;
class FadeControl;
class Menu;
class MenuWindow;
class Item;
class Inventory;

class DungeonScene {
public:

	enum class NowWindoowType {
		NONE,
		OPTION,

	};

	DungeonScene();
	~DungeonScene();
	

	

	//********************�ϐ�************************
	
	

	//�v���C���[�V�[�P���X�ϐ�
	tnl::Sequence<DungeonScene> sequence_ = tnl::Sequence<DungeonScene>(this, &DungeonScene::ActiveSkillCheck);
	//�G�V�[�P���X�ϐ�
	tnl::Sequence<DungeonScene> enemy_sequence_ = tnl::Sequence<DungeonScene>(this, &DungeonScene::SeqActiveEnemyAttack);
	//���j���[�V�[�P���X�ϐ�
	tnl::Sequence<DungeonScene> menu_seq_ = tnl::Sequence<DungeonScene>(this, &DungeonScene::SeqFirstMenu);
	//�t�F�[�h�V�[�P���X�ϐ�
	tnl::Sequence<DungeonScene> fade_seq_ = tnl::Sequence<DungeonScene>(this, &DungeonScene::SeqFadeIn);
	
	//********************�֐�************************

	void Update(float delta_time);
	void Draw();
	 
	//UI�֘A�̕`��
	void UIDraw();

	//�_���W�����V�[���̏�����
	void InitDungeonScene();

	//���ݓ���ł���^�C���ɂ���ĕ\��������
	void CheckExtraOnTile();
	void MoveLevel(int add_level);

	//�G���U���ς݂����肵�A�U���ς݂Ȃ�can_attack_enemy����폜����֐�
	void CheckIsEnemyAttacked();

	//effect�`��֌W
	void EffectUpdate(float delta_time);
	void EffectDraw(const hm::Camera& camera);
	//�A�j���[�V�����I���t���O�`�F�b�N�֐�
	void CheckEffectIsAlive();


	//--------------------------��{�V�[�P���X----------------------------------------------
	// �v���C���[�̃L�[�`�F�b�N�V�[�P���X
	// �v���C���[�̃L�[�ɂ���Ĕ��������X�L���ɂ����effect���ꎞ�I�ɐ������čĐ�
	// 
	// �G�̍U���������_���Ɍ���
	// �����_���Ɍ��肳�ꂽ�G�̔����X�L���ɂ����effect���ꎞ�I�ɐ������čĐ�
	// 
	//--------------------------------------------------------------------------------------
	//�v���C���[���X�L�������L�[����͂������`�F�b�N
	bool ActiveSkillCheck(const float delta_time);
	//�v���C���[�̃L�[���͂ɂ���ē���̃A�j���[�V�������Đ������
	bool SeqPlayerAttack(const float delta_time);
	//�G���S�`�F�b�N�֐�
	bool SeqCheckDeadEnemy(const float delta_time);
	
	//�U���\�ȓG�����X�g�Ɋi�[����
	bool SeqActiveEnemyAttack(const float delta_time);
	//�����_���ɓG�̍U�������肷��
	bool SeqEnemyAttack(const float delta_time);	

	//************MenuSequence*************
	
	bool SeqFirstMenu(const float delta_time);
	bool SeqInventoryOpen(const float delta_time);
	bool SeqInventoryUse(const float delta_time);


	//************ShopSequence*************
	
	//�V���b�v�ŃA�C�e�����w������
	//���̃V�[�P���X���ł���΁A���̑��̏����͈ꎞ�X�g�b�v
	bool SeqBuyInStores(const float delta_time);

	//�V���b�v�ɃA�C�e�����Z�b�g����֐�
	void SetShopItem(int set_value);


	//*************FadeSequence*************

	//�t�F�[�h�C���V�[�N�G���X
	bool SeqFadeIn(const float delta_time);
	//�t�F�[�h�A�E�g�V�[�N�G���X
	bool SeqFadeOut(const float delta_time);
	//�G���A�ړ����̕����t�F�[�h�V�[�N�G���X
	bool SeqDescFade(const float delta_time);
	//�K�w�ړ����̃t�F�[�h���ɕ�����`�悷��
	void DrawFadeDesc();

private:


	//���݂̕`����
	enum class NowDrawUiWindow {
		NONE,
		OPTION,
		SKILL,
		INVENTORY,
		SHOP,
	};

	//�h�����Ă���UI
	enum class NowDrawSecondWindow {
		NONE,
		ITEMUSE
	};

	//Menu�V�[�P���X
	enum class MenuSequence {
		NONE,
		FIRSTMENU,
		INVENTORY_OPEN,
		INVENTORY_USE
	};

	enum class FadeSequence {
		NONE,
		FADEIN,
		FADEOUT,
		FADEDESC
	};

	//Ui�錾*******************************************
	MenuWindow* use_usable = nullptr;
	MenuWindow* first_menu = nullptr;
	Menu* alway_menu = nullptr;
	
	//��ʉ���UI
	Menu* stetus_ui = nullptr;
	//�������Ȃǂ�������Ă���UI
	Menu* gold_ui = nullptr;
	//�X�L���Z�b�gUI
	Menu* skill_ui = nullptr;
	Menu* have_skill_ui = nullptr;
	Menu* option = nullptr;
	Menu* inventory = nullptr;
	Menu* desc = nullptr;

	//�_���W�����K�i�ɏ������\��
	Menu* dungeon_level_ui = nullptr;
	//�V���b�v�ɓ���UI
	Menu* shop_ui = nullptr;
	//�V���b�v�C���x���g��UI
	Menu* shop_inve_ui = nullptr;
	//�V���b�v���R�C��UI
	Menu* shop_coin_ui = nullptr;
	//�V���b�v�����g�̃A�C�e��UI
	Menu* shop_my_inve_ui = nullptr;

	std::list<Menu*>alway_is_ui;
	


	//*************************�ϐ�**************************************
	 
	NowDrawSecondWindow now_draw_sec_uiwin = NowDrawSecondWindow::NONE;
	NowDrawUiWindow now_draw_uiwin = NowDrawUiWindow::NONE;
	MenuSequence now_menu_type = MenuSequence::NONE;
	FadeSequence fade_sequence_type = FadeSequence::FADEOUT;

	std::shared_ptr<FadeControl> fade_control = nullptr;
	//�f���^�^�C�������Z����
	float desc_fade_count = 0;
	//�t�F�[�h���̕�����`�悷�鎞��
	const float DESCFADETIME = 3;

	//�_���W�����̃��x��
	int dungeon_level = 1;//�Q�b�^�[�����ق�����������

	Skill* last_skill = nullptr;

	//�`�悳����effectlist
	std::list<std::shared_ptr<EffectManager>> draw_effect_list;

	//�G�V�[�P���X�`�F���W�C���^�[�o��
	const int ENEMYATKINTERVAL = 20;
	int enemy_atk_interval = ENEMYATKINTERVAL;

	//���ݕ`�撆�̃V���b�v�y�[�W
	int draw_shop_page = 0;
	//���݂̃V���b�v�y�[�W��
	int shop_page = 0;
	//���݃V���b�v�ɓ����Ă���t���O
	bool now_shop_in = false;

	//�X�L�������L�[(�A�C�R�����}�E�X�ŃN���b�N���邱�ƂŔ����ł���悤�ɂ�����\��)
	int skill_key[6] = { 
		(int)tnl::Input::eKeys::KB_1,
		(int)tnl::Input::eKeys::KB_2,
		(int)tnl::Input::eKeys::KB_3,
		(int)tnl::Input::eKeys::KB_4, 
		(int)tnl::Input::eKeys::KB_5,
		(int)tnl::Input::eKeys::KB_6 
	};

	//�U���\�ȓG���i�[����
	std::list<std::shared_ptr<Enemy>>can_attack_enemy;
	std::list<std::shared_ptr<Enemy>>::iterator ene_itr;

	//icon�����X�g�Ɋi�[
	std::vector<int>icon_list;
	//goldui�ɏ��������Ȃǂ�z�u����Ƃ��̃x�[�X�̃|�W�V����
	//(���̍��W�ɉ��Z���ĕt�߂ɔz�u������̃|�W�V����������ł���)
	tnl::Vector3 base_gold_icon_pos = { 1075,30,0 };
	//�X�L���Z�b�gUI�|�W�V����
	//�t�߂�UI�Ȃǂ̔z�u�͂��̃|�W�V�������x�[�X�ɉ��Z���Č��肷��
	tnl::Vector3 base_skill_set_pos = { 1185,400,0 };

	tnl::Vector3 hpbar_postion = { 720,600,0 };

	tnl::Vector3 player_pos = { 0, 0, 0 };

	//********drag&drop�����ϐ�***************
	

	//�}�E�X�̃|�W�V����
	int mouce_x = 0;
	int mouce_y = 0;

	//�}�E�X�h���b�O�t���O
	bool mouse_drag = false;
	//�}�E�X�̃N���b�N�t���O
	bool mouse_down = false;
	//�A�C�R���h���b�v�t���O
	bool dropped = false;

	// �A�C�R���̐�
	const static int ICON_NUM = 5;
	// �A�C�R���̃T�C�Y
	const int ICON_WIDTH = 50;
	const int ICON_HEIGHT = 50;
	// �A�C�R���̏����ʒu
	const int ICON_X = base_skill_set_pos.x - 450;
	const int ICON_Y = base_skill_set_pos.y - 50;
	// �A�C�R���̊Ԋu
	const int ICON_SPACING = 20;
	// �A�C�R���̍��W
	int icon_x[ICON_NUM];
	int icon_y[ICON_NUM];
	// �A�C�R���ƃ}�E�X�̋����̓��
	int dist_sq[ICON_NUM];
	// �A�C�R���̃n���h��
	std::vector<int>icon_handle;
	//�X�L���X���b�g�ɒu���ꂽ�X�L�����i�[
	std::vector<int>skill_set_index;
	// �N���b�N���ꂽ�A�C�R���̃C���f�b�N�X
	int clicked_icon = -1;
	//�N���b�N���ăZ�b�g�����X�L���������X�L���ꗗ����������߂̑I���ς݃C���f�b�N�X�ۑ���
	std::vector<int>clicked_set_icon;
	int clicked_set_icon_[ICON_NUM] = { -1, -1, -1, -1, -1 };
	
	// �X�L���Z�b�g�̍��W�ƃT�C�Y���`����
	// �X�L���Z�b�g�̍����x���W
	const int SKILL_SET_X = base_skill_set_pos.x + 45;  
	// �X�L���Z�b�g�̍����y���W
	const int SKILL_SET_Y = base_skill_set_pos.y;	
	// �X�L���Z�b�g�̕�
	const int SKILL_SET_WIDTH = ICON_WIDTH; 
	// �X�L���Z�b�g�̍���
	const int SKILL_SET_HEIGHT = ICON_HEIGHT;
	// �X�L���Z�b�g�̐�
	const int SKILL_SET_NUM = 5; 

	// �h���b�O���̃X�L���ƁA�h���b�O���ăX���b�g�ɃZ�b�g�����X�L���̃C���f�b�N�X���L�^����z��
	int drag_set_icon[ICON_NUM] = { -1,-1,-1,-1,-1 };

	//�X���b�g���ɃX�L�����Z�b�g����Ă���t���O
	bool drag_set[ICON_NUM + 1] = { true, false, false, false, false, false };


	//***************�C���x���g���g�p�ϐ�******************

	//�A�C�e�����E���t���O
	bool item_get_flag = true;
	//�V���b�v�̃A�C�e���y�[�W��
	int shop_page_value = 0;
	int draw_inventory_page = 0;
	//�V���b�v�̃y�[�W�����Ă����z��
	std::vector<Inventory*>shop_pages;
	//�C���x���g�����őI�������A�C�e�����ꎞ�I�ɕۑ�����
	std::shared_ptr<Item> item_buff = nullptr;


	//************************�֐�******************************
	// �h���b�O���̃X�L���ƁA�h���b�O���ăX���b�g�ɃZ�b�g�����X�L���̃C���f�b�N�X���L�^����֐�
	void SetDragSetIcon(int drag_index, int set_index) {
		drag_set_icon[drag_index] = set_index;
	}

	// �h���b�O���̃X�L���ƁA�h���b�O���ăX���b�g�ɃZ�b�g�����X�L���̃C���f�b�N�X�����Z�b�g����֐�
	void ResetDragSetIcon(int drag_index) {
		clicked_icon = -1;
		drag_set_icon[drag_index] = -1;
		skill_set_index[drag_index] = 0;
		clicked_set_icon[drag_index] = 0;
		drag_set[drag_index + 1] = false;
		
	}

	//�����ɓn���ꂽ�C���f�b�N�X�̗v�f�̃X�L���Z�b�g�t���O���X�V����
	void SetDragSet(int drag_index, bool is_set) {
		drag_set[drag_index] = is_set;
	}


	//�C���x���g���I�[�v�����̏���
	//void InventoryOpen();
	//�C���x���g���I�[�v�����Ɏg�p�A�C�e�����g�p�����Ƃ��̊m�F
	//void InventoryItemUse(std::string name);
	//�I�������A�C�e��������Ďg���֐�
	void SelectItemUseMenu(int select_value,std::string name);
	void ItemUse(int inventory_page, std::string name);
	//�C���x���g�����폜����
	bool IsInventoryDelete();
	//�C���x���g�����̃A�C�e����`��
	void DrawInventory(int x, int y);
	//�C���x���g���̕ʃy�[�W�Ɉړ�����֐�
	void ChangeInventory();
	//�A�C�e���擾����
	bool DetectItem();

	//���C���V�[�P���X��ύX����֐�
	void ChangeMainSequence(float delta_time);
	//���j���[�V�[�P���X��ύX����֐�
	void ChangeMenuSequence(MenuSequence next_menu);
	//�t�F�[�h�V�[�P���X��ύX����֐�
	void ChangeFadeSequence(FadeSequence next_fade);
};