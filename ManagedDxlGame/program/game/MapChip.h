///**************Description*****************
/// MapChip�N���X�̕`����s���N���X
/// Chip���ƁA�|�W�V�����̏���ێ�����
/// ���̃N���X�ł̓}�b�v�`�b�v�̕`����s���A�`��̐����MapManager���s���Ă���
///******************************************
#pragma once
class Object;
class Player;
class Enemy;
class hm::Camera;
class Collistion;

class MapChip : public Object {
public:
	MapChip(){}
	//�C���X�^���X����MapManager��Resource����󂯎����csv�i�[vector���󂯎��
	MapChip(std::vector<int> re_chip,GameManager* game_manager_);

	//�X�^�[�g�}�b�v�̃f�[�^���󂯎��C���X�^���X��������(���������X�g)
	MapChip(tnl::Vector3& pos, int chip_handle) : start_map_pos(pos), start_map_handle(chip_handle) { SetMapChipPos(pos); };

	
	//***********************enum********************
	//�}�b�v�̃^�C�v
	enum MapType {
		WALL,		//��
		PASSWAY,	//�ʘH
		CHARACTOR,	//�L�����N�^�[
		STAIRS,		//�K�i
		SHOP,		//�X
	};

	//�I�[�g�^�C���p�񋓑�
	enum GraphicType {
		ROOMWALL,
		ROOMTOP,
		ROOMRIGHT,
		ROOMBOTTOM,
		ROOMLEFT,
		ROOMLEFTTOP,
		ROOMRIGHTTOP,
		ROOMLEFTBOTTOM,
		ROOMRIGHTBOTTOM,
		MAXNUM
	};

	//***********************�ϐ�********************
	//�I�[�g�^�C���p�`�b�v
	int autoTileChip[GraphicType::MAXNUM];

	//�}�b�v�`�b�v
	int wall = 0;
	int floor = 0;
	int stair = 0;
	int shop = 0;
	int voidgh = 0;
	
	int up = 0;
	int botton = 0;
	int right = 0;
	int left = 0;

	//�}�b�v�`�b�v���i�[
	std::vector<int> allchip;

	//�X�^�[�ƃ}�b�v����󂯎��
	tnl::Vector3 start_map_pos;
	int start_map_handle = 0;

	//*********************�֐�************************
	ObjectType GetObjectType() const override {
		return ObjectType::MAP;
	}

	void SetMapChipPos(tnl::Vector3 pos) {
		SetOldChipPos(pos);
	}


	//�}�b�v�`�b�v�̕`��
	//MapManager�ŕ`��𐧌䂵�Ă��� 
	//arg1�ccamera
	//arg2�cchipsize
	//arg3�c�g�嗦
	//arg4�c�摜�n���h��
	void MapChipDraw(const hm::Camera& camera, float graphicssize, double ratio, int chip_handle);

	//�X�^�[�ƃ}�b�v�`��֐�
	void StartMapDraw(const hm::Camera& camera);

private:
	GameManager* game_manager;
	hm::Camera camera ;
};