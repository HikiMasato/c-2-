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
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "SceneManager.h"
#include "ObjectManager.h"
#include "ResourceManager.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "SkillManager.h"
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
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//others
#include "Stetus.h"
#include "Debug.h"
#include "Skill.h"
#include "Inventory.h"


//------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
GameManager::GameManager() {
	
	//=================�C���X�^���X��============================
	re_manager = std::make_shared<ResourceManager>(this);
	sound_manager = std::make_shared<SoundManager>(re_manager->sound_handle);
	//object����
	object_manager = std::make_shared<ObjectManager>(re_manager->chara_stetus, re_manager->effect_graphics, re_manager->item_handle, this);

	debug_manager = new DebugManager();
	collision = std::make_shared<Collision>();

}

GameManager::~GameManager()
{	
	if (!inventories.empty()) {
		for (int i = 0; i < inventories.size(); ++i) {
			delete inventories[i];
		}
	}
	InitGraph();
}

//------------------------------------------------------------------------------------------------------------
//���t���[�����s
void GameManager::update(float delta_time) {

	object_manager->Update(delta_time);
}

//------------------------------------------------------------------------------------------------------------
//�`��֐�
void GameManager::Draw() {
	//�S�̂̃}�b�v��`��
	map_manager->MapDraw(object_manager->factory->camera, now_gh_size, ratio);
	//�~�j�}�b�v�̕`��
	map_manager->MiniMapDraw(object_manager->factory->camera);
	
	object_manager->Draw();

	
}
 
//------------------------------------------------------------------------------------------------------------
//GameManager������������
void GameManager::InitGameManager() {
	SRand(time(0));
	//�}�b�v�`�b�v�����[�h
	re_manager->LoadMapChipCsv();

	//�`��T�C�Y�ύX
	ScaleChange();
	ModifyScalePos();

	inventory = new Inventory(0,this);
	inventories.emplace_back(inventory);
	bankinventory = new Inventory(0,this);
	bank_inventroy_list.emplace_back(bankinventory);
	
	
	//�_���W��������
	GenerateDungeon(DungeonType::TOWER);
	
}

//------------------------------------------------------------------------------------------------------------
//�_���W�����Đ���(���̊K�w�ɍs���Ƃ���)
void GameManager::DungeonReCreate()
{
	if (map_manager != nullptr) {
		map_manager.reset();
	}
	waypoint.clear();
	//�_���W�����𐶐��J�n
	GenerateDungeon(nowdungeon);
}

//------------------------------------------------------------------------------------------------------------
//�V���b�v�̃A�C�e�����v���C���[�̃C���x���g���ɒǉ�����֐�
void GameManager::AddItemFromShop(std::shared_ptr<Item> shop_item)
{
	//���̃C���x���g���̎��A�C�e���z�񂪍ő�Ȃ�
	if (inventories[inventory_value]->inventory_list.size() >= 10) {
		Inventory* new_inventory = new Inventory(inventory_value + 1, this);
		//inventory�z��ɒǉ�����
		inventories.emplace_back(new_inventory);
		//�o�^�C���x���g�����X�V
		inventory_value++;
	}

	//�A�C�e���̖��O���擾
	std::string item_name = shop_item->GetItemName();

	//�擾�����A�C�e���̖��O�̃A�C�e�����擾
	std::shared_ptr<Item> new_item = object_manager->GetItemManager()->GetItemData(item_name);
	
	//�v���C���[�C���x���g���ɃA�C�e����ǉ�
	inventories[inventory_value]->AddInventory(new_item);

	tnl::DebugTrace("\n===================�V���b�v����C���x���g���ɒǉ�=========================\n");
}

//------------------------------------------------------------------------------------------------------------
//�C���x���g���ɃA�C�e����ǉ�����
void GameManager::AddItemToInventory(const std::string name, std::vector<Inventory*>& inventories, int& inventory_value)
{
	//���̃C���x���g���̎��A�C�e���z�񂪍ő�Ȃ�
	if (inventories[inventory_value]->inventory_list.size() >= 10) {
		Inventory* new_inventory = new Inventory(inventory_value + 1,this);
		//inventory�z��ɒǉ�����
		inventories.emplace_back(new_inventory);
		//�o�^�C���x���g�����X�V
		inventory_value++;
	}

	//�}�b�v���Ŏ擾�������O�ƈ�v����A�C�e�����擾
	std::shared_ptr<Item> item = object_manager->GetItemManager()->GetItemData(name);

	inventories[inventory_value]->AddInventory(item);

	tnl::DebugTrace("\n===================�}�b�v���炩��C���x���g���ɒǉ�=========================\n");

}

//------------------------------------------------------------------------------------------------------------
//�C���x���g������A�C�e�����폜����֐�(�C���x���g���A�C�e����O�ɋl�߂鏈�������Ă���)
void GameManager::EraseItemFromInventory(const int now_inventory_id)
{
	//���݂̈ʒu�̔z��ԍ����擾
	int select_value = inventories[now_inventory_id]->GetCursorValue();
	//�\�����̃C���x���g�����擾
	auto now_inventory = inventories[now_inventory_id]->inventory_list.begin();

	//�I�����ꂽ�A�C�e���܂ŃC�e���[�^�ړ�
	for (int i = 0; i < select_value; i++) {
		now_inventory++;
	}

	now_inventory = inventories[now_inventory_id]->inventory_list.erase(now_inventory);
	//�I�𒆂̃A�C�e���͏������Ă���̂ŃJ�[�\�������Ɉړ�
	inventories[now_inventory_id]->SetCursorValue(-1);

	if (now_inventory_id != inventory_value) {
		int check_inv_value = now_inventory_id;
		while (1) {

			if (inventories[check_inv_value + 1]->inventory_list.empty())break;
			//���̃y�[�W�̍ŏ��̃A�C�e�����R�s�[���ď������A�C�e���𖖔��ɒǉ�����
			auto item = inventories[check_inv_value + 1]->inventory_list.begin();
			//�A�C�e����ǉ�
			inventories[check_inv_value]->inventory_list.emplace_back((*item));
			//���̃y�[�W�̍ŏ��̃A�C�e����pop����
			inventories[check_inv_value]->inventory_list.pop_front();
			//�C���x���g���̍Ō�̃y�[�W�ɂ��ǂ蒅������break.
			if (check_inv_value + 1 == inventory_value)break;
			check_inv_value++;
		}
	}
	//�ŏ��̃C���x���g���Ȃ�
	else {
		//�C���x���g�����̃A�C�e�����P���炷
		//����ȏ�O�̃y�[�W���Ȃ�����
		inventories[now_inventory_id]->SetItemValue(-1);
	}
	//�����A�C�e�����C���x���g�����폜����
	if (inventories[now_inventory_id]->inventory_list.empty()) {
		if (inventory_value != 0) {
			delete inventories[now_inventory_id];
			inventories[inventory_value] = nullptr;
			inventories.pop_back();
			//�C���x���g����������炷
			inventory_value--;
			//�폜����
			is_delete_invantory = true;

		}
	}
	//�C���x���g���̍폜�t���O�������Ă�����return
	if (is_delete_invantory)return;
	//�J�[�\���̈ʒu����ԏ�Ƀ��Z�b�g
	if (inventories[now_inventory_id]->inventory_list.empty())inventories[now_inventory_id]->CursorReset();

}

//------------------------------------------------------------------------------------------------------------
//����̃A�C�e���𐶑����X�g����폜����
bool GameManager::DetectDropItem(std::shared_ptr<Item> item, std::list<std::shared_ptr<Item>>& it_list)
{
	bool erase = false;
	for (auto item_itr = it_list.begin(); item_itr != it_list.end();) {
		//��v������̂�����΍폜����
		if ((*item_itr) == item) {
			item_itr = it_list.erase(item_itr);
			erase = true;
			break;
		}
		else {
			item_itr++;
		}
	}
	return erase;
}

//------------------------------------------------------------------------------------------------------------
bool GameManager::GetDoneBuyInStores() const
{
	return map_manager->GetDoneBuy();
}

//------------------------------------------------------------------------------------------------------------
//�_���W�������Ƃ̉摜���擾����
std::vector<int>& GameManager::GetGraphicsHandles(DungeonType dungeontype) const 
{
	//�����ɂ���ĉ摜���擾����
	return re_manager->dungeon_handls[std::underlying_type<DungeonType>::type(dungeontype)];
}
//------------------------------------------------------------------------------------------------------------
//�}�b�v�`�b�v�̏����擾����֐�
int GameManager::GetMapChip(tnl::Vector3 mappos) const
{
	return map_manager->GetChip(mappos.x,mappos.y);
}

//------------------------------------------------------------------------------------------------------------
//�_���W�����̐���(�v���C���[�̐������s���Ă���)
void GameManager::GenerateDungeon(DungeonType dungeontype) {
	
	//MapManager��delete����Ă��Ȃ���Έ�xdelete����(Map���ꂩ���蒼��)
	if (map_manager != nullptr) {
		map_manager.reset();
	}

	waypoint.clear();
	
	//�����œn�����_���W�����^�C�v�����݂̃_���W�����ɕύX����
	nowdungeon = dungeontype;
	debug_manager->DebugString("\n======================�_���W��������======================\n");
	
	//Map�̐������J�n
	GenerateDungeonMap(dungeontype);
	debug_manager->DebugString("\n======================�_���W���������I��======================\n");
	
	
	//�}�b�v��������
	map_manager->AreaDivideStart(MAPWIDTH, MAPHEIGHT, map_manager);
	debug_manager->DebugString("\n======================�����I��======================\n");

	
	//�K�i�̃}�b�v���W���擾
	tnl::Vector3 stairpos = SetStartPosition(SetStartPositionType::STAIR);
	debug_manager->DebugString("\n======================�K�i���W�擾����======================\n");
	
	
	//�K�i�ݒu
	map_manager->ResetChip(stairpos.x, stairpos.y, object_manager->factory->GetMapChip()->STAIRS);
	
	//debug�p
	map_manager->AddStairList(stairpos);
	

	//�V���b�v�������_���Őݒu
	map_manager->SetShop();
	debug_manager->DebugString("\n======================�V���b�v�ݒu======================\n");


	//�}�b�v�`�b�v�N���X�C���X�^���X��
	object_manager->GenerateOrdersToMapChip();
	debug_manager->DebugString("\n======================�}�b�v�`�b�v�C���X�^���X������=======================\n");


	//�v���C���[�����Ȃ���Έȉ��̏����͂��Ȃ�
	if (object_manager->factory->GetPlayer() == nullptr) {
		return;
	}
	//�|�W�V����������
	object_manager->factory->GetPlayer()->SetCharaPos(SetStartPosition(SetStartPositionType::PLAYER));
	//�|�W�V�������擾
	tnl::Vector3 player_pos = object_manager->factory->GetPlayer()->GetCharaPos();
	
	map_manager->ChangeRoomVisit(WorldToLocalPos(player_pos));
	debug_manager->DebugString("\n======================�|�W�V�����o�^����======================\n");


	CameraReset();
}

//------------------------------------------------------------------------------------------------------------
//�_���W�����}�b�v�̐���
void GameManager::GenerateDungeonMap(DungeonType dungeontype)
{
	//int�^��vector�Ɋi�[
	std::vector<int>gf_handles = GetGraphicsHandles(dungeontype);
	//Map�̃C���X�^���X��(�����ɂ́AMap�̕��ƍ������w��,�摜�n���h��)
	map_manager = std::make_shared<MapManager>(MAPWIDTH, MAPHEIGHT, gf_handles,this);
}

//------------------------------------------------------------------------------------------------------------
//����̍��W�ɓG�����邩�ǂ������`�F�b�N����
bool GameManager::CheckIsThereEnemy(tnl::Vector3 pos)
{
	bool ene_is_there = false;
	for (auto enemy : object_manager->GetAliveEnemyList()) {
		tnl::Vector3 enemy_pos = WorldToLocalPos(enemy->GetCharaPos());
		//�����ɓn���ꂽ�G�̃|�W�V�����ƓG�̐������X�g�Ɋi�[����Ă���G�̃|�W�V�����������ꍇ�t���O�𗧂Ă�
		if (enemy_pos.x == pos.x && enemy_pos.y == pos.y) {
			ene_is_there = true;
			break;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
std::shared_ptr<Enemy> GameManager::GetIsThereEnemyToDir(tnl::Vector3 pos)
{
	std::shared_ptr<Enemy>there_enemy = nullptr;

	for (auto enemy : object_manager->GetAliveEnemyList()) {
		//�G�̃|�W�V�������擾
		tnl::Vector3 get_enemy_pos = enemy->GetCharaPos();
		tnl::Vector3 enemy_pos = WorldToLocalPos(get_enemy_pos);
		//�����̍��W�ƈ�v����ꍇ
		if (enemy_pos.x == pos.x && enemy_pos.y == pos.y) {
			there_enemy = enemy;
			break;
		}
	}

	return there_enemy;
}

//------------------------------------------------------------------------------------------------------------
//����̓G�ƃv���C���[���אڂ��Ă��邩�`�F�b�N
bool GameManager::CheckNearByPlayer(std::shared_ptr<Enemy> enemy)
{
	//enemyPos�̓}�b�v���W
	tnl::Vector3 enemy_pos = WorldToLocalPos(enemy->GetCharaPos());

	tnl::Vector3 down;
	tnl::Vector3 left;
	tnl::Vector3 right;
	tnl::Vector3 up;

	if (enemy_pos.y > 0)		up = enemy_pos + GetFirstDirVector((int)Character::MoveDir::UP);
	if (enemy_pos.x < MAPWIDTH)	right = enemy_pos + GetFirstDirVector((int)Character::MoveDir::RIGHT);
	if (enemy_pos.y < MAPHEIGHT)down = enemy_pos + GetFirstDirVector((int)Character::MoveDir::DOWN);
	if (enemy_pos.x > 0)		left = enemy_pos + GetFirstDirVector((int)Character::MoveDir::LEFT);

	tnl::Vector3 enemyPosNear[4] = { left,up,right,down };

	tnl::Vector3 playerPos = WorldToLocalPos(object_manager->factory->GetPlayer()->GetCharaPos());

	bool isNear = false;
	for (int i = 0; i < 4; ++i) {
		if (playerPos.x == enemyPosNear[i].x && playerPos.y == enemyPosNear[i].y) {
			isNear = true;
			break;
		}
	}
	return isNear;
	return false;

}
//------------------------------------------------------------------------------------------------------------
void GameManager::CheckIsDeadCharactor(std::shared_ptr<Character> object, tnl::Vector3 pos)
{
	tnl::Vector3 dead_pos;
	//dir�����̃|�W�V�������Ƃ�
	dead_pos = WorldToLocalPos(object->GetCharaPos()) + GetFirstDirVector((int)object->GetCharaDir());


}

//------------------------------------------------------------------------------------------------------------
void GameManager::SetDoneBuyInStores() const
{
	map_manager->SetDoneBuy();
}

//------------------------------------------------------------------------------------------------------------
//�I�u�W�F�N�g���m�̓����蔻��̏�������ɂ���֐�
void GameManager::SetObjectSHitCheck() {


	//------------------------------------------------------------------------------------------
	//�v���C���[�ƓG�̃y�A���쐬
	collision->RegistIntersectedProcess<Player, Enemy>(
		[](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {

			//�Q�ƃJ�E���^�[��ۂ����܂܃|�C���^�̕ϊ����s���Ă����
			std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(a);
			std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(b);

			if (&Collision::IsIntersectAABB) {
				DebugManager::DebugString("\n=========�v���C���[�ƓG���Փ�==========\n");
			}
		});

	for (auto& enemy : object_manager->GetAliveEnemyList()) {
		collision->RegistPairObject<Player, Enemy>(object_manager->factory->GetPlayer(), enemy);
	}


	//------------------------------------------------------------------------------------------
	//�G���m�̓����蔻��̃y�A���쐬
	collision->RegistIntersectedProcess<Enemy, Enemy>(
		[](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {

			//�Q�ƃJ�E���^�[��ۂ����܂܃|�C���^�̕ϊ����s���Ă����
			std::shared_ptr<Enemy> enemy_first = std::dynamic_pointer_cast<Enemy>(a);
			std::shared_ptr<Enemy> enemy_second = std::dynamic_pointer_cast<Enemy>(b);

			if (&Collision::IsIntersectAABB) {
				DebugManager::DebugString("\n===========�G�ƓG���Փ�=============\n");
			}
		});

	//�O���̃��[�v���R���e�i�̍ŏ��̗v�f����n�܂�A�����̃��[�v���O���̃��[�v�̌��݂̗v�f�̎��̗v�f����n�߂�
	//����ɂ��A�e�y�A����x������������邱�Ƃ��ۏ؂����
	//std::next(it1) ���g�p���邱�ƂŁAit1 �� it2 �������I�u�W�F�N�g���w�����Ƃ͂Ȃ�
	for (auto it1 = object_manager->GetAliveEnemyList().begin(); it1 != object_manager->GetAliveEnemyList().end(); ++it1) {
		for (auto it2 = std::next(it1); it2 != object_manager->GetAliveEnemyList().end(); ++it2) {
			collision->RegistPairObject<Enemy, Enemy>(*it2, *it1);
		}
	}


	//------------------------------------------------------------------------------------------
	//�v���C���[�ƕǂ̓����蔻��̃y�A���쐬
	collision->RegistIntersectedProcess<Player, MapChip>(
		[](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {

			//�Q�ƃJ�E���^�[��ۂ����܂܃|�C���^�̕ϊ����s���Ă����
			std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(a);
			std::shared_ptr<MapChip> mapchip = std::dynamic_pointer_cast<MapChip>(b);

			if (&Collision::IsIntersectAABB) {
				DebugManager::DebugString("\n============�v���C���[�ƕǂ��Փ�===============\n");
			}
		});

	for (auto& mapchip : object_manager->GetWallMapChipList()) {
		collision->RegistPairObject<Player, MapChip>(object_manager->factory->GetPlayer(), mapchip);
	}


	//------------------------------------------------------------------------------------------
	//�G�ƕǂ̓����蔻��̃y�A���쐬
	collision->RegistIntersectedProcess<Enemy, MapChip>(
		[](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {

			//�Q�ƃJ�E���^�[��ۂ����܂܃|�C���^�̕ϊ����s���Ă����
			std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(b);
			std::shared_ptr<MapChip> mapchip = std::dynamic_pointer_cast<MapChip>(a);

			if (&Collision::IsIntersectAABB) {
				DebugManager::DebugString("\n=============�G�ƕǂ��Փ�=============\n");
			}
		});

	for (auto it_enemy = object_manager->GetAliveEnemyList().begin(); it_enemy != object_manager->GetAliveEnemyList().end(); ++it_enemy) {
		for (auto it_wall = object_manager->GetWallMapChipList().begin(); it_wall != object_manager->GetWallMapChipList().end(); ++it_wall) {
			collision->RegistPairObject<Enemy, MapChip>(*it_enemy, *it_wall);
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//�y�A�ɂ���Object���ǂ��������������Ă��邩�𔻒�
void GameManager::UpdateHitCheck()
{

	//�v���C���[�̓����蔻��
	for (auto& pl_en : collision->GetIntersectList()) {

		//IsIntersectAABB��bool�^
		//�v���C���[�ƓG�̓����蔻��
		if (collision->IsIntersectAABB(pl_en.object_a->GetCharaPos(), tnl::Vector3(pl_en.object_a->GetCharaChipSize()),
			pl_en.object_b->GetCharaPos(), tnl::Vector3(pl_en.object_b->GetCharaChipSize())))

		  
		{ // li->a_ �� li->b_ �̏Փ˔���
			pl_en.intersected_call(pl_en.object_a, pl_en.object_b);

			tnl::CorrectPositionRect(pl_en.object_a->GetCharaPos(), 
									 pl_en.object_b->GetCharaPos(), 
									 tnl::Vector2i(pl_en.object_a->GetCharaChipSize(), pl_en.object_a->GetCharaChipSize()),
									 tnl::Vector2i(pl_en.object_b->GetCharaChipSize(), pl_en.object_b->GetCharaChipSize()), 
									 pl_en.object_a->SetCharaPosToCollistion(),
									 pl_en.object_b->SetCharaPosToCollistion(),
									 tnl::eCorrTypeRect::BOTH, 
									 tnl::eCorrTypeRect::BOTH);

		}

	}

	//�G���m�̓����蔻��`�F�b�N
	for (auto& enemy : collision->GetIntersectList()) {
		
		//IsIntersectAABB��bool�^
		if (collision->IsIntersectAABB(enemy.object_a->GetCharaPos(), tnl::Vector3(enemy.object_a->GetCharaChipSize()),
			enemy.object_b->GetCharaPos(), tnl::Vector3(enemy.object_b->GetCharaChipSize())))


		{ // enemy �� enemy �̏Փ˔���
			enemy.intersected_call(enemy.object_a, enemy.object_b);

			tnl::CorrectPositionRect(enemy.object_a->GetCharaPos(),
									 enemy.object_b->GetCharaPos(),
									 tnl::Vector2i(enemy.object_a->GetCharaChipSize(), enemy.object_a->GetCharaChipSize()),
									 tnl::Vector2i(enemy.object_b->GetCharaChipSize(), enemy.object_b->GetCharaChipSize()),
									 enemy.object_a->SetCharaPosToCollistion(),
									 enemy.object_b->SetCharaPosToCollistion(),
									 tnl::eCorrTypeRect::BOTH,
									 tnl::eCorrTypeRect::BOTH);

		}
	}


	//�v���C���[�̓����蔻��
	for (auto& pl_en : collision->GetIntersectList()) {

		//IsIntersectAABB��bool�^
		//�v���C���[�ƕǂ̓����蔻��`�F�b�N
		if (collision->IsIntersectAABB(pl_en.object_a->GetCharaPos(), tnl::Vector3(pl_en.object_a->GetCharaChipSize()),
			pl_en.object_b->GetOldChipPos(), tnl::Vector3(pl_en.object_b->GetMapChipSize())))


		{ // li->a_ �� li->b_ �̏Փ˔���
			pl_en.intersected_call(pl_en.object_a, pl_en.object_b);

			tnl::CorrectPositionRect(pl_en.object_a->GetCharaPos(),
									 pl_en.object_b->GetOldChipPos(),
									 tnl::Vector2i(pl_en.object_a->GetCharaChipSize(), pl_en.object_a->GetCharaChipSize()),
									 tnl::Vector2i(pl_en.object_b->GetMapChipSize(), pl_en.object_b->GetMapChipSize()),
									 pl_en.object_a->SetCharaPosToCollistion(),
									 pl_en.object_b->SetOldMapChipPos(),
									 tnl::eCorrTypeRect::PWRFL_B,
									 tnl::eCorrTypeRect::PWRFL_B);

		}

	}


	//�G�̓����蔻��
	for (auto& pl_en : collision->GetIntersectList()) {

		//IsIntersectAABB��bool�^
		//�G�ƕǂ̓����蔻��`�F�b�N
		if (collision->IsIntersectAABB(pl_en.object_a->GetCharaPos(), tnl::Vector3(pl_en.object_a->GetCharaChipSize()),
			pl_en.object_b->GetOldChipPos(), tnl::Vector3(pl_en.object_b->GetMapChipSize())))


		{ // li->a_ �� li->b_ �̏Փ˔���
			pl_en.intersected_call(pl_en.object_a, pl_en.object_b);

			tnl::CorrectPositionRect(pl_en.object_a->GetCharaPos(),
									 pl_en.object_b->GetOldChipPos(),
									 tnl::Vector2i(pl_en.object_a->GetCharaChipSize(), pl_en.object_a->GetCharaChipSize()),
									 tnl::Vector2i(pl_en.object_b->GetMapChipSize(), pl_en.object_b->GetMapChipSize()),
									 pl_en.object_a->SetCharaPosToCollistion(),
									 pl_en.object_b->SetOldMapChipPos(),
									 tnl::eCorrTypeRect::PWRFL_B,
									 tnl::eCorrTypeRect::PWRFL_B);

		}

	}
}

//------------------------------------------------------------------------------------------------------------
//HP��0�̃L�����N�^�[�̓����蔻����폜����
void GameManager::EraseHitList()
{
	//�擪�C�e���[�^�[���擾
	auto character = collision->GetIntersectList().begin();
	
	while (character != collision->GetIntersectList().end())
	{
		//�ǂ��炩�̃I�u�W�F�N�g�̐����t���O������Ă����ꍇ
		if (!character->object_a->GetIsAlive() || !character->object_b->GetIsAlive()) {
			//���̃y�A���폜
			character = collision->GetIntersectList().erase(character);
		}
		else {
			character++;
		}
		
	}
}

//------------------------------------------------------------------------------------------------------------
//�`����W����}�b�v���W�ɕϊ�
tnl::Vector3 GameManager::WorldToLocalPos(tnl::Vector3 pos) {

	return WorldToMap(pos.x, pos.y);
}

//------------------------------------------------------------------------------------------------------------
//�}�b�v���W����`����W�ɕϊ�
tnl::Vector3 GameManager::LocalToWorldPos(int map_x, int map_y) {

	return MapToWorld(map_x, map_y);
}

//------------------------------------------------------------------------------------------------------------
tnl::Vector3 GameManager::WorldToMap(const int world_x, const int world_y)
{

	int map_x = world_x / now_gh_size;
	int map_y = world_y / now_gh_size;

	return tnl::Vector3(map_x, map_y, 0);
}

//------------------------------------------------------------------------------------------------------------
tnl::Vector3 GameManager::MapToWorld(const int map_x, const int map_y)
{
	
	int world_x = map_x * now_gh_size;
	int world_y = map_y * now_gh_size;

	return tnl::Vector3(world_x, world_y, 0);
}

//------------------------------------------------------------------------------------------------------------
//�t���C�E�F�C�g�p�^�[�����̗p����LoadGraph(std::string_view�͎��̂������Ȃ�������,�|�C���^�ƒ�������������)
int GameManager::LoadGraphEx(std::string_view graph) {

	auto it = map_graph.find(graph);
	if (it != map_graph.end()) {
		return map_graph[graph];
	}
	else {
		int existing_gh = LoadGraph(graph.data());
		map_graph.insert(std::make_pair(graph, existing_gh));
	}

	return map_graph[graph];
}

//------------------------------------------------------------------------------------------------------------
int GameManager::CheckIsThere(tnl::Vector3 pos)
{
	return map_manager->CheckIsThere(pos.x,pos.y);
}

//------------------------------------------------------------------------------------------------------------  
//��ʂ̊g��k����ύX����֐�
void GameManager::ScaleChange()
{
	tnl::Vector3 pos = object_manager->factory->GetPlayer()->GetCharaPos();
	//�v���C���[�̃��[�J�����W���擾
	player_pos_buff = WorldToLocalPos(pos);
	//���̕`����@���ʏ�{���Ȃ�
	if (now_scale == ScaleMode::NOMAL) {
		//�`����@���g��ɂ���
		now_scale = ScaleMode::ENLAEGE;
	}
	//���̕`�悪�g��{���Ȃ�
	else {
		now_scale = ScaleMode::NOMAL;
	}
	//�`��`�b�v�̃T�C�Y��ύX����
	now_gh_size = scale[std::underlying_type<ScaleMode>::type(now_scale)];
	//�`��{����ύX
	ratio = now_gh_size / GRAPHICSIZE;

}

//------------------------------------------------------------------------------------------------------------  
//�X�P�[���`�F���W���̃|�W�V�����C��
void GameManager::ModifyScalePos()
{
	object_manager->factory->GetPlayer()->SetCharaPos(LocalToWorldPos(player_pos_buff.x, player_pos_buff.y));

	CameraReset();
}

//------------------------------------------------------------------------------------------------------------  
//MiniMap���`��ł��邩����
bool GameManager::CanDrawMiniMap(tnl::Vector3 localpos) {
	return map_manager->CheckCanDraw(localpos);
}

//------------------------------------------------------------------------------------------------------------
//MiniMap�̍X�V
void GameManager::RefreshMiniMap(tnl::Vector3 localpos) {

	map_manager->ChangeRoomVisit(localpos);
	map_manager->ChangeWayVisit(localpos);
}

//------------------------------------------------------------------------------------------------------------
//MiniMap��blend���Ă���`��
void GameManager::DrawBlendedMiniMap() {

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	map_manager->MiniMapDraw(object_manager->factory->camera);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

//------------------------------------------------------------------------------------------------------------
//�����œn���ꂽ�����̐���1�𑫂����l���Awaypoint�x�N�^�[�̃T�C�Y�ɐݒ�
void GameManager::InitWayPointVector(const int initroomnum) {
	//�����œn���ꂽ�����̐���1�𑫂����l���Awaypoint�x�N�^�[�̃T�C�Y�ɐݒ�
	//����́A�����̔ԍ���0����n�܂邽��
	//waypoint�x�N�^�[�̊e�v�f�́A���̕����ɓ��B���邽�߂̍ŒZ�o�H��\���E�F�C�|�C���g�̃��X�g
	waypoint.resize(initroomnum + 1);
}

//------------------------------------------------------------------------------------------------------------
//�ʘH�������ɁA�����ԍ��ƁA�|�W�V������vector�ɒǉ�����֐�
void GameManager::SetRoomWayPoint(const tnl::Vector3 pos, const int roomid) {
	
	waypoint[roomid].emplace_back(pos);
}

//------------------------------------------------------------------------------------------------------------
//�����ɓn���ꂽ�����ԍ������W�����ԉ����o�����擾����֐�
tnl::Vector3 GameManager::GetFarPoint(int room_id, tnl::Vector3 pos)
{
	//room_id����o���̍��W���擾����
	std::vector<tnl::Vector3>out_point;
	for (int i = 0; i < waypoint[room_id].size(); i++) {
		out_point.emplace_back(waypoint[room_id][i]);
	}

	std::vector<int>distances_list;

	//out_point�̍��W��pos�̍��������߂āAdistance_list�ɒǉ�����
	for (auto point : out_point) {
		int dis_x = point.x - pos.x;
		int dis_y = point.y - pos.y;
		int distance = dis_x * dis_x + dis_y * dis_y;
		distances_list.emplace_back(distance);
	}

	//distances�̓Y�����ԍ���out_point�̓Y���ԍ��͈�v
	//�ő勗������out_point�̔ԍ����擾����
	 
	//�ő勗���̔z��ԍ���ێ�����ϐ�
	int max = 0;
	//��r�l���Ȃ��ꍇ�̈ꎞ�ۊǗp�ϐ�
	int buff = 0;

	for (int i = 0; i < distances_list.size(); i++) {
		//buff�ɒl�������ĂȂ���Ύ������g������
		if(buff == 0)buff = distances_list[i];
		if (buff < distances_list[i]) {
			//�ő勗���z��ԍ�
			max = i;
		}
	}
	//��ԉ���out_point��Ԃ�
	return out_point[max];
}

//------------------------------------------------------------------------------------------------------------
//�v���C���[�̕����ԍ���ύX����
void GameManager::SetPlayerRoomValue(int nowroomvalue)
{	
	//�L�����̕����ԍ��̕ύX��̒l���Z�b�g����
	object_manager->factory->GetPlayer()->SetNowRoomValue(nowroomvalue);
}

//------------------------------------------------------------------------------------------------------------
//�����_���ȍ��W��ݒ肷��֐�
tnl::Vector3 GameManager::SetStartPosition(SetStartPositionType type) {
	
	//�����_���ȕ����ԍ����擾
	int random_room = rand() % (map_manager->GetRoomNum());
	//�����ԍ����畔�����擾
	std::vector<int> room = map_manager->GetRoom(random_room);
	//�����̒��̃����_���ȃ}�b�v���W���擾
	int type_x = GetRandValue(room[0], room[2]);
	int type_y = GetRandValue(room[1], room[3]);
	//�G�̐����̏ꍇ(����h���`�F�b�N)
	//�v���C���[�̃|�W�V�����Ɣ��̂�h��
	if (type == SetStartPositionType::ENEMY) {
		int player_x = object_manager->factory->GetPlayer()->GetCharaPos().x;
		int player_y = object_manager->factory->GetPlayer()->GetCharaPos().y;
		int debagcount = 0;
		while (1)
		{
			//�v���C���[�ƓG�̍��W���d�Ȃ�Ȃ��Ȃ�܂Ń��[�v
			if (player_x != type_x && player_y != type_y)break; {
				type_x = GetRandValue(room[0], room[2]);
				type_y = GetRandValue(room[1], room[3]);
				debagcount++;
				tnl::DebugTrace("\n================���W����%d���===================\n", debagcount);

			}
		}
					
	}
	//�K�i�̏ꍇ
	if (type == SetStartPositionType::STAIR)return tnl::Vector3( type_x,type_y,0 );

	//�����_���ɐݒ肵�����W��`����W�ɕϊ�
	tnl::Vector3 newpos = MapToWorld(type_x, type_y);

	return newpos;
}

//------------------------------------------------------------------------------------------------------------
//�����Z���k�E�c�C�X�^�[(�����ɍŏ��ő�l��^���A���̊Ԃ��烉���_���ɒl��Ԃ�)
int GameManager::GetRandValue(int a, int b) {

	//std::mt19937�́A�����Z���k�E�c�C�X�^�[�Ƃ����A���S���Y�����g���āA���i���ȋ[�������𐶐����闐��������
	//����������(mt19937�Ƃ�������������I�u�W�F�N�g�����A
	//random_device�Ƃ����̂��񌈒�I�ȗ������������瓾���l����Ƃ���mt�ɓn��)
	std::mt19937 mt(std::random_device{}());


	if (b > a or a == b) {

		// ���z�I�u�W�F�N�g(�����������ŏo���l�̍ŏ��l�A�ő�l�����߂�
		std::uniform_int_distribution<int> dist(a, b);
		// �����_���Ȓl
		int random = dist(mt);

		return random;
	}
	else {

		// ���z�I�u�W�F�N�g(�����������ŏo���l�̍ŏ��l�A�ő�l�����߂�
		std::uniform_int_distribution<int> dist(b, a);
		// �����_���Ȓl
		int random = dist(mt);

		return random;
	}

}

//------------------------------------------------------------------------------------------------------------
//�X�L���ȊO�̃_���[�W��^����֐�
float GameManager::CalculationDamage(int atk, int def)
{
	float calc_value = (float)GetRandValue(-5, 8);
	float damage = atk / std::pow(2.5f, (def / 10));

	//0�ȉ��Ȃ�
	if (damage < 0)damage = 0;

	return damage;
}

//------------------------------------------------------------------------------------------------------------
void GameManager::InflictDamageToTarget(Character* object, tnl::Vector3 pos)
{
	//�v���C���[�̏ꍇ
	if (object->GetObjectType() == Object::ObjectType::PLAYER) {
		//���ׂĂ̓G����O�ɂ���G���擾
		std::shared_ptr<Enemy>front_enemy = GetIsThereEnemyToDir(pos);
		//��U��
		if (front_enemy == nullptr) {
			return;
		}
		//�U���͂Ɩh��͂��擾���Čv�Z����
		float damage = CalculationDamage(object_manager->factory->GetPlayer()->GetCharaStetus(Character::Stetus::HP), front_enemy->GetCharaStetus(Character::Stetus::DEF));
		//�_���[�W���f
		AffectDamage(damage, front_enemy);

	}
	else {
		float damage = CalculationDamage(object->GetCharaStetus(Character::Stetus::ATK), object_manager->factory->GetPlayer()->GetCharaStetus(Character::Stetus::DEF));

		//�_���[�W���f
		AffectDamage(damage, object_manager->factory->GetPlayer());
	}


}

//------------------------------------------------------------------------------------------------------------
//�^�[�Q�b�g�Ƀ_���[�W��^����֐�(�X�L���̏ꍇ)
void GameManager::CalculationSkillDamageToTarget(Character* object, const tnl::Vector3 pos, Skill* now_use_skill)
{
	//�X�L���̐��l�f�[�^���擾
	const float* skill_data = now_use_skill->GetSkillFloat();
	//�X�L�������擾
	std::string skill_name = now_use_skill->GetSkillName();

	//�v���C���[���U������Ȃ�
	if (object->GetObjectType() == Object::ObjectType::PLAYER) {
		//���ׂĂ̓G����O�ɂ���G���擾
		std::shared_ptr<Enemy>front_enemy = GetIsThereEnemyToDir(pos);
		//������U��Ȃ�
		if (front_enemy == nullptr) {
			return;
		}
		float damage = CalculationSkillDamage(skill_data[0], object_manager->factory->GetPlayer()->GetCharaStetus(Character::Stetus::ATK), front_enemy->GetCharaStetus(Character::Stetus::DEF));

		//���ۂ�hp�ɔ��f������
		AffectDamage(damage, front_enemy);
	}
	else {
		float damage = CalculationSkillDamage(skill_data[0], object->GetCharaStetus(Character::Stetus::ATK), object_manager->factory->GetPlayer()->GetCharaStetus(Character::Stetus::DEF));
		//���ۂ�hp�ɔ��f������
		AffectDamage(damage, object_manager->factory->GetPlayer());
	}
}
//------------------------------------------------------------------------------------------------------------
float GameManager::CalculationSkillDamage(const float skill_rate, const int atk, const int def)
{
	float skill_damage = atk * skill_rate;

	return CalculationDamage(skill_damage, def);
}

//------------------------------------------------------------------------------------------------------------
//���ۂɃX�e�[�^�X�ɉe��������֐�
void GameManager::AffectDamage(float damage, std::shared_ptr<Character> object)
{
	object->SetAffectToCharaStetus(Character::Stetus::HP, -damage);
}

//------------------------------------------------------------------------------------------------------------
//�J�����̈ʒu���v���C���[�̈ʒu�ɍ��킹��
void GameManager::CameraReset()
{
	if(tnl::Input::IsKeyDownTrigger(eKeys::KB_6))
	{ object_manager->factory->camera.Zoom(); }

	//�J�����������񂱂�����
	tnl::Vector3 pos = object_manager->factory->GetPlayer()->GetCharaPos();

	//�J�������v���C���[�ɍ��킹��
	object_manager->factory->camera.cameraPos = pos - tnl::Vector3(DXE_WINDOW_WIDTH >> 1, DXE_WINDOW_HEIGHT >> 1, 0);

	
#if 0
	//�f�o�b�O
	tnl::DebugTrace("\nplayer:(%f,%f),pos:(%f,%f),camera:(%f,%f)(CameraReset)\n",
		//�v���C���[�̃|�W�V����
		object_manager->factory->GetPlayer()->GetCharaPos().x, object_manager->factory->GetPlayer()->GetCharaPos().y,
		//�擾�����v���C���[�̃|�W�V����(�����͏�L�̃v���C���[�̍��W�Ɠ����ɂȂ��Ă���͂�)
		pos.x, pos.y,
		//�J�������W
		object_manager->factory->camera.cameraPos.x, object_manager->factory->camera.cameraPos.y);

#endif // 0


}
