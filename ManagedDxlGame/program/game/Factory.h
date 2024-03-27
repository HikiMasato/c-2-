///*****Description**********************
///�I�u�W�F�N�g�𐶐�����N���X
///�Q�[���S�̂ɑ΂��Ċ֘A���铮�I�����Ȃǂ���ɍs���N���X
/// (���ۃN���X�ɂ��ăT�u�N���X�ɐ�����C����ׂ��Ȃ̂��c)
///**************************************
#pragma once
//-------------------------------------------------------
//Object
#include "MyCamera.h" 

class Object;
class Character;
class MapChip;
class hm::Camera;
class Player;
class Enemy;
class Item;

class GameManager;
class EnemyManager;



class Factory {
public:

	Factory(){}
	~Factory();	

	Factory(GameManager* game_manager_) : game_manager(game_manager_) {};

	//�v���C���[�̃X�|�[���ꏊ
	enum class PlayerSpawn {
		STARTMAP,
		DUNGEON
	};

	//=====================�ϐ�============================
	hm::Camera camera;
	//=====================�֐�============================
	
	//�󒍂��󂯂Đ�������֐�
	//�������鐔�A�^�C�~���O�͂��ׂ�ObjectManager���Ǘ����Ă���
	//�����܂ł������݂̂��s��
	//arg1�c�����|�W�V����(�����_���Ɍ��܂�)
	//arg2�c�q�b�g�|�C���g(HP)
	//arg3�cMP(WIP)
	//arg4�cATK
	//arg5�cDEF
	//arg6�cName
	//arg7�c�摜�n���h��
	//arg8�c���ׂẴI�u�W�F�N�g���i�[���Ă��郊�X�g
	void GeneratePlayer(PlayerSpawn now_spawn, tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, std::list<std::shared_ptr<Character>>& chara_list);
	//arg9�c�������Ă���G�̃��X�g
	void GenerateEnemy(tnl::Vector3 start_pos, int hp, int mp, int attack, int defence, std::string name, std::string chara_ph, int exp, int level, std::list<std::shared_ptr<Character>>& chara_list, std::list<std::shared_ptr<Enemy>>& ali_ene);
	//arg2�c�A�C�e�����v���C���[�ɗ^������ʂ̐��l(HP�񕜗ʂ�AATK�㏸��,DEF�㏸�� etc...)
	void GenerateItem(tnl::Vector3 item_pos, int item_stetus, std::string item_name, std::string item_handle, std::string item_desctiption, int item_price, std::list<std::shared_ptr<Object>>& obj_list, std::list<std::shared_ptr<Item>>& ali_item);

	//���ׂẴA�C�e������x�������Ċi�[����֐�
	//�����_���ɐ������ꂽ���̈ȊO��
	void AllItemCreate(tnl::Vector3 item_pos, int item_stetus, std::string item_name, std::string item_handle, std::string item_desctiption, int item_price, std::vector<std::shared_ptr<Item>>& all_item);

	//�}�b�v�`�b�v���C���X�^���X������
	//arg1�c�}�b�v�`�b�v�̌����W
	//arg2�c�摜�n���h��
	//arg3�c�C���X�^���X�������}�b�v�`�b�v�N���X���i�[����z��
	//arg4�cwall_chip���i�[����z��
	//arg5�c�C���X�^���X������}�b�v�`�b�v���ǂ��ǂ����̃t���O(true�c�ǁAfalse�c�ǈȊO)
	void GenerateMapChip(tnl::Vector3 old_chip_pos, int chip_handle, std::list<std::shared_ptr<MapChip>>& all_chip_list, std::list<std::shared_ptr<MapChip>>& wall_list, bool is_wall = false);


	//======================�Q�b�^�[===========================

	
	std::shared_ptr<Player> GetPlayer() const {
		return player;
	}

	std::shared_ptr<Enemy> GetEnemy() const {
		return enemy;
	}

	std::shared_ptr<MapChip> GetMapChip() const {
		return mapchip;
	}

	std::shared_ptr<Item> GetItem() const {
		return item;
	}

	PlayerSpawn GetPlayerSpawn() const {
		return now_spawn;
	}
	
	//======================�Z�b�^�[===========================
	void SetPlayerSpawn(PlayerSpawn set_spawn) {
		now_spawn = set_spawn;
	}

	void SetPlayer(std::shared_ptr<Player> player_) {
		player = player_;
	}


private:

	//=====================�ϐ�============================
	GameManager* game_manager;

	std::shared_ptr<Player> player = nullptr;
	std::shared_ptr<Enemy> enemy = nullptr;
	std::shared_ptr<MapChip>mapchip = nullptr;
	std::shared_ptr<Item> item = nullptr;

	//�v���C���[�̃X�|�[���^�C�v
	PlayerSpawn now_spawn = PlayerSpawn::STARTMAP;

#if 0

	////�Ăяo�����ň����ɐݒ肵���N���X��template <A>�ɒu�������
	////�N���X�̃C���X�^���X�����s���֐�
	//template<class A>
	//std::shared_ptr<A> GenerateObject(A) {
	//	object->object_list.emplace_back(object = std::make_shared<A>());
	//}

	//template<Object::ObjectType A>
	//std::list<std::shared_ptr<Object*>> StoreObject(Object::ObjectType) {
	//	auto it = object->object_list.begin();
	//	switch (Object::ObjectType)
	//	{
	//	case:Object::ObjectType::ENEMY
	//		for (it->get() == Object::ObjectType::ENEMY) {
	//			return
	//		}
	//		break;
	//	
	//	case:Object::ObjectType::ITEM
	//		for (it->get() == Object::ObjectType::ITEM) {
	//			return
	//		}
	//		break;
	//		
	//	default:
	//		break;
	//	}
	//	
	//}

		////object����������֐�
	//void  DeleteObject() {

	//	auto it = obj->objlist.begin();
	//	while (it != obj->objlist.end()) {

	//		if (Obj::ObjectType::TARGET == obj->GetActType()) {
	//			obj.reset();
	//		}
	//		if (Obj::ObjectType::PLAYER == obj->GetActType()) {
	//			obj.reset();
	//		}
	//		if (Obj::ObjectType::ITEM == obj->GetActType()) {
	//			obj.reset();
	//		}

	//		it++;
	//	}

	//}
#endif // 0
};
