///******************Description***********************
///�I�u�W�F�N�g���Ǘ�����N���X
///Factory�N���X�ɐ������󒍂���
///�C���X�^���X�����ꂽ�I�u�W�F�N�g���i�[����Ă��郊�X�g��ێ����Ă��� 
///(���̎��ɃX�e�[�^�X�N���X���琶������I�u�W�F�N�g�̃f�[�^���󂯎�肻�̃f�[�^�ƂƂ��Ɏ󒍂���)
///GameManager�N���X�ɂ͂��̃N���X���C���X�^���X������΂��ׂẴI�u�W�F�N�g��`��A�Ȃǂ��ꊇ�ōs����
///**************************************************** 
#pragma once
#include "MyCamera.h"
#include "Factory.h"
#include "Skill.h"
#include "SkillManager.h"

class GameManager;
class ItemManager;

class Factory;
class Object;
class Character;
class Stetus;

class hm::Camera;
class Enemy;
class Item;

class SkillManager;
class Skill;

class ObjectManager {
public:
	ObjectManager(std::vector<std::vector<std::string>>& re_stetus, std::vector<std::vector<std::string>>& effect_csv, std::vector<std::vector<std::string>>re_item, GameManager* game_manager_ = nullptr);
	~ObjectManager();

	


	//*****************�ϐ��錾******************
	std::shared_ptr<Factory>factory = nullptr;


	//�L�����N�^�[�ȊO�̃I�u�W�F�N�g���i�[����Ă���(Item�Ȃ�etc...)
	std::list<std::shared_ptr<Object>>object_list;
	//�L�����N�^�[�I�u�W�F�N�g���i�[����Ă���
	std::list<std::shared_ptr<Character>>charactor_list;
	
	

	//�G�̃X�e�[�^�X�Ɩ��O�A�摜�n���h�������vector�ɂ܂Ƃ߂�
	std::vector<std::pair<std::vector<int>, std::vector<std::string>>> chara_info;

	//*******************�֐�********************
	void Update(const float delta_time);
	void Draw();

	//Factory�N���X�ɐ������󒍂���֐�(������Factory���s�����߂����ł̓X�e�[�^�X��n���̂Ǝ󒍂̂ݍs��)
	//arg1�c���݂̃X�|�[���^�C�v(STARTMAP�̏ꍇ : �v���C���[�̂ݐ����ADUNGEON�̏ꍇ : �v���C���[�A�A�C�e���A�G����)
	//arg2�c�K�w�ړ��̏ꍇ�ɌĂяo���ꍇ : false(�v���C���[�̐����ȊO���s��)�Atrue(���ׂẴI�u�W�F�N�g�̐������s��)
	void GenerateOrders(Factory::PlayerSpawn now_spawn_, bool is_dungeon_level_up = false);

	//DungeonMap�𐶐�
	void GenerateOrdersToMapChip();

	//�X�^�[�g�}�b�v�p�̃}�b�v�𐶐�
	void GenerateOrdersToStartMapChip();

	//���ׂẴA�C�e�������O�ɂ��ׂĐ������Ă����֐�
	void GenerateAllItem();

	//�V���b�v�ɕ��ׂ�A�C�e�������߂�֐�
	//string�^�ŕԂ�
	std::string GenerateShopItem();

	//�G��|�����Ƃ��Ƀh���b�v�A�C�e��
	void GenerateDropItem(tnl::Vector3 pos);

	//�����ɍ��v���郊�X�g�̗v�f�����ׂč폜�ɂ���֐�
	void EraseObjectList();
		
	//�����ɍ��v����L�������X�g�̗v�f�����ׂč폜����֐�(��ɓG�ɍU��������ɌĂяo��)
	void EraseCharaList();

	//���̃N���X�����L���郊�X�g�̗p��c���ׂăN���A����֐�
	//arg1�c�v���C���[���폜���邩�ǂ����̃t���O(true : �폜����Afalse�c�폜���Ȃ�)
	void AllListClear(bool player_deletion_flag = true);

	//�}�b�v�`�b�v�i�[���X�g���폜����
	void AllChipListClear();

	//�L�����N�^�[�̃X�e�[�^�X����������
	void RainForceCharaStetus();

	//*****************�Q�b�^�[*******************
	 
	//���ׂẴA�C�e�����i�[����Ă��郊�X�g��Ԃ�
	const std::vector<std::shared_ptr<Item>>& GetAllItemList() const {
		return all_item;
	}

	//�}�b�v���ɐ������Ă���A�C�e�����X�g��ς���
	std::list<std::shared_ptr<Item>>& GetAliveItem() {
		return alive_item;
	}
	//�������̓G���X�g��Ԃ�
	std::list<std::shared_ptr<Enemy>>& GetAliveEnemyList() {
		return alive_enemy;
	}

	std::shared_ptr<ItemManager> GetItemManager() {
		return item_manager;
	}
	//�X�^�[�ƃ}�b�v�`�b�v���i�[���ꂽ���X�g��Ԃ�
	std::list<std::shared_ptr<MapChip>>& GetStartMapChipList() {
		return start_map_chip_list;
	}
	//�X�^�[�g�}�b�v�̃��C���[2���i�[����Ă��郊�X�g��Ԃ�
	std::list<std::shared_ptr<MapChip>>& GetStartMapSecondChipList() {
		return start_map_second_chip_list;
	}

	//�ǂ��i�[����Ă��郊�X�g��Ԃ�
	std::list<std::shared_ptr<MapChip>>& GetWallMapChipList() {
		return wall_chip;
	}

	//���ׂẴ}�b�v�`�b�v���i�[����Ă��郊�X�g��Ԃ�
	std::list<std::shared_ptr<MapChip>>& GetAllMapChipList() {
		return all_chip_list;
	}

	std::shared_ptr<SkillManager>& GetSkillManager() {
		return skill_manager;
	}
	//*****************�Z�b�^�[*******************

	//�v���C���[�̃X�|�[���ꏊ��ύX����
	void SetPlayerSpawnType(Factory::PlayerSpawn next_spawn);


	//���ׂẴA�C�e�����i�[���郊�X�g�ɃA�C�e����ǉ�����֐�
	void AddAllItemList(Item* item) {
		all_item.emplace_back(item);
	}



private:

	//*****************�ϐ��錾******************
	GameManager* game_manager;
	std::shared_ptr<Stetus>stetus = nullptr;
	std::shared_ptr<ItemManager>item_manager = nullptr;
	std::shared_ptr<SkillManager>skill_manager = nullptr;

	//ResourceManager����R���X�g���N�^��Item�̃f�[�^���󂯎��
	std::vector<std::vector<std::string>> item_data;
	
	//���ׂẴA�C�e�����i�[���Ă����z��
	std::vector<std::shared_ptr<Item>>all_item;
	//�A�C�e���̐������X�g
	std::list<std::shared_ptr<Item>>alive_item;

	//�G�̐������X�g
	std::list<std::shared_ptr<Enemy>>alive_enemy;

	//�Ǌi�[���X�g(�����蔻��Ɏg�p)
	std::list<std::shared_ptr<MapChip>>wall_chip;
	//���ׂẴ}�b�v�`�b�v���i�[����Ă���
	std::list <std::shared_ptr<MapChip>>all_chip_list;

	//�����������̃I�u�W�F�N�g�̐�
	const int GENERATE_ENEMY = 5;
	const int GENERATE_ITEM = 5;

	//�v���C���[�̃X�^�[�g�}�b�v�|�W�V����
	tnl::Vector3 start_pl_pos = { 700, 600, 0 };
	//�}�b�v�`�b�v�̌��f�o�C�X�n���W
	tnl::Vector3 old_chip_pos;


	//=====�X�^�[�ƃ}�b�v�p�ϐ�======
	//�}�b�v�`�b�v�̏c���A����
	int map_graph_value_x = 8;
	int map_graph_value_y = 11;
	//DivGraph�ŕ��������}�b�v�`�b�v���i�[
	int start_map_handle[88];

	const int ALL_START_MAP_CHIP = 88;
	//�}�b�v�`�b�v�P��������̃T�C�Y
	const int MAPCHIPSIZE = 32;
	
	//�X�^�[�ƃ}�b�v�̃f�o�C�X���W(�����W)
	tnl::Vector3 former_map_pos = { -625.0f, -350.0f, 0 };
	
	//�X�^�[�g�}�b�v�̃}�b�v�����i�[����
	std::vector<std::vector<int>>start_map_csv;
	std::vector<std::vector<int>>start_map_second_csv;

	//csv�ɏ������܂�Ă���`�b�v�ԍ��ɉ����Ĕz�u�����f�[�^���i�[����
	std::list<std::shared_ptr<MapChip>> start_map_chip_list;
	std::list<std::shared_ptr<MapChip>> start_map_second_chip_list;

	//*******************�֐�********************
	//�����_���ɓG�̖��O��Ԃ�(���̒l�ɂ���Đ�������G�����߂�)
	std::string GetRandKindEnemyValue();
	//�����_���ɃA�C�e���̖��O��Ԃ�(���̒l�ɂ���Đ�������A�C�e�������߂�)
	std::string GetRandKindItemValue();

	//�L�����̃X�L���f�[�^��SkillManager�ɓn��
	void GiveSkillEffectData();
	//�L�����̃X�L�����擾����֐�
	void SetCharaSkill(std::vector<Skill*>& skill, SkillManager::UseEffectType type, SkillManager::EffectName name);

	//�v���C���[�̃X�L�����Z�b�g����
	void SetPlayerSkill();
	//�G�X�L�����Z�b�g����
	void SetEnemySkill(std::string name);
};