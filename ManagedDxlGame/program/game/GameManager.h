///*****Description**********************
///�Q�[���@�\�S�ʂɊւ���N���X
///�Q�[���S�̂ɑ΂��Ċ֘A���鏈���y�уC���X�^���X��ۗL����N���X
/// �Q�[���S�ʂ̋@�\�N���X���m�̏����Ȃǂ������ōs��
///**************************************
#pragma once
#include "Skill.h"
#include "SkillManager.h"
//�O���錾
class ObjectManager;
class Item;

class Character;
class Enemy;


class SceneManager;
class DebugManager;
class ResourceManager;
class MapManager;
class Collision;
class EffectManager;
class Menu;
class SoundManager;
class Inventory;

class GameManager {
public:
	
	GameManager();
	~GameManager();

	//StartPositionPlace
	enum class SetStartPositionType {
		PLAYER,//�v���C���[
		STAIR, //�K�i
		ENEMY, //�G
		ITEM   //�A�C�e��
	};

	//�_���W�������(�ȉ��̗񋓌^�̊e�l��uint32_t�^�Ƃ��Ĉ�����)
	//���^ : uint32_t�^
	enum class DungeonType : uint32_t {
		TOWER,		//0
		FOREST,		//1
		WATERWAY,	//2
		DUNGEONMAX	//3
	};

	enum class ScaleMode : uint32_t {
		NOMAL,
		ENLAEGE		//�g��
	};

	//************************�ϐ�*******************************

	const int MAPHEIGHT = 48;
	const int MAPWIDTH = 60;
	//�}�b�v�`�b�v�̈ꖇ�̑傫��
	const int GRAPHICSIZE = 20;

	
	//************************�֐�*******************************
	
	void update(float delta_time);
	
	void Draw();
	//MiniMap���u�����h���ĕ`��
	void DrawBlendedMiniMap();


	//GameManager�̏�����
	void InitGameManager();

	void DungeonReCreate();
	inline void ForceInventoryChange(int& value) {
		value--;
		is_delete_invantory = false;
	}
	
	//�V���b�v�̃A�C�e�����v���C���[�̃C���x���g���ɒǉ�����֐�
	void AddItemFromShop(std::shared_ptr<Item>shop_item);
	//�C���x���g���ɃA�C�e����ǉ�����֐�
	void AddItemToInventory(const std::string name, std::vector<Inventory*>& inventories, int& inventory_value);
	//�C���x���g������A�C�e�����폜����֐�
	void EraseItemFromInventory(const int now_inventory_id);
	//�h���b�v�A�C�e�����X�g�������̃A�C�e�����폜����֐�
	bool DetectDropItem(std::shared_ptr<Item> item, std::list<std::shared_ptr<Item>>& it_list);
	

	//�_���W�����̐���
	void GenerateDungeon(DungeonType dungeontype);
	//�_���W�����}�b�v�̐���
	void GenerateDungeonMap(DungeonType dungeontype);


	bool CheckIsThereEnemy(tnl::Vector3 pos);
	
	//�����̓G���v���C���[�Ɨׂ荇���Ă��邩�m�F
	bool CheckNearByPlayer(std::shared_ptr<Enemy> enemy);
	//�G������ł��邩����
	void CheckIsDeadCharactor(std::shared_ptr<Character> object,tnl::Vector3 pos);
	
	
	//�y�A�ɂ���Object���ǂ��������������Ă��邩�𔻒�
	void UpdateHitCheck();

	//HP��0�̃L�����N�^�[�̓����蔻����폜����
	void EraseHitList();

	//�t���C�E�F�C�g�p�^�[����LoadGraph
	int LoadGraphEx(std::string_view graph);

	int CheckIsThere(tnl::Vector3 pos);

	//�`����W����}�b�v���W�ɕϊ�
	tnl::Vector3 WorldToLocalPos(tnl::Vector3 pos);
	//�}�b�v���W����`����W�ɕϊ�
	tnl::Vector3 LocalToWorldPos(int map_x, int map_y);

	//�`����W�����[�J�����W�ɕϊ�
	tnl::Vector3 WorldToMap(const int world_x, const int world_y);
	//���[�J�����W��`����W�ɕϊ�
	tnl::Vector3 MapToWorld(const int map_x, const int map_y);
	
	//��ʂ̊g��k����ύX����֐�
	void ScaleChange();
	//�X�P�[���`�F���W���̃|�W�V�����C��
	void ModifyScalePos();

	//MiniMap���`��ł��邩�ǂ����𔻒�
	bool CanDrawMiniMap(tnl::Vector3 localpos);
	//MiniMap�̍X�V
	void RefreshMiniMap(tnl::Vector3 localpos);
	
	
	//�o�H��̒��Ԓn�_�̍��W�����T�C�Y����
	//�����œn���ꂽ�����̐���1�𑫂����l���Awaypoint�x�N�^�[�̃T�C�Y�ɐݒ�
	void InitWayPointVector(const int initroomnum);


	//================�_���[�W�֐�==================
	// 
	//�_���[�W�v�Z����
	void InflictDamageToTarget(Character* object, tnl::Vector3 pos);
	//�_���[�W�v�Z����
	float CalculationDamage(int atk, int def);
	void CalculationSkillDamageToTarget(Character* object, const tnl::Vector3 pos, Skill* now_use_skill);
	float CalculationSkillDamage(const float skill_rate, const int atk, const int def);
	//�_���[�W���f����
	void AffectDamage(float damage, std::shared_ptr<Character>object);
	
	//�J�����̒��S���W���v���C���[�̍��W�ɐݒ肷��
	void CameraReset();


	//==================================�Q�b�^�[========================================

	//���ݍ��W�̎w��̕����̂P��̍��W���擾����
	inline const tnl::Vector3& GetFirstDirVector(int dir) const {
		return dir_vectors[dir];
	}

	//�_���W�����^�C�v���i�[����Ă���z�񂩂疼�O���擾����
	inline std::string GetDungeonType(DungeonType dungeontype) {
		//underlying_type(�񋓌^�̊��^���擾���邽�߂̌^����)
		//static_cast�Ƃ͈Ⴂ�A�^�ϊ��ł͂Ȃ��^�̏��̒񋟂��s������(�f�[�^�̑������N�������Ƃ�h����)
		return DUNGEONTYPE[std::underlying_type<DungeonType>::type(dungeontype)];
	}

	//���݂̃O���t�B�b�N�T�C�Y��Ԃ�
	float GetNowGraphSize() const {
		return now_gh_size;
	}

	//���݃}�b�v�Ŕ������������t���O�̎擾
	bool GetDoneBuyInStores() const;

	//�_���W�������Ƃ̉摜���擾����
	std::vector<int>& GetGraphicsHandles(DungeonType dungeontype) const;
	//�}�b�v�`�b�v�̏����擾
	int GetMapChip(tnl::Vector3 mappos) const;

	//����̍��W�̓G���擾����
	std::shared_ptr<Enemy> GetIsThereEnemyToDir(tnl::Vector3 pos);

	//�����ɓn���ꂽ�����ԍ������W�����ԉ����o�����擾����֐�
	tnl::Vector3 GetFarPoint(int room_id, tnl::Vector3 pos);

	//�����_���Ȓl��Ԃ��֐�
	inline int GetRandValue(int a, int b);

	int& GetInventoryValue() {
		return inventory_value;
	}

	std::vector<Inventory*>& GetInventorys() {
		return inventories;
	}

	bool GetIsDeleteInventotry() const {
		return is_delete_invantory;
	}

	bool GetIsDebug() const {
		return is_debug;
	}

	//���݂̉摜�{�����擾
	double GetRatio() const {
		return ratio;
	}

	//���݂̕`���Ԃ��擾
	ScaleMode GetNowScale() const {
		return now_scale;
	}

	std::shared_ptr<Collision> GetCollision() {
		return collision;
	}

	std::shared_ptr<SoundManager> GetSoundManager() {
		return sound_manager;
	}

	std::shared_ptr<ResourceManager> GetResourceManager() {
		return re_manager;
	}

	std::shared_ptr<MapManager> GetMapManager() {
		return map_manager;
	}

	std::shared_ptr<ObjectManager> GetObjectManager() {
		return object_manager;
	}

	//==================================�Z�b�^�[========================================

	//���݂̃O���t�B�b�N�T�C�Y���Z�b�g����
	void SetNowGraphSize(int new_size) {
		now_gh_size = new_size;
	}

	//���݂̃}�b�v�Ŕ������������t���O�̍X�V
	void SetDoneBuyInStores() const;

	//�����蔻����`�F�b�N����Object�ǂ������y�A�ɂ���
	void SetObjectSHitCheck();

	//�ʘH��������
	void SetRoomWayPoint(const tnl::Vector3 pos, const int roomid);

	//�v���C���[�̕����ԍ���ύX����
	void SetPlayerRoomValue(int nowroomvalue);

	//�}�b�v���̃����_���ȕ������擾
	//�����̒��̃����_���ȍ��W���擾
	//���W��`����W�ɕϊ����ĕԂ�
	tnl::Vector3 SetStartPosition(SetStartPositionType type);

	void SetIsDeleteInventory(bool now_is_delete) {
		is_delete_invantory = now_is_delete;
	}

	bool SetIsDebug(bool now_debug) {
		is_debug = now_debug;
	}

	//�摜�{�����X�V
	void SetRatio(double now_ratio) {
		ratio = now_ratio;
	}

	//�X�P�[����Ԃ�ύX
	ScaleMode SetNowScale(ScaleMode new_scale) {
		now_scale = new_scale;
	}
	//===========================================
	
	//�C���x���g������1���Z����
	void AddInventoryValue() {
		inventory_value++;
	}
	//�C���x���g������1���Z����
	void SubtractionInventoryValue() {
		inventory_value--;
	}

private:
 
	enum class Vec {
		DOWN,
		LEFT,
		RIGHT,
		UP
	};

	std::shared_ptr<ObjectManager> object_manager = nullptr;
	std::shared_ptr<MapManager> map_manager = nullptr;
	std::shared_ptr<ResourceManager> re_manager = nullptr;
	std::shared_ptr<SoundManager>sound_manager = nullptr;
	std::shared_ptr<Collision> collision = nullptr;
	DebugManager* debug_manager = nullptr;
	Inventory* inventory = nullptr;
	Inventory* bankinventory = nullptr;


	//========================�ϐ�======================================


	//�����Object�������Ă���������
	//animation�`����W
	tnl::Vector3 dir_vectors[4]{
		tnl::Vector3(0,1,0),	//down
		tnl::Vector3(-1,0,0),	//left
		tnl::Vector3(1,0,0),	//right
		tnl::Vector3(0,-1,0)	//up

	};

	//�C���x���g�����폜���ꂽ���ǂ�����flag
	bool is_delete_invantory = false;

	//�v���C���[�ϐ��ꎞ�ۊǗp
	tnl::Vector3 player_pos_buff = {};

	std::vector<std::vector<tnl::Vector3>> waypoint;
	//LoadGraphEx�Ŏg�p(�摜���i�[)
	std::unordered_map<std::string_view, int> map_graph;

	DungeonType nowdungeon;
	//�_���W�����^�C�v
	const std::string DUNGEONTYPE[3] = { "��","�X","��" };


	//�A�C�e����ǉ�����ۂ̊i�[�\�ȃC���x���g���z��ԍ�
	int inventory_value = 0;
	//UI�C���x���g���ɕ`�悷�郊�X�g
	std::vector<Inventory*>inventories;
	//camp�V�[���̑q�ɃC���x���g���z��
	std::vector<Inventory*>bank_inventroy_list;

	//���݂̃O���t�B�b�N�T�C�Y
	float now_gh_size = GRAPHICSIZE;

	//�{��(���Graphics�Ŏg�p)
	double ratio = 1.0;
	const float scale[2] = { 20.0f,40.0f };
	//���݂̕`����@
	ScaleMode now_scale = ScaleMode::NOMAL;

	//debug�؂�ւ�
	bool is_debug = false;

};

