///*****Description******************************
///�_���W�������̃}�b�v�Ɋւ���N���X
///��敪���@�ɂ��}�b�v�����_�������ƁA
///���@��@�̂��}�b�v�̈�M�����h�~�����̂Q�ɂ���ă}�b�v�����������
///�܂��A�S�ẴZ���̏��������A�ǂ��ǂłȂ����Ȃǂ�Ԃ��֐�������
///**********************************************

///********�ǉ�********
/// �s���~�܂�
/// ������
/// �B���ʘH(�ƂĂ������I)
/// ���[�v
///********************

///*******����(�������)
///�@�_���W�����̐[��(�����_��)
///  
/// 
///*******************
  
#pragma once
#include "MyCamera.h"
#include "Object.h"

class ObjectManager;
class Object;
class GameManager;
class Factory;
class hm::Camera;

class MapManager{
public:
	MapManager() {}
	//�C���X�^���X����Resource����󂯎����csv�i�[vector���󂯎��
	//�}�b�v���̏�����(���ׂĂ�ǂɂ���)
	MapManager(const int map_width, const int map_height, std::vector<int>handles, GameManager* ptr);
	~MapManager();

	//�I�[�g�^�C���p�񋓑�
	enum GraphicType
	{
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

	//���̌�����\���񋓌^
	enum class WayDir {
		UP,		//��
		DOWN,	//��
		LEFT,	//��
		RIGHT	//�E
	};

	//�}�b�v�S�̂̕`��
	void MapDraw(const hm::Camera& camera, float graphicssize, double ratio);

	//�~�j�}�b�v�`��
	void MiniMapDraw(const hm::Camera& camera);


	//�~�j�}�b�v�p �����\������ύX�֐�
	//���[�J�����W�������ɂƂ��āA���̍��W�ɑΉ����镔����K��ς݂ɂ���Ƃ�������������֐�
	void ChangeRoomVisit(tnl::Vector3 localPos);

	//�~�j�}�b�v�p �ʘH�\������ύX�֐�
	// ���[�J�����W�������ɂƂ��āA���̍��W�̎��͂̒ʘH��K��ς݂ɂ���֐�
	void ChangeWayVisit(tnl::Vector3 localPos);

	//�~�j�}�b�v�p ���W����\�����Ă����`�b�v�����肷��֐�
	inline bool CheckCanDraw(tnl::Vector3 localPos) {
		return visited[localPos.y][localPos.x];
	}
	
	//�}�b�v�`�b�v��ResetChip�ŏ�������
	void ResetChip(const int x, const int y, const int SetChip);
	//�G���A�����J�n
	void AreaDivideStart(const int Width, const int Height, std::shared_ptr<MapManager> map);
	
	//����̃}�b�v���W�������̂ǂ����ɑ��݂��邩�m�F����֐� �Ԃ�l�͕����ԍ�
	int CheckIsThere(const int x, const int y);


	//�K�i�e�X�g�p�֐�
	void AddStairList(tnl::Vector3 pos);
	//�K�i�e�X�g�p�N���A�֐�
	inline void ResetStairList() {
		stairs.clear();
	}
	//stairs�c�K�i
	std::vector<tnl::Vector3> stairs;

	//���\������Ă���S�Ă̕����̏㉺���E���W������`�悷��Debug�֐�
	void DrawAllRoomPos(const std::vector<std::vector<int>>RoomList);

	//�Z��������͂̒ʘH���m�F���A�`�悷�ׂ��摜�����Ԃ��擾����֐� 
	int CheckAroundWay(const int x, const int y);

	//�Z�����猩�Ď��͂ɒʘH�����邩�ǂ����m���߂�֐�
	bool CheckAround(const int x, const int y);

	//===============================================�Q�b�^�[=================================================
	
	//�}�b�v�`�b�v���擾
	inline int GetChip(const int x, const int y) {
		//�͈͊O
		if (IsOutOfRange(x, y))return OUTOFRANGE;
		//�͈͓�
		return ground[y][x];
	}
	//���l�ł̃}�b�v��񂪊i�[���ꂽ�z���Ԃ�
	std::vector<std::vector<int>> GetGround() const {
		return ground;
	}

	//AutoTitleChip�Q�b�^�[
	int GetAutoTileChip(GraphicType type) {
		return auto_tile_chip[type];
	}
	int GetFloor() const {
		return floor;
	}
	int GetStair() const {
		return stair;
	}
	int GetShop() const {
		return shop;
	}
	//�}�b�v���̃V���b�v�Ŕ��������������ǂ����擾����֐�
	inline bool GetDoneBuy() const {
		return done_buy_item;
	}

	//�͈͓��̃}�b�v�`�b�v���擾
	void GetAllChip(const int roomNum, std::vector<std::vector<int>>& chips);

	//�O������"�����̐�"���擾����Ƃ��̊֐�
	inline int GetRoomNum() {
		return divideRoom.size() - 1;
	}

	//�O������"����̕���"���擾����Ƃ��̊֐�
	inline std::vector<int> GetRoom(int roomNum) {
		return divideRoom[roomNum];
	}

	//�O������"����̕����̑傫��"���擾����Ƃ��̊֐�
	tnl::Vector3 GetRoomValue(const int roomNum);

	//�������W�n��0,0���擾����֐�
	tnl::Vector3 GetRoomStartPos(const int roomNum);


	
	//===============================================�Z�b�^�[=================================================

	void SetShop();

	// ���͈͂��ׂĂ𓹂ŏ�������
	void SetAllChip(const int Left, const int Up, const int Right, const int Down);

	//����̕����̍���ƉE���̍��W���擾����֐�
	void SetCornerPos(int roomNum, tnl::Vector3& LeftTop, tnl::Vector3& RightBottom);
	
	inline void SetDoneBuy() {
		if (!done_buy_item)done_buy_item = true;
	}


private:
	
	GameManager* game_manager;

	//2�����z��
	//�}�b�v�̔w�i
	std::vector<std::vector<int>> ground;
	//�~�j�}�b�v�p�\������z��
	std::vector<std::vector<bool>> visited;

	//�}�b�v�`�b�v���i�[
	std::vector<int> allchip;

	//�����̍���ƉE���̍��W�̃o�b�t�@�[
	tnl::Vector3 leftupper;
	tnl::Vector3 rightbotton;

	//�����̐�
	int sumRoomNum = 0;

	//�I�[�g�^�C���p�`�b�v
	int auto_tile_chip[GraphicType::MAXNUM];

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

	


	int color_red = GetColor(255, 0, 0);
	int color_green = GetColor(0, 255, 0);
	int color_blue = GetColor(0, 0, 255);
	int color_yellow = GetColor(200, 200, 0);
	int color_purple = GetColor(200, 0, 200);

	int colors[5] = { color_red,color_green ,color_blue,color_yellow,color_purple };

	//���񕪊����̂��߂̃t���O
	bool doneFirstDivide = false;
	//�������ς݃V���b�v�̃t���O
	bool done_buy_item = false;

	//�V���b�v���[�J�����W
	tnl::Vector3 localPos;

	//�����̍ŏ���
	const int ROOMMINWIDTH = 8;
	//�����̍ŏ�����
	const int ROOMMINHEIGHT = 5;
	//�����̍ő吔
	const int ROOMMAXNUM = 8;


	//�}�b�v�̕�(�R���X�g���N�^�Ŏg�p)
	int width = 0;
	//�}�b�v�̍���(�R���X�g���N�^�Ŏg�p)
	int height = 0;
	//�͈͊O����p
	const int OUTOFRANGE = -1;

	//�~�j�}�b�v�`��p�}�b�v�`�b�v
	int miniMapChip[3] = {};
	//�~�j�}�b�v�v���C���[gh
	int miniPlayer = 0;
	////�~�j�}�b�v�G�l�~�[gh
	//int miniEnemy = 0;

	//��ԕ���(���)
	std::vector<std::vector<int>> divideArea;

	//����������Ԃ���U�ۑ�����vector
	std::vector<std::vector<int>> dumpDivideArea;

	//��ԕ���(�敪��)
	std::vector<std::vector<int>> divideLine;

	//��ԕ���(����) ��,��,�E,��,id
	std::vector<std::vector<int>> divideRoom;


	//��������
	enum MapDir {
		VERTICAL,	//�c
		HORIZONTAL,	//��
	};

	bool IsOutOfRange(const int x, const int y);

	//��ԕ����@
	//�㉺���E�̍��W�ƕ����ԍ����擾����vector�Ɋi�[
	void SetDivideArea(const int Left, const int Up, const int Right, const int Down, const int Id);

	//���������G���A�̑傫�������i�[����֐�
	void SetLargeDivideArea(const int Left, const int Up, const int Right, const int Down, const int Id);

	//�S�G���A����ŕ���
	//�n�_,�I�_��xy,����
	void SetDivideLine(const int Start_x, const int Start_y, const int Goal_x, const int Goal_y, const int Dir);

	//�����̊i�[
	void SetDivideRoom(const int Left, const int Up, const int Right, const int Down, const int RoomId);


	//���̕���
	void AreaDivide();
	//�����̍쐬
	void CreateRoom();
	//�ʘH�̍쐬
	void CreatePassWay();



	//�����̕ӂɒʘH�����邩�ǂ����m�F����֐�
	//arg1�c�������Ȃ����W(�c�Ɍ����Ȃ�x���W,���Ɍ����Ȃ�y���W
	//arg2�c��������ӂ̍ŏ��̍��W
	//arg3�c��������ӂ̍ő�̍��W
	//arg4�c�����̂ǂ��������̕ǂ� 0:��,1:�E,2:��,3:��
	bool CheckPassWay(const int roomPos_set, const int roomPos_moveStart, const int roomPos_moveGoal, const WayDir dir);

	//�ŏ��ƍŌ�̕����������̕����������ւ̓���������֐� 
	tnl::Vector3 RandomPoint(const int roomId, WayDir& dir);
	//tnl::Vector3 Map::RandomPoint(int roomId, int& dir,vector<int>setDivideLine);

	//���������אڂ��镔���ւƒʘH��L�΂��֐�
	bool CreateSecondWay(const int x, const int y, const WayDir dir, const int roomId);

	

	//���̍��W�̃Z�����ʘH��������true��Ԃ��֐�
	////�����œn���ꂽ���W�̃}�b�v�`�b�v�̒l��PASSWAY,STAIRS,SHOP�Ȃ�true���A�����łȂ����false��Ԃ��֐�
	bool CheckThisCell(const int x, const int y);

	//�Z��(x,y)����݂�dir�����ɐ����ȃZ��2��dir�����̃Z�����ǂ��ʘH���m�F����֐�
	bool CheckChip(const int x, const int y, const WayDir nextDir);

	//�F�R�[�h����string��Ԃ��֐�
	std::string GetColorName(const int code);


};
