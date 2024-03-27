//-------------------------------------------------------
//�����̂���
#include <time.h>
#include <vector>
#include <string>
#include <memory>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "DxLib.h"
#include "gm_main.h"
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "ObjectManager.h"
#include "SceneManager.h"
#include "GameManager.h"
#include "ResourceManager.h"
#include "MapManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "MyCamera.h" 
#include "Player.h"
//-------------------------------------------------------
#include "Debug.h"



//------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
MapManager::MapManager(int map_width, int map_height, std::vector<int>handles, GameManager* game_manager_)
{
	game_manager = game_manager_;
	
	allchip.resize(handles.size());
	allchip = handles;

	//��
	floor = allchip[0];
	//��
	wall = allchip[1];
	//top
	up = allchip[2];
	//bottom
	botton = allchip[3];
	//left
	left = allchip[4];
	//right
	right = allchip[5];

	//�I�[�g�^�C���p�z��
	auto_tile_chip[0] = wall;
	auto_tile_chip[1] = up;
	auto_tile_chip[2] = right;
	auto_tile_chip[3] = botton;
	auto_tile_chip[4] = left;
	auto_tile_chip[5] = wall;
	auto_tile_chip[6] = wall;
	auto_tile_chip[7] = wall;
	auto_tile_chip[8] = wall;

	//������csv����ǂݍ��񂾂ق������ꂢ�ȋC������
	stair = game_manager->LoadGraphEx("testgraphics/Stairs_.png");//�K�i

	shop = game_manager->LoadGraphEx("testgraphics/shop.png");//shop

	voidgh = game_manager->LoadGraphEx("testgraphics/void.png");

	//-------------------------------------------------------------------------------------------
	//minimap�p
	miniMapChip[0] = game_manager->LoadGraphEx("testgraphics/mini_PassWay.png");
	miniMapChip[1] = game_manager->LoadGraphEx("testgraphics/mini_Stair.png");
	miniMapChip[2] = game_manager->LoadGraphEx("testgraphics/mini_shop.png");

	miniPlayer = game_manager->LoadGraphEx("testgraphics/mini_Player.png");
	//miniEnemy = game_manager->LoadGraphEx("graphics/mini_Enemy.png");

	//-------------------------------------------------------------------------------------------
	width = map_width; //default�l��0
	height = map_height;//default�l��0

	//�c�̏�����
	ground.resize(height);
	visited.resize(height);
	for (auto v : ground)v.resize(width);
	for (auto v : visited)v.resize(width);

	for (int i = 0; i < height; i++) {
		ground[i].resize(width);
		visited[i].resize(width);
	}

	//-------------------------------------------------------------------------------------------
	//���ׂĂ�ǂɂ���
	for (int i = 0; i < height; i++) {
		for (int k = 0; k < width; k++) {
			ground[i][k] = MapChip::MapType::WALL;
			visited[i][k] = false;
		}
	}

	//-------------------------------------------------------------------------------------------
	


}

//------------------------------------------------------------------------------------------------------------
//�f�X�g���N�^
MapManager::~MapManager()
{
	ground.clear();

}

//------------------------------------------------------------------------------------------------------------
// �~�j�}�b�v�p�����\������
// ���[�J�����W�������ɂƂ��āA���̍��W�ɑΉ����镔����K��ς݂ɂ���֐�
void MapManager::ChangeRoomVisit(tnl::Vector3 localpos)
{
	// �����ɓn���ꂽ���W���}�b�v���̂ǂ���������
	int check = CheckIsThere(localpos.x, localpos.y);
	// �͈͊O(�}�b�v�͈͊O)�̏ꍇreturn
	if (check == OUTOFRANGE)return;

	// ���[�J�����W�ɑΉ�����A����ƁA�E���̍��W���擾����
	SetCornerPos(check, leftupper, rightbotton);

	// rightbotton.y���}�b�v�̍�����菬�����Arightbotton.x���}�b�v�̕���菬�������
	// �E���̍��W���}�b�v�̍����ƕ������������ꍇ�́A����ƉE���̍��W�𒆐S�ɁA2�}�X���̗]�T�������āA
	// visited�z��̒l��true�ɍX�V���� 
	// ����́A�����̎��͂̕ǂ�ʘH���K��ς݂ɂ��邽��
	if (rightbotton.y < game_manager->MAPHEIGHT && rightbotton.x < game_manager->MAPWIDTH) {
		for (int i = leftupper.y - 2; i < rightbotton.y + 2; i++) {
			for (int k = leftupper.x - 2; k < rightbotton.x + 2; k++) {
				// visited�z��̒l��true�ɍX�V����
				visited[i][k] = true;
			}
		}
	}

	// �E���̍��W���}�b�v�̍����╝�𒴂��Ă���ꍇ�́A����ƉE���̍��W�͈͓̔��ŁA
	// visited�z��̒l��true�ɍX�V���܂��B
	// ����́A�}�b�v�̒[�ɂ��镔���̏ꍇ�ɁA�͈͊O�̍��W�ɃA�N�Z�X���Ȃ��悤�ɂ��邽��
	else {
		for (int i = leftupper.y; i < rightbotton.y; i++) {
			for (int k = leftupper.x; k < rightbotton.x; k++) {
				// visited�z��̒l��true�ɍX�V����
				visited[i][k] = true;
			}
		}
	}

}

//------------------------------------------------------------------------------------------------------------
// �~�j�}�b�v�p�ʘH�\������
// ���[�J�����W�������ɂƂ��āA���̍��W�̎��͂̒ʘH��K��ς݂ɂ���֐�
void MapManager::ChangeWayVisit(tnl::Vector3 localpos)
{
	//�����ɓn���ꂽ���W���}�b�v���̂ǂ���������
	int check = CheckIsThere(localpos.x, localpos.y);

	//�͈͊O(�}�b�v�͈͊O)�̏ꍇreturn
	if (check != OUTOFRANGE)return;

	//����2�}�X���͈͊O�ł����return(�ʘH�̂���2�}�X�`�F�b�N�ő��v)
	//���A���`�F�b�N
	if (IsOutOfRange(localpos.x - 2, localpos.y - 2))return;
	//�E�A��`�F�b�N
	if (IsOutOfRange(localpos.x + 2, localpos.y + 2))return;

	
	//x1 : ���@x2 : �E

	int x1 = localpos.x - 2;
	int x2 = localpos.x + 2;
	
	//y1 : ���@y2 : ��

	int y1 = localpos.y - 2;
	int y2 = localpos.y + 2;

	//����ƉE���̍��W�͈͓̔��ŁAvisited�z��̒l��true�ɍX�V����
	//����́A�ʘH�̈ꕔ��K��ς݂ɂ��邽��
	for (int i = y1; i <= y2; i++) {
		for (int k = x1; k <= x2; k++) {
			//visited�z��̒l��true�ɍX�V����
			visited[i][k] = true;
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//�w�肳�ꂽ�͈͂̃}�b�v�`�b�v�����ׂĒʘH�ɐݒ肷��֐�
void MapManager::SetAllChip(int left, int up, int right, int down)
{
	//�����œn���ꂽ����ƉE���̍��W�����ƂɁA��d��for���Ŕ͈͓��̂��ׂẴ}�b�v�`�b�v�ɃA�N�Z�X����
	for (int i = up; i <= down; i++) {
		for (int k = left; k <= right; k++) {

			//ResetChip�֐����Ăяo���āA�e�}�b�v�`�b�v�̎�ނ�PASSWAY�i�ʘH�j�ɕύX����
			ResetChip(k, i, MapChip::MapType::PASSWAY);
		}
	}
}

//------------------------------------------------------------------------------------------------------------
//�}�b�v�̕����ƕ����̍쐬�A�ʘH�̍쐬���s���֐�
void MapManager::AreaDivideStart(int Width, int Height, std::shared_ptr<MapManager> map)
{
	//�}�b�v����
	AreaDivide();
	//�������ꂽ��Ԃɕ����𐧍�
	CreateRoom();

	//�����̐������E�F�C�|�C���g�̃x�N�^�[��������
	game_manager->InitWayPointVector(GetRoomNum());
	DebugManager::DebugString("\n�ʘH�쐬�O�f�o�b�O\n");

	CreatePassWay();
	
	for (auto room : divideRoom) {
		SetAllChip(room[0], room[1], room[2], room[3]);
	}
	sumRoomNum = divideRoom.size();
}

//------------------------------------------------------------------------------------------------------------
void MapManager::SetShop()
{

	//�ݒu���W�����[�J�����W�Ƃ��Ď擾����
	tnl::Vector3 localPos = game_manager->SetStartPosition(GameManager::SetStartPositionType::STAIR);

	ResetChip(localPos.x, localPos.y, MapChip::MapType::SHOP);


}

//------------------------------------------------------------------------------------------------------------
tnl::Vector3 MapManager::GetRoomValue(int room_value)
{
	int x = abs(divideRoom[room_value][2] - divideRoom[room_value][0]);
	int y = abs(divideRoom[room_value][3] - divideRoom[room_value][1]);
	return tnl::Vector3(x + 1, y + 1, 0);
}

//------------------------------------------------------------------------------------------------------------
tnl::Vector3 MapManager::GetRoomStartPos(int room_value)
{
	int Left = divideRoom[room_value][0];
	int Up = divideRoom[room_value][1];
	return tnl::Vector3(Left, Up, 0);
}


//------------------------------------------------------------------------------------------------------------
//����̃}�b�v���W�������̂ǂ����ɑ��݂��邩�m�F����֐� �Ԃ�l�͕����ԍ�
int MapManager::CheckIsThere(int x, int y)
{
	bool isThere = false;
	//�S�Ă̕����̍��W���`�F�b�N
	//x,y�����͈͓̔��ɂ���Ȃ炻�̕����ɑ��݂���
	for (auto room : divideRoom) {
		if (x > room[2])continue;
		if (y > room[3])continue;
		if (x >= room[0] && y >= room[1]) {
			return room[4];
			isThere = true;
		}
	}

	return OUTOFRANGE;
}

//------------------------------------------------------------------------------------------------------------
void MapManager::SetCornerPos(int roomNum, tnl::Vector3& LeftTop, tnl::Vector3& RightBottom)
{
	LeftTop.x = static_cast<float>(divideRoom[roomNum][0]);
	LeftTop.y = static_cast<float>(divideRoom[roomNum][1]);
	RightBottom.x = static_cast<float>(divideRoom[roomNum][2]);
	RightBottom.y = static_cast<float>(divideRoom[roomNum][3]);
}


//------------------------------------------------------------------------------------------------------------
void MapManager::AddStairList(tnl::Vector3 pos)
{
	stairs.emplace_back(pos);
}

//------------------------------------------------------------------------------------------------------------
void MapManager::GetAllChip(int roomNum, std::vector<std::vector<int>>& chips)
{

	tnl::Vector3 hoge = GetRoomValue(roomNum);
	//�z�����U���Z�b�g
	chips.clear();

	//�����̏c�̑傫���Ŕz��̑傫����ݒ�
	chips.resize(hoge.y);

	int Left = divideRoom[roomNum][0];
	int Up = divideRoom[roomNum][1];
	int Right = divideRoom[roomNum][2];
	int Down = divideRoom[roomNum][3];

	int g = 0;
	for (int i = Up; i <= Down; ++i) {
		for (int k = Left; k <= Right; ++k) {
			chips[g].emplace_back(GetChip(k, i));
		}
		++g;
	}

}

//------------------------------------------------------------------------------------------------------------
//���ꂼ��̑Ή������}�b�v�`�b�v���Z�b�g����
void MapManager::ResetChip(int x, int y, int SetChip)
{
	if (IsOutOfRange(x, y))return;
	ground[y][x] = SetChip;
}

//------------------------------------------------------------------------------------------------------------
//�}�b�v�`��
void MapManager::MapDraw(const hm::Camera& camera, float graphicssize, double ratio)
{
	//map�̕`����W�̌v�Z

	//�f�o�b�O���[�h�̎�----------------------------------------------------
	if (game_manager->GetIsDebug()) {
		int count = 0;
		for (auto hoge : divideArea) {

			int x1 = (hoge[0] - 1) * graphicssize - camera.cameraPos.x;
			int y1 = (hoge[1] - 1) * graphicssize - camera.cameraPos.y;
			int x2 = (hoge[2] + 1) * graphicssize - camera.cameraPos.x;
			int y2 = (hoge[3] + 1) * graphicssize - camera.cameraPos.y;

			DrawBox(x1, y1, x2, y2, colors[count], true);

			count = (count + 1) % 5;
		}
		for (auto hoge : divideLine) {
			int x1 = hoge[0] * graphicssize - camera.cameraPos.x;
			int y1 = hoge[1] * graphicssize - camera.cameraPos.y;
			int x2 = hoge[2] * graphicssize - camera.cameraPos.x;
			int y2 = hoge[3] * graphicssize - camera.cameraPos.y;


			DrawLine(x1, y1, x2, y2, -1);
		}
		//**
	}



	//�ʏ�̏����̎�----------------------------------------------------
	//�}�b�v�`�b�v���i�[���Ă��郊�X�g����(�`�b�v���A�`�b�v�����W�������Ă���)
	for (auto chip : game_manager->GetObjectManager()->GetAllMapChipList()) {
		chip->MapChipDraw(camera, graphicssize, ratio, chip->start_map_handle);
	}
		

	//debug
	if (game_manager->GetIsDebug()) {
		DrawAllRoomPos(divideRoom);
	}

}

//------------------------------------------------------------------------------------------------------------
//�~�j�}�b�v�̕`��
void MapManager::MiniMapDraw(const hm::Camera& camera)
{
	
	int x = 0;
	int y = 0;

	for (int i = 0; i < ground.size(); ++i) {
		for (int k = 0; k < ground[i].size(); ++k) {
			if (visited[i][k] == false) {
				x += 8;
				continue;
			}
			if (ground[i][k] == MapChip::MapType::PASSWAY) {
				DrawRotaGraph(x, y, 0.4, 0, miniMapChip[0], false);
			}
			else if (ground[i][k] == MapChip::MapType::STAIRS) {
				DrawRotaGraph(x, y, 0.4, 0, miniMapChip[0], false);
				DrawRotaGraph(x, y, 0.4, 0, miniMapChip[1], false);
			}
			else if (ground[i][k] == MapChip::MapType::SHOP) {
				DrawRotaGraph(x, y, 0.4, 0, miniMapChip[0], false);
				DrawRotaGraph(x, y, 0.4, 0, miniMapChip[2], false);
			}
			x += 8;
		}
		x = 0;
		y += 8;
	}


	
	//minimap�ɕ`�悷��v���C���[�̍��W���}�b�v���W�ɕϊ�����v�Z
	//player_pos��chara_in_map�Ɠ���
	tnl::Vector3 player_pos = game_manager->WorldToLocalPos(SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos());
	//�~�j�}�b�v�Ƀv���C���[�̈ʒu��`��
	DrawRotaGraph(player_pos.x * 8, player_pos.y * 8 , 0.4, 0, miniPlayer, true);


}

//------------------------------------------------------------------------------------------------------------
bool MapManager::IsOutOfRange(int x, int y)
{
	if (x <= -1 || width <= x)return true;
	if (y <= -1 || height <= y)return true;
	return false;
}

//------------------------------------------------------------------------------------------------------------
//�G���A����
void MapManager::SetDivideArea(int Left, int Up, int Right, int Down, int Id)
{
	divideArea.emplace_back();
	int size = divideArea.size();
	//���ɏ����i�[
	divideArea[size - 1].emplace_back(Left);
	divideArea[size - 1].emplace_back(Up);
	divideArea[size - 1].emplace_back(Right);
	divideArea[size - 1].emplace_back(Down);
	divideArea[size - 1].emplace_back(Id);

}

//------------------------------------------------------------------------------------------------------------
void MapManager::SetLargeDivideArea(int Left, int Up, int Right, int Down, int Id)
{
	dumpDivideArea.emplace_back();
	int size = dumpDivideArea.size();
	//���ɏ����i�[
	dumpDivideArea[size - 1].emplace_back(Left);
	dumpDivideArea[size - 1].emplace_back(Up);
	dumpDivideArea[size - 1].emplace_back(Right);
	dumpDivideArea[size - 1].emplace_back(Down);
	dumpDivideArea[size - 1].emplace_back(Id);
}

//------------------------------------------------------------------------------------------------------------
//������
void MapManager::SetDivideLine(int Start_x, int Start_y, int Goal_x, int Goal_y, int Dir)
{
	divideLine.emplace_back();
	int size = divideLine.size();
	//�������̏����i�[
	divideLine[size - 1].emplace_back(Start_x);
	divideLine[size - 1].emplace_back(Start_y);
	divideLine[size - 1].emplace_back(Goal_x);
	divideLine[size - 1].emplace_back(Goal_y);
	divideLine[size - 1].emplace_back(Dir);
}

//------------------------------------------------------------------------------------------------------------
//��������
void MapManager::SetDivideRoom(int Left, int Up, int Right, int Down, int RoomId)
{
	divideRoom.emplace_back();
	int size = divideRoom.size();
	//�����̏����i�[
	divideRoom[size - 1].emplace_back(Left);
	divideRoom[size - 1].emplace_back(Up);
	divideRoom[size - 1].emplace_back(Right);
	divideRoom[size - 1].emplace_back(Down);
	divideRoom[size - 1].emplace_back(RoomId);
	//SetAllChip(Left, Up, Right, Down);
}

//------------------------------------------------------------------------------------------------------------
void MapManager::AreaDivide()
{
	int lowerWidth = 0;
	int upperWidth = width - 1;

	int lowerHeight = 0;
	int upperHeight = height - 1;

	int roomId = 0;



	while (1) {
		if (roomId > 0) {
			//�����̍Œᕝ��苷�������番�����Ȃ�
			if (dumpDivideArea[roomId - 1][2] - dumpDivideArea[roomId - 1][0] < 2 * ROOMMINWIDTH + 4) {
				break;
			}
			if (dumpDivideArea[roomId - 1][3] - dumpDivideArea[roomId - 1][1] < 2 * ROOMMINHEIGHT + 4) {
				break;
			}
		}
		//�ő啔�����ɒB�����番������߂�
		if (roomId > ROOMMAXNUM) {
			break;
		}


		//�������W
		int dividePoint = 0;

		//���������畔���܂ł͍Œ�2����

		//�ŏ��̕�����������
		if (!doneFirstDivide) {
			//���������c���傫��������
			if ((upperWidth - lowerWidth) > (upperHeight - lowerHeight)) {
				//���̂ǂ����ŏc�ɐ؂�
				dividePoint = game_manager->GetRandValue((lowerWidth + 2 + ROOMMINWIDTH), (upperWidth - ROOMMINWIDTH - 2));

				//������荶�Ȃ� �����̕������������A�E�̕����͍L��
				if (dividePoint < (upperWidth / 2)) {
					//����������o�^����
					SetDivideArea(lowerWidth + 1, lowerHeight + 1, dividePoint - 2, upperHeight - 1, roomId);
					//�傫������o�^����
					SetLargeDivideArea(dividePoint + 1, lowerHeight - 1, upperWidth - 1, upperHeight - 1, roomId);
				}
				//�������E�Ȃ� ���E�̕������������A���̕����͍L��
				else {
					//����������o�^����
					SetDivideArea(dividePoint + 2, lowerHeight + 1, upperWidth - 1, upperHeight - 1, roomId);
					//�傫������o�^����
					SetLargeDivideArea(lowerWidth + 1, lowerHeight - 1, dividePoint - 1, upperHeight - 1, roomId);
				}
				//������������
				SetDivideLine(dividePoint, lowerHeight + 1, dividePoint, upperHeight - 1, VERTICAL);
			}
			//�����c�������傫��������
			else {
				//�c�̂ǂ����ŉ��ɂ���
				dividePoint = game_manager->GetRandValue((lowerHeight + 2 + ROOMMINHEIGHT), (upperHeight - ROOMMINHEIGHT - 2));
				//��������Ȃ� ����̕������������A���̕����͍L��
				if (dividePoint < (upperHeight / 2)) {
					//����������o�^����
					SetDivideArea(lowerWidth + 1, lowerHeight + 1, upperWidth - 1, dividePoint - 2, roomId);
					//�傫������o�^����
					SetLargeDivideArea(lowerWidth + 1, dividePoint + 1, upperWidth - 1, upperHeight - 1, roomId);

				}
				//������艺�Ȃ� ����̕������傫���A���̕����͏�����
				else {
					//����������o�^����
					SetDivideArea(lowerWidth + 1, dividePoint + 2, upperWidth - 1, upperHeight - 1, roomId);
					//�傫������o�^����
					SetLargeDivideArea(lowerWidth + 1, lowerHeight + 1, upperWidth - 1, dividePoint - 1, roomId);
				}

				//������������
				SetDivideLine(lowerWidth + 1, dividePoint, upperWidth - 1, dividePoint, HORIZONTAL);
			}
			//�ŏ��̕������I������@�ȍ~��SetLargeDivideArea�ɓo�^���ꂽ�������X�ɕ������Ă���
			doneFirstDivide = true;
			++roomId;
			tnl::DebugTrace("\nroonId:%d\n", roomId);
			continue;
		}

		else
		{
			//��O�ɓo�^���ꂽ�����������̃f�[�^
			int lastLeft = divideArea[roomId - 1][0];
			int lastUp = divideArea[roomId - 1][1];
			int lastRight = divideArea[roomId - 1][2];
			int lastDown = divideArea[roomId - 1][3];

			int dir = divideLine[roomId - 1][4];

			/*int prev_2_Left = divideArea[roomId - 2][0];
			int prev_2_Up = divideArea[roomId - 2][1];
			int prev_2_Right = divideArea[roomId - 2][2];
			int prev_2_Down = divideArea[roomId - 2][3];*/

			//���񕪊�����G���A�̃f�[�^
			int left = dumpDivideArea[roomId - 1][0];
			int up = dumpDivideArea[roomId - 1][1];
			int right = dumpDivideArea[roomId - 1][2];
			int down = dumpDivideArea[roomId - 1][3];

			//���������c���傫��������
			if ((right - left) > (down - up)) {
				int midPoint = (left + right) / 2;
				//1�O�̃G���A�����ɂ��邩�E�ɂ��邩�m���߂�
				//��O�̕������c�̏ꍇ�͕����_�ŏꍇ����
				//��O���c�̕����@���@�����������O�̃G���A���ׂ荇���悤�ɐ؂�Ȃ���΂����Ȃ�
				if (dir == VERTICAL) {

					//1�O�̃G���A�̉E����������G���A�̍���菬������1�O�̃G���A�͍��ɂ���
					if (lastRight < left) {

						//�����_�͕�������G���A�̍����ɂȂ���΂����Ȃ�
						dividePoint = game_manager->GetRandValue((left + 2 + ROOMMINWIDTH), (midPoint/* - roomMinWidth - 2*/));
						//�K�������G���A�̍����̕������������Ȃ�
						//�����������̓o�^
						SetDivideArea(left + 1, up + 1, dividePoint - 2, down - 1, roomId);
						//�傫�������̓o�^
						SetLargeDivideArea(dividePoint + 1, up + 1, right - 1, down - 1, roomId);
					}
					//1�O�̃G���A���E�ɂ���
					else {
						//�����_�͉E���ɂȂ���΂����Ȃ�
						dividePoint = game_manager->GetRandValue((midPoint /*+ 2 + roomMinWidth*/), (right - ROOMMINWIDTH - 2));
						//�K���E���̕������������Ȃ�
						//����������o�^����
						SetDivideArea(dividePoint + 2, up + 1, right - 1, down - 1, roomId);
						//�傫������o�^����
						SetLargeDivideArea(left + 1, up + 1, dividePoint - 1, down - 1, roomId);

					}
				}
				else {
					//��O�����̕����@���@�����������ǂ��؂��Ă��ׂ荇��
					dividePoint = game_manager->GetRandValue((left + 2 + ROOMMINWIDTH), (right - ROOMMINWIDTH - 2));
					//������荶�Ȃ� �����̕������������A�E�̕����͍L��
					if (dividePoint < ((right + left) / 2)) {
						//����������o�^����
						SetDivideArea(left + 1, up + 1, dividePoint - 2, down - 1, roomId);
						//�傫������o�^����
						SetLargeDivideArea(dividePoint + 1, up + 1, right - 1, down - 1, roomId);
					}
					//�������E�Ȃ� ���E�̕������������A���̕����͍L��
					else {
						//����������o�^����
						SetDivideArea(dividePoint + 2, up + 1, right - 1, down - 1, roomId);
						//�傫������o�^����
						SetLargeDivideArea(left + 1, up + 1, dividePoint - 1, down - 1, roomId);
					}
				}
				//������������
				SetDivideLine(dividePoint, up + 1, dividePoint, down - 1, VERTICAL);
			}
			//�����c�������傫��������
			else {
				int midPoint = (up + down) / 2;
				//1�O�̃G���A����ɂ��邩���ɂ��邩�m���߂�
				// 1�O�̕��������Ȃ番���_��1�O�̃G���A���
				if (dir == HORIZONTAL) {


					//��O�̃G���A�̏オ�����G���A�̉������傫����Έ�O�̃G���A�͉��ɂ���
					if (down < lastUp) {
						//�����_�͕�������G���A�̉����ɂȂ���΂����Ȃ�
						dividePoint = game_manager->GetRandValue((midPoint + 2 + ROOMMINHEIGHT), (down - ROOMMINHEIGHT - 2));
						//�K�������G���A�̉����̕������������Ȃ�
						//�����������̓o�^

						SetDivideArea(left + 1, dividePoint + 2, right - 1, down - 1, roomId);
						//�傫�������̓o�^
						SetLargeDivideArea(left + 1, up + 1, right - 1, dividePoint - 1, roomId);
					}
					//��O�̃G���A����ɂ���
					else {
						//�����_�͕�������G���A�̏㑤�ɂȂ���΂����Ȃ�
						dividePoint = game_manager->GetRandValue((up + 2 + ROOMMINHEIGHT), (midPoint - ROOMMINHEIGHT - 2));
						//�K���㑤�̕������������Ȃ�
						//����������o�^����
						SetDivideArea(left + 1, up + 1, right - 1, dividePoint - 2, roomId);
						//�傫������o�^����
						SetLargeDivideArea(left + 1, dividePoint + 1, right - 1, down - 1, roomId);

					}
				}
				// 1�O�̕������c�Ȃ番���_�ǂ��ł��G���A�ׂ͗荇��
				else {
					dividePoint = game_manager->GetRandValue((up + 2 + ROOMMINWIDTH), (down - ROOMMINWIDTH - 2));
					//������艺�Ȃ� �����̕������������A��̕����͍L��
					if (dividePoint < ((up + down) / 2)) {
						//����������o�^����
						SetDivideArea(left + 1, up + 1, right - 1, dividePoint - 2, roomId);
						//�傫������o�^����
						SetLargeDivideArea(left + 1, dividePoint + 2, right - 1, down - 1, roomId);
					}
					//��������Ȃ� ����̕������������A���̕����͍L��
					else {
						//����������o�^����
						SetDivideArea(left + 1, dividePoint + 2, right - 1, down - 1, roomId);
						//�傫������o�^����
						SetLargeDivideArea(left + 1, up + 1, right - 1, dividePoint - 2, roomId);
					}
				}
				//������������
				SetDivideLine(left + 1, dividePoint, right - 1, dividePoint, HORIZONTAL);
			}
			++roomId;
			tnl::DebugTrace("\nroonId:%d\n", roomId);
			continue;
		}

	}
}

//------------------------------------------------------------------------------------------------------------
void MapManager::CreateRoom()
{
	for (auto area : divideArea) {

		int left = area[0];
		int up = area[1];
		int right = area[2];
		int down = area[3];
		int id = area[4];

		int roomLeft	= game_manager->GetRandValue(left + 2, right - ROOMMINWIDTH + 2);
		int roomRight	= game_manager->GetRandValue(roomLeft + ROOMMINWIDTH - 2, right - 2);
		int roomUp		= game_manager->GetRandValue(up + 2, down - ROOMMINHEIGHT + 2);
		int roomDown	= game_manager->GetRandValue(roomUp + ROOMMINHEIGHT - 2, down - 2);

		SetDivideRoom(roomLeft, roomUp, roomRight, roomDown, id);
	}

}


//------------------------------------------------------------------------------------------------------------
//�}�b�v�ɕ������m���Ȃ��ʘH���쐬����֐�
//�ʘH�̐��͕����̐�+1
void MapManager::CreatePassWay()
{
	int count = 0;
	int size = divideLine.size() - 1;

	//divideLine�z������[�v���āA�e�������̏����擾����
	//�������̓}�b�v�𕪊������Ƃ��ɂł�����ŁA�ʘH�̊�ɂ���
	for (auto line : divideLine) {
		tnl::DebugTrace("\n�ʉ�%d���\n", count);
		if (count >= size)break;
		//�������̗��[�̍��W�ƕ����i�c�������j
		int startX = line[0];
		int startY = line[1];
		int goalX = line[2];
		int goalY = line[3];
		int dir = line[4];

		//�����̏��̎擾
		//divideRoom�z�񂩂�A�������̑O��ɂ��镔���̏����擾����
		//�����̏��ɂ́A����ƉE���̍��W�ƕ����̔ԍ�������
		std::vector<int> roomBefore = divideRoom[count];
		std::vector<int> roomAfter = divideRoom[count + 1];

		//�O
		//leftBefore  : ��
		int leftBefore = roomBefore[0];
		//upBefore	  : ��
		int upBefore = roomBefore[1];
		//rightBefore : �E
		int rightBefore = roomBefore[2];
		//downBefore  : ��
		int downBefore = roomBefore[3];
		

		//��
		//leftBefore  : ��
		int leftAfter = roomAfter[0];
		//upBefore	  : ��
		int upAfter = roomAfter[1];
		//rightBefore : �E
		int rightAfter = roomAfter[2];
		//downBefore  : ��
		int downAfter = roomAfter[3];
		

		//���������c�Ȃ�
		if (dir == VERTICAL) {
			//��������ʘH�𐶂₷�ʒu������
			//�ʘH�̈ʒu�́A�����̓����Ƀ����_���ɑI�΂��
			//y���W
			int passWayBefore = game_manager->GetRandValue(upBefore + 1, downBefore - 1);
			int passWayAfter = game_manager->GetRandValue(upAfter + 1, downAfter - 1);

			//�����̑��Έʒu�ŏꍇ����
			//�����O�̕�����������̕�����荶�ɂ����
			if (leftBefore < leftAfter) {
				//�ʘH�̊J�n�n�_��SetRoomWayPoint�֐��ŕۑ�
				//�O�̕���
				game_manager->SetRoomWayPoint(tnl::Vector3(rightBefore + 1, passWayBefore, 0), roomBefore[4]);
				//���Ƃ̕���
				game_manager->SetRoomWayPoint(tnl::Vector3(leftAfter - 1, passWayAfter, 0), roomAfter[4]);

				//�O�̕������番�����ɒ��������������
				SetAllChip(rightBefore, passWayBefore, startX, passWayBefore);
				//���������炠�Ƃ̕����֒��������������
				SetAllChip(startX, passWayAfter, leftAfter, passWayAfter);
			}
			else {
				//�ʘH�̊J�n�n�_��SetRoomWayPoint�֐��ŕۑ�
				//�O�̕���
				game_manager->SetRoomWayPoint(tnl::Vector3(leftBefore - 1, passWayBefore, 0), roomBefore[4]);
				//���Ƃ̕���
				game_manager->SetRoomWayPoint(tnl::Vector3(rightAfter + 1, passWayAfter, 0), roomAfter[4]);
				//SetAllChip�֐����Ăяo���āA�������番�����Ɍ������Ē����̒ʘH���쐬����
				//�܂��A����������ʂ̕����Ɍ������Ă������̒ʘH���쐬����
				SetAllChip(rightAfter, passWayAfter, startX, passWayAfter);
				SetAllChip(startX, passWayBefore, leftBefore, passWayBefore);
			}
			//��������ɂ���ʘH�̈ʒu�𒲐����āA�������ɉ����ĒʘH���쐬����
			//�ʘH�̈ʒu�́A�������琶�₵���ʘH�̈ʒu�̂����A���������Ƒ傫�����ɍ��킹��
			if (passWayBefore > passWayAfter)std::swap(passWayBefore, passWayAfter);
			SetAllChip(startX, passWayBefore, startX, passWayAfter);
		}
		//�����������Ȃ�
		else {
			//��������ʘH�𐶂₷�ʒu������
			//�ʘH�̈ʒu�́A�����̓����Ƀ����_���ɑI�΂��
			//x���W
			int passWayBefore = game_manager->GetRandValue(leftBefore + 1, rightBefore - 1);
			int passWayAfter = game_manager->GetRandValue(leftAfter + 1, rightAfter - 1);

			//�����̑��Έʒu�ŏꍇ����
			//�O�̕�����肠�Ƃ̕��������ɂ����
			if (upBefore < upAfter) {
				//�ʘH�̊J�n�n�_��ۑ�����
				//�O�̕���
				game_manager->SetRoomWayPoint(tnl::Vector3(passWayBefore, downBefore + 1, 0), roomBefore[4]);
				//���Ƃ̕���
				game_manager->SetRoomWayPoint(tnl::Vector3(passWayAfter, upAfter - 1, 0), roomAfter[4]);
				//SetAllChip�֐����Ăяo���āA�������番�����Ɍ������Ē����̒ʘH���쐬����
				//�܂��A����������ʂ̕����Ɍ������Ă������̒ʘH���쐬����
				SetAllChip(passWayBefore, downBefore, passWayBefore, startY);
				SetAllChip(passWayAfter, startY, passWayAfter, upAfter);
			}
			else {
				//�ʘH�̊J�n�n�_��ۑ�����
				//�O�̕���
				game_manager->SetRoomWayPoint(tnl::Vector3(passWayBefore, upBefore - 1, 0), roomBefore[4]);
				//���Ƃ̕���
				game_manager->SetRoomWayPoint(tnl::Vector3(passWayAfter, downAfter + 1, 0), roomAfter[4]);
				//SetAllChip�֐����Ăяo���āA�������番�����Ɍ������Ē����̒ʘH���쐬����
				//�܂��A����������ʂ̕����Ɍ������Ă������̒ʘH���쐬����
				SetAllChip(passWayAfter, downAfter, passWayAfter, startY);
				SetAllChip(passWayBefore, startY, passWayBefore, upBefore);
			}
			//��������ɂ���ʘH�̈ʒu�𒲐����āA�������ɉ����ĒʘH���쐬����
			//�ʘH�̈ʒu�́A�������琶�₵���ʘH�̈ʒu�̂����A���������Ƒ傫�����ɍ��킹��
			if (passWayBefore > passWayAfter)std::swap(passWayBefore, passWayAfter);
			SetAllChip(passWayBefore, startY, passWayAfter, startY);
		}
		count++;
	}
	DebugManager::DebugString("\n�ʏ�ʘH�쐬�����f�o�b�O\n");

	{	//�ŏ��̕����ƍŌ�̕����ɑ΂��āA�ǉ��̒ʘH���쐬����
		//����́A�}�b�v�̒[�ɂ��镔���ɂ��ʘH���Ȃ��邽��
		//RandomPoint�֐��ŁA��������ʘH�𐶂₷�ʒu�������_���ɑI��
		//�ʘH�̕����́A�����̈ʒu�ɉ����Č��܂�
		//CreateSecondWay�֐����Ăяo���āA�ǉ��̒ʘH���쐬����
		//���̊֐��́A�ʘH�̕����ɉ����āA�}�b�v�̒[�܂ŒʘH��L�΂��A���̒ʘH�ƌ�������Ƃ���Ŏ~�߂�
		// 
		//0:��, 1 : �E, 2 : ��, 3 : ��
		WayDir dir = WayDir::UP;
		//�ŏ��̕����̏o���_���擾����
		tnl::Vector3 start = RandomPoint(0, dir);

		//CreateSecondWay�֐����Ăяo���āA�ǉ��̒ʘH���쐬����
		//���̊֐��́A�ʘH�̕����ɉ����āA�}�b�v�̒[�܂ŒʘH��L�΂��A���̒ʘH�ƌ�������Ƃ���Ŏ~�߂�
		CreateSecondWay(start.x, start.y, dir, 0);

		DebugManager::DebugString("\n�ŏ��̕����ǉ��ʘH�쐬�����f�o�b�O\n");

		//�Ō�̕����̏o���_���擾����
		int lastroomId = divideRoom.back()[4];
		WayDir lastdir = WayDir::UP;
		tnl::Vector3 laststart = RandomPoint(lastroomId, lastdir);

		//CreateSecondWay�֐����Ăяo���āA�ǉ��̒ʘH���쐬����
		//���̊֐��́A�ʘH�̕����ɉ����āA�}�b�v�̒[�܂ŒʘH��L�΂��A���̒ʘH�ƌ�������Ƃ���Ŏ~�߂�
		CreateSecondWay(laststart.x, laststart.y, lastdir, lastroomId);

	}
}

//------------------------------------------------------------------------------------------------------------
//0:��, 1 : �E, 2 : ��, 3 : ��
//�Ō�̕����̏ꍇ�͎擾���镪�����͈�O�̕���������Ȃ��Ƃ����Ȃ�
tnl::Vector3 MapManager::RandomPoint(int roomId, WayDir& dir)
{
	//*************************************************
	// 
	//				�Ō�̕����̏ꍇ
	// 
	//************************************************* 
	 
	//�����ԍ����ŏ��̕����ł͂Ȃ����`�F�b�N
	if (roomId != 0) {
		//divideLine�z�񂩂�A�Ō�̕����̒��O�ɂ��镪�����̕������擾���܂�
		if (divideLine[roomId - 2][4] == VERTICAL) {
			//��������x���W�ƕ�����x���W���ׂ�
			//��������蕔���̍����傫����Ε������͍��ɂ���
			if (divideLine[roomId - 2][0] < divideRoom[roomId][0]) {
				int point = 0;

				int debug = 0;
				while (1) {
					//�����̍��[�̂ǂ�����y���W
					point = game_manager->GetRandValue(divideRoom[roomId][1], divideRoom[roomId][3]);
					//GetChip�֐����Ăяo���āA�ʘH�̈ʒu�ɗאڂ���}�b�v�`�b�v���ʘH�łȂ����Ƃ��m�F����
					if ((GetChip(divideRoom[roomId][0] - 1, point) == 0 && GetChip(divideRoom[roomId][0] - 1, point - 1) == 0 && GetChip(divideRoom[roomId][0] - 1, point + 1)) == 0) break;
					//�����ʘH������΁A�K���ȍ��W�ł͂Ȃ��ƃf�o�b�O�o�͂��āA�ʂ̈ʒu��I��
					else {
						debug++;
						tnl::DebugTrace("\n�K�����W�ł͂Ȃ�%d���(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
						if (debug > 50)break;
					}
				}
				//�ʘH�̕����𕪊����ƒ�������悤�ɐݒ肷��
				//���������c�Ȃ�A�ʘH�͍����E�ɐL�т�B�����������Ȃ�A�ʘH�͏ォ���ɐL�т�
				//SetRoomWayPoint�֐��ŁA�ʘH�̊J�n�n�_��ۑ�����
				//����́A�����ɓ��B���邽�߂̃E�F�C�|�C���g�Ƃ��Ďg����
				//����́ACreateSecondWay�֐��ɓn����āA�ʘH�̍쐬�𑱂���
				dir = WayDir::LEFT;
				int pointX = divideRoom[roomId][0];
				game_manager->SetRoomWayPoint(tnl::Vector3(pointX - 1, point, 0), divideRoom[roomId][4]);
				//�ʘH�̊J�n�n�_��tnl::Vector3�^�ŕԂ��܂��B
				return tnl::Vector3(pointX, point, 0);
			}
			else {
				int point = 0;
				int debug = 0;
				while (1) {
					//�����̉E�[�̂ǂ�����y���W
					point = game_manager->GetRandValue(divideRoom[roomId][1], divideRoom[roomId][3]);
					//GetChip�֐����Ăяo���āA�ʘH�̈ʒu�ɗאڂ���}�b�v�`�b�v���ʘH�łȂ����Ƃ��m�F����
					if ((GetChip(divideRoom[roomId][0] + 1, point)) == 0 && (GetChip(divideRoom[roomId][0] + 1, point - 1)) == 0 && (GetChip(divideRoom[roomId][0] + 1, point + 1)) == 0) break;
					//�����ʘH������΁A�K���ȍ��W�ł͂Ȃ��ƃf�o�b�O�o�͂��āA�ʂ̈ʒu��I��
					else {
						debug++;
						tnl::DebugTrace("\n�K�����W�ł͂Ȃ�%d���(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
						if (debug > 50)break;
					}
				}
				//�ʘH�̕����𕪊����ƒ�������悤�ɐݒ肷��
				//���������c�Ȃ�A�ʘH�͍����E�ɐL�т�B�����������Ȃ�A�ʘH�͏ォ���ɐL�т�
				//SetRoomWayPoint�֐��ŁA�ʘH�̊J�n�n�_��ۑ�����
				//����́A�����ɓ��B���邽�߂̃E�F�C�|�C���g�Ƃ��Ďg����
				//����́ACreateSecondWay�֐��ɓn����āA�ʘH�̍쐬�𑱂���
				dir = WayDir::RIGHT;
				int pointX = divideRoom[roomId][2];
				game_manager->SetRoomWayPoint(tnl::Vector3(pointX + 1, point, 0), divideRoom[roomId][4]);
				//�ʘH�̊J�n�n�_��tnl::Vector3�^�ŕԂ��܂��B
				return tnl::Vector3(pointX, point, 0);
			}
		}

		//*********���Ȃ�ォ��*********HORIZONTAL
		else {
			//��������y���W�ƕ�����y���W���ׂ�
			//��������蕔���̏オ�傫����Ε������͏�ɂ���
			if (divideLine[roomId - 2][1] < divideRoom[roomId][1]) {
				int point = 0;
				int debug = 0;
				while (1) {
					//�����̏�[�̂ǂ�����x���W
					point = game_manager->GetRandValue(divideRoom[roomId][0], divideRoom[roomId][2]);
					//GetChip�֐����Ăяo���āA�ʘH�̈ʒu�ɗאڂ���}�b�v�`�b�v���ʘH�łȂ����Ƃ��m�F����
					if ((GetChip(point, divideRoom[roomId][1] - 1)) == 0 && (GetChip(point - 1, divideRoom[roomId][1] - 1)) == 0 && (GetChip(point + 1, divideRoom[roomId][1] - 1)) == 0) break;
					//�����ʘH������΁A�K���ȍ��W�ł͂Ȃ��ƃf�o�b�O�o�͂��āA�ʂ̈ʒu��I��
					else {
						debug++;
						tnl::DebugTrace("\n�K�����W�ł͂Ȃ�%d���(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
						if (debug > 50)break;
					}
				}
				//�ʘH�̕����𕪊����ƒ�������悤�ɐݒ肷��
				//���������c�Ȃ�A�ʘH�͍����E�ɐL�т�B�����������Ȃ�A�ʘH�͏ォ���ɐL�т�
				//SetRoomWayPoint�֐��ŁA�ʘH�̊J�n�n�_��ۑ�����
				//����́A�����ɓ��B���邽�߂̃E�F�C�|�C���g�Ƃ��Ďg����
				//����́ACreateSecondWay�֐��ɓn����āA�ʘH�̍쐬�𑱂���
				dir = WayDir::UP;
				int pointY = divideRoom[roomId][1];
				game_manager->SetRoomWayPoint(tnl::Vector3(point, pointY - 1, 0), divideRoom[roomId][4]);
				//�ʘH�̊J�n�n�_��tnl::Vector3�^�ŕԂ��܂��B
				return tnl::Vector3(point, pointY, 0);
			}
			else {
				int point = 0;
				int debug = 0;
				while (1) {
					//�����̉��[�̂ǂ�����x���W
					point = game_manager->GetRandValue(divideRoom[roomId][0], divideRoom[roomId][2]);
					//GetChip�֐����Ăяo���āA�ʘH�̈ʒu�ɗאڂ���}�b�v�`�b�v���ʘH�łȂ����Ƃ��m�F����
					if ((GetChip(point, divideRoom[roomId][3] + 1)) == 0 && (GetChip(point - 1, divideRoom[roomId][3] + 1)) == 0 && (GetChip(point + 1, divideRoom[roomId][3] + 1)) == 0) break;
					//�����ʘH������΁A�K���ȍ��W�ł͂Ȃ��ƃf�o�b�O�o�͂��āA�ʂ̈ʒu��I��
					else {
						debug++;
						tnl::DebugTrace("\n�K�����W�ł͂Ȃ�%d���(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
						if (debug > 50)break;
					}
				}
				//�ʘH�̕����𕪊����ƒ�������悤�ɐݒ肷��
				//���������c�Ȃ�A�ʘH�͍����E�ɐL�т�B�����������Ȃ�A�ʘH�͏ォ���ɐL�т�
				//SetRoomWayPoint�֐��ŁA�ʘH�̊J�n�n�_��ۑ�����
				//����́A�����ɓ��B���邽�߂̃E�F�C�|�C���g�Ƃ��Ďg����
				//����́ACreateSecondWay�֐��ɓn����āA�ʘH�̍쐬�𑱂���
				dir = WayDir::DOWN;
				int pointY = divideRoom[roomId][3];
				game_manager->SetRoomWayPoint(tnl::Vector3(point, pointY + 1, 0), divideRoom[roomId][4]);
				//�ʘH�̊J�n�n�_��tnl::Vector3�^�ŕԂ��܂��B
				return tnl::Vector3(point, pointY, 0);
			}
		}

		//*************************************************
		// 
		//				�Ō�̕����̏ꍇ
		// 
		//************************************************* 
	}
	


	//*************************************************
	// 
	//				�ŏ��̕����̏ꍇ
	// 
	//************************************************* 

	//*********���������c�Ȃ獶���E***********
	//�����̕������猩�ĕ��������ǂ��ɂ��邩���ׂ�
	if (divideLine[roomId][4] == VERTICAL) {
		//��������x���W�ƕ�����x���W���ׂ�
		//��������蕔���̍����傫����Ε������͍��ɂ���
		if (divideLine[roomId][0] < divideRoom[roomId][0]) {
			int point = 0;

			int debug = 0;
			while (1) {
				//�����̍��[�̂ǂ�����y���W
				point = game_manager->GetRandValue(divideRoom[roomId][1], divideRoom[roomId][3]);
				//GetChip�֐����Ăяo���āA�ʘH�̈ʒu�ɗאڂ���}�b�v�`�b�v���ʘH�łȂ����Ƃ��m�F����
				if ((GetChip(divideRoom[roomId][0] - 1, point)) == 0 && (GetChip(divideRoom[roomId][0] - 1, point + 1)) == 0 && (GetChip(divideRoom[roomId][0] - 1, point - 1)) == 0) break;
				else {
					debug++;
					tnl::DebugTrace("\n�K�����W�ł͂Ȃ�%d���(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
					if (debug > 50)break;
				}
			}
			//�ʘH�̕����𕪊����ƒ�������悤�ɐݒ肷��
			//���������c�Ȃ�A�ʘH�͍����E�ɐL�т�B�����������Ȃ�A�ʘH�͏ォ���ɐL�т�
			//SetRoomWayPoint�֐��ŁA�ʘH�̊J�n�n�_��ۑ�����
			//����́A�����ɓ��B���邽�߂̃E�F�C�|�C���g�Ƃ��Ďg����
			//����́ACreateSecondWay�֐��ɓn����āA�ʘH�̍쐬�𑱂���
			dir = WayDir::LEFT;
			int pointX = divideRoom[roomId][0];
			game_manager->SetRoomWayPoint(tnl::Vector3(pointX - 1, point, 0), divideRoom[roomId][4]);
			//�ʘH�̊J�n�n�_��tnl::Vector3�^�ŕԂ��܂��B
			return tnl::Vector3(pointX, point, 0);
		}
		else {
			int point = 0;

			int debug = 0;
			while (1) {
				//�����̉E�[�̂ǂ�����y���W
				point = game_manager->GetRandValue(divideRoom[roomId][1], divideRoom[roomId][3]);
				//GetChip�֐����Ăяo���āA�ʘH�̈ʒu�ɗאڂ���}�b�v�`�b�v���ʘH�łȂ����Ƃ��m�F����
				if ((GetChip(divideRoom[roomId][2] + 1, point)) == 0 && (GetChip(divideRoom[roomId][2] + 1, point - 1)) == 0 && (GetChip(divideRoom[roomId][2] + 1, point + 1)) == 0) break;
				else {
					debug++;
					tnl::DebugTrace("\n�K�����W�ł͂Ȃ�%d���(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
					if (debug > 50)break;
				}
			}
			//�ʘH�̕����𕪊����ƒ�������悤�ɐݒ肷��
			//���������c�Ȃ�A�ʘH�͍����E�ɐL�т�B�����������Ȃ�A�ʘH�͏ォ���ɐL�т�
			//SetRoomWayPoint�֐��ŁA�ʘH�̊J�n�n�_��ۑ�����
			//����́A�����ɓ��B���邽�߂̃E�F�C�|�C���g�Ƃ��Ďg����
			//����́ACreateSecondWay�֐��ɓn����āA�ʘH�̍쐬�𑱂���
			dir = WayDir::RIGHT;
			int pointX = divideRoom[roomId][2];
			game_manager->SetRoomWayPoint(tnl::Vector3(pointX + 1, point, 0), divideRoom[roomId][4]);
			//�ʘH�̊J�n�n�_��tnl::Vector3�^�ŕԂ��܂��B
			return tnl::Vector3(pointX, point, 0);
		}
	}
	//*********���Ȃ�ォ��*********HORIZONTAL
	else {
		//��������y���W�ƕ�����y���W���ׂ�
		//��������蕔���̏オ�傫����Ε������͏�ɂ���
		if (divideLine[roomId][1] < divideRoom[roomId][1]) {
			int point = 0;
			int debug = 0;
			while (1) {
				//�����̏�[�̂ǂ�����y���W
				point = game_manager->GetRandValue(divideRoom[roomId][0], divideRoom[roomId][2]);
				//GetChip�֐����Ăяo���āA�ʘH�̈ʒu�ɗאڂ���}�b�v�`�b�v���ʘH�łȂ����Ƃ��m�F����
				if ((GetChip(point, divideRoom[roomId][1] - 1)) == 0 && (GetChip(point - 1, divideRoom[roomId][1] - 1)) == 0 && (GetChip(point + 1, divideRoom[roomId][1] - 1)) == 0) break;
				else {
					debug++;
					tnl::DebugTrace("\n�K�����W�ł͂Ȃ�%d���(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
					if (debug > 50)break;
				}
			}
			dir = WayDir::UP;
			//�ʘH�̕����𕪊����ƒ�������悤�ɐݒ肷��
			//���������c�Ȃ�A�ʘH�͍����E�ɐL�т�B�����������Ȃ�A�ʘH�͏ォ���ɐL�т�
			//SetRoomWayPoint�֐��ŁA�ʘH�̊J�n�n�_��ۑ�����
			//����́A�����ɓ��B���邽�߂̃E�F�C�|�C���g�Ƃ��Ďg����
			//����́ACreateSecondWay�֐��ɓn����āA�ʘH�̍쐬�𑱂���
			int pointY = divideRoom[roomId][1];
			game_manager->SetRoomWayPoint(tnl::Vector3(point, pointY - 1, 0), divideRoom[roomId][4]);
			//�ʘH�̊J�n�n�_��tnl::Vector3�^�ŕԂ��܂��B
			return tnl::Vector3(point, pointY, 0);
		}
		else {
			int point = 0;
			int debug = 0;
			while (1) {
				//�����̉��[�̂ǂ�����y���W
				point = game_manager->GetRandValue(divideRoom[roomId][0], divideRoom[roomId][2]);
				//GetChip�֐����Ăяo���āA�ʘH�̈ʒu�ɗאڂ���}�b�v�`�b�v���ʘH�łȂ����Ƃ��m�F����
				if ((GetChip(point, divideRoom[roomId][3] + 1)) == 0 && (GetChip(point - 1, divideRoom[roomId][3] + 1)) == 0 && (GetChip(point + 1, divideRoom[roomId][3] + 1)) == 0) break;
				else {
					debug++;
					tnl::DebugTrace("\n�K�����W�ł͂Ȃ�%d���(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
					if (debug > 50)break;
				}
			}
			dir = WayDir::DOWN;
			//�ʘH�̕����𕪊����ƒ�������悤�ɐݒ肷��
			//���������c�Ȃ�A�ʘH�͍����E�ɐL�т�B�����������Ȃ�A�ʘH�͏ォ���ɐL�т�
			//SetRoomWayPoint�֐��ŁA�ʘH�̊J�n�n�_��ۑ�����
			//����́A�����ɓ��B���邽�߂̃E�F�C�|�C���g�Ƃ��Ďg����
			//����́ACreateSecondWay�֐��ɓn����āA�ʘH�̍쐬�𑱂���
			int pointY = divideRoom[roomId][3];
			game_manager->SetRoomWayPoint(tnl::Vector3(point, pointY + 1, 0), divideRoom[roomId][4]);
			//�ʘH�̊J�n�n�_��tnl::Vector3�^�ŕԂ��܂��B
			return tnl::Vector3(point, pointY, 0);
		}

		//*************************************************
		// 
		//				�ŏ��̕����̏ꍇ
		// 
		//************************************************* 
	}
	

	//return tnl::Vector3(-1, -1, -1);
}

//------------------------------------------------------------------------------------------------------------
//�}�b�v�ɒǉ��̒ʘH���쐬����֐�
bool MapManager::CreateSecondWay(int x, int y, WayDir dir, int roomId)
{
	if (x - 1 < 0 || y - 1 < 0)return true;
	if (x + 1 > game_manager->MAPWIDTH - 1 || y + 1 > game_manager->MAPHEIGHT - 1)return true;
	int chip = 0;
	//���Ɉړ�����`�b�v�̈ʒu
	int setX = 0;
	int setY = 0;

	//��̏ꍇ��x�͌��݂�x�ƕς��Ȃ����Ay���W�́A��ɏオ�邽��-1�����l
	if (dir == WayDir::UP) {
		setX = x;
		setY = y - 1;
		//�v�Z��̈ʒu�̃`�b�v��ʘH�ɕύX
		ResetChip(setX, setY, MapChip::MapType::PASSWAY/* EXPASSWAY*/);
		//chip = GetChip(x, y - 1);
	}
	//�E�̏ꍇ�́Ax���W���E��1�����Ay���W�͌��݂̂܂�
	else if (dir == WayDir::RIGHT) {
		setX = x + 1;
		setY = y;
		//�v�Z��̈ʒu�̃`�b�v��ʘH�ɕύX
		ResetChip(setX, setY, MapChip::MapType::PASSWAY/*EXPASSWAY*/);
		//chip = GetChip(x + 1, y);
	}
	//���̏ꍇ��x���W�͕ω����Ȃ����Ay���W��+1�����ʒu�ɕω�
	else if (dir == WayDir::DOWN) {
		setX = x;
		setY = y + 1;
		//�v�Z��̈ʒu�̃`�b�v��ʘH�ɕύX
		ResetChip(setX, setY, MapChip::MapType::PASSWAY /*EXPASSWAY*/);
		//chip = GetChip(x, (y + 1));
	}
	//���̏ꍇ�́Ax���W��-1�����ʒu�ɂȂ�Ay���W�͕ω����Ȃ�
	else if (dir == WayDir::LEFT) {
		setX = x - 1;
		setY = y;
		//�v�Z��̈ʒu�̃`�b�v��ʘH�ɕύX
		ResetChip(setX, setY, MapChip::MapType::PASSWAY/* EXPASSWAY*/);
		//chip = GetChip((x - 1), y);
	}
	//�����ʘH�̉��̂ǂ��炩���A�i�s�����ɒʘH�������
	if (CheckChip(setX, setY, dir))
	{
		//�֐����I��
		return true;
	}

	//
	//�����ԍ��ɉ����āA�ׂ̕����̕ӂƒʘH�̍��W����v���邩�ǂ������`�F�b�N����
	//������v����ꍇ�́A�ʘH�̕����𒼊p�ɋȂ��āACreateSecondWay�֐����ċA�I�ɌĂяo��
	//��������x,y���W�Ɨׂ̕����̕ӂ̂ǂ�����x,�܂���y���W�������Ȃ璼�p�ɋȂ���
	//
 
	//�ŏ��̕����Ȃ�
	if (roomId == 0) {
		//�ォ���Ȃ�
		if (dir == WayDir::UP || dir == WayDir::DOWN) {
			//�����͈͓̔��ɂ��邩�m�F
			if (divideRoom[roomId + 2][1]<setY && divideRoom[roomId + 2][3]>setY) {
				//���������ɂ���Ȃ�(�ω����x���W��菬�����ꍇ)
				if (divideRoom[roomId + 2][2] < setX) {
					return CreateSecondWay(setX, setY, WayDir::DOWN, roomId);
				}
				else {
					return CreateSecondWay(setX, setY, WayDir::RIGHT, roomId);
				}
			}
		}
		//�E�����Ȃ�
		else if (dir == WayDir::RIGHT || dir == WayDir::LEFT) {
			if (divideRoom[roomId + 2][0]<setX && divideRoom[roomId + 2][2]>setX) {
				//��������ɂ���
				if (divideRoom[roomId + 2][3] < setY) {
					return CreateSecondWay(setX, setY, WayDir::UP, roomId);
				}
				else {
					return CreateSecondWay(setX, setY, WayDir::DOWN, roomId);
				}
			}
		}
	}
	//�Ō�̂ւ�Ȃ�
	else if (roomId == divideRoom.back()[4]) {
		if (dir == WayDir::UP || dir == WayDir::DOWN) {
			//��
			if (divideRoom[roomId - 2][1]<setY && divideRoom[roomId - 2][3]>setY) {
				//���������ɂ���
				if (divideRoom[roomId - 2][2] < setX) {
					return CreateSecondWay(setX, setY, WayDir::LEFT, roomId);
				}
				else {
					return CreateSecondWay(setX, setY, WayDir::RIGHT, roomId);
				}
			}
		}
		else if (dir == WayDir::RIGHT || dir == WayDir::DOWN) {
			if (divideRoom[roomId - 2][0]<setX && divideRoom[roomId - 2][2]>setX) {
				//��������ɂ���
				if (divideRoom[roomId - 2][3] < setY) {
					return CreateSecondWay(setX, setY, WayDir::UP, roomId);
				}
				else {
					return CreateSecondWay(setX, setY, WayDir::DOWN, roomId);
				}
			}
		}
	}
	//��v���Ȃ��ꍇ�́A�ʘH�̕��������̂܂܂ɂ��āACreateSecondWay�֐����ċA�I�ɌĂяo��
	return CreateSecondWay(setX, setY, dir, roomId);
}

//------------------------------------------------------------------------------------------------------------
//�}�b�v�ɒǉ��̒ʘH���쐬����Ƃ��ɁA�ʘH�̈ʒu�ɑ��̒ʘH�╔�������邩�ǂ������`�F�b�N����֐�
bool MapManager::CheckChip(int x, int y, WayDir nextDir)
{
	//���W���}�b�v�͈̔͊O�ɂȂ�ꍇ�́Atrue��Ԃ��Ċ֐����I��
	if (x + 1 > game_manager->MAPWIDTH - 1 || y + 1 > game_manager->MAPHEIGHT - 1 || x <= 1 || y <= 1)return true;

	//�ʘH�̈ʒu�ɑ��̒ʘH�╔�������邩�ǂ����̃t���O
	bool isThere = false;
	if (nextDir == WayDir::UP) {
		//�אڂ���ʒu�ɂ��낪����΃t���O��ture�ɕω�������
		if (GetChip(x - 1, y) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x + 1, y) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x, y - 1) == MapChip::MapType::PASSWAY)isThere = true;

		int num = 0;
		//�����̗L�����`�F�b�N
		if (num = CheckIsThere(x - 1, y) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
		else if (num = CheckIsThere(x + 1, y) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
		else if (num = CheckIsThere(x, y - 1) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
	}
	else if (nextDir == WayDir::RIGHT) {
		//�אڂ���ʒu�ɂ��낪����΃t���O��ture�ɕω�������
		if (GetChip(x, y - 1) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x, y + 1) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x + 1, y) == MapChip::MapType::PASSWAY)isThere = true;

		int num = 0;
		//�����̗L�����`�F�b�N
		if (num = CheckIsThere(x, y - 1) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
		else if (num = CheckIsThere(x, y + 1) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
		else if (num = CheckIsThere(x + 1, y) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
	}
	else if (nextDir == WayDir::DOWN) {
		//�אڂ���ʒu�ɂ��낪����΃t���O��ture�ɕω�������
		if (GetChip(x - 1, y) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x + 1, y) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x, y + 1) == MapChip::MapType::PASSWAY)isThere = true;

		int num = 0;
		//�����̗L�����`�F�b�N
		if (num = CheckIsThere(x - 1, y) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
		else if (num = CheckIsThere(x + 1, y) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
		else if (num = CheckIsThere(x, y + 1) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
	}
	else if (nextDir == WayDir::LEFT) {
		//�אڂ���ʒu�ɂ��낪����΃t���O��ture�ɕω�������
		if (GetChip(x, y - 1) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x, y + 1) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x - 1, y) == MapChip::MapType::PASSWAY)isThere = true;

		int num = 0;
		//�����̗L�����`�F�b�N
		if (num = CheckIsThere(x, y - 1) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
		else if (num = CheckIsThere(x, y + 1) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
		else if (num = CheckIsThere(x - 1, y) != OUTOFRANGE)
		{
			//if (num != 0)gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num - 1);
			//else gManager->SetRoomWayPoint(tnl::Vector3(x, y, 0), num);
			isThere = true;
		}
	}
	if (isThere)return true;
	return false;
}

//------------------------------------------------------------------------------------------------------------
//�����ƕ������Ȃ��ʘH�̍쐬�ɍۂ��āA�ʘH�̈ʒu�ɑ��̒ʘH���Ȃ����ǂ������`�F�b�N����֐�
bool MapManager::CheckPassWay(int roomPos_set, int roomPos_moveStart, int roomPos_moveGoal, WayDir dir)
{
	//�ʘH�̈ʒu�ɂق��̒ʘH���Ȃ����ǂ����̃t���O
	bool check = true;
	for (int i = roomPos_moveStart; i < roomPos_moveGoal; ++i) {
		//�ʘH�������for���𔲂���
		if (dir == WayDir::UP) {
			//������E�֒���
			if (roomPos_set - 1 < 0)break;
			if (GetChip(i, roomPos_set - 1) == MapChip::MapType::PASSWAY) {
				check = false;
				break;
			}
		}
		else if (dir == WayDir::RIGHT) {
			//�ォ�牺�֒���
			if (roomPos_set + 1 > width)break;
			if (GetChip(roomPos_set + 1, i) == MapChip::MapType::PASSWAY) {
				check = false;
				break;
			}
		}
		else if (dir == WayDir::DOWN) {
			//������E�֒���
			if (roomPos_set + 1 > height)break;
			if (GetChip(i, roomPos_set + 1) == MapChip::MapType::PASSWAY) {
				check = false;
				break;
			}
		}
		else if (dir == WayDir::LEFT) {
			//�ォ�牺�֒���
			if (roomPos_set - 1 < 0)break;
			if (GetChip(roomPos_set - 1, i) == MapChip::MapType::PASSWAY) {
				check = false;
				break;
			}
		}
	}
	//for���𔲂�����Acheck�̒l��Ԃ��܂��Bcheck��true�Ȃ�A�ʘH�̈ʒu�ɑ��̒ʘH���Ȃ����Ƃ�\��
	//check��false�Ȃ�A�ʘH�̈ʒu�ɑ��̒ʘH�����邱�Ƃ�\��
	return check;
}

//------------------------------------------------------------------------------------------------------------
std::string MapManager::GetColorName(int code)
{
	if (code == color_red)return "��";
	else if (code == color_green)return "��";
	else if (code == color_blue)return "��";
	else if (code == color_yellow)return "��";
	else if (code == color_purple)return "��";

	else return "�G���[";
}

//------------------------------------------------------------------------------------------------------------
//�Z�����猩�Ď��͂ɒʘH�����邩�ǂ����m���߂�֐�
bool MapManager::CheckAround(int x, int y)
{
	int up = 0;
	int right = 0;
	int botton = 0;
	int left = 0;

	int leftup = 0;
	int leftbotton = 0;
	int rightup = 0;
	int rightbotton = 0;

	//
	//if���ŁA�w�肳�ꂽ���W���}�b�v�͈͓̔��ɂ��邩�ǂ������m�F����A�����͈͊O�Ȃ�A�������X�L�b�v����
	//ground�Ƃ����񎟌��z�񂩂�A�w�肳�ꂽ���W�̎��͂̃}�b�v�`�b�v�̒l���擾���āA�Ή�����ϐ��ɑ��
	//���ׂĂ̕ϐ��̒l�����v���āA1�ȏ�Ȃ�true��Ԃ��܂��B����́A�w�肳�ꂽ���W�̎��͂ɒʘH�╔�������邱�Ƃ�\���܂��B
	//�����łȂ���΁Afalse��Ԃ��܂��B����́A�w�肳�ꂽ���W�̎��͂ɒʘH�╔�����Ȃ����Ƃ�\���܂��B
	//
	if (x > 0) {
		if (y > 0)leftup = ground[x - 1][y - 1];
		left = ground[x - 1][y];
		if (y + 1 < game_manager->MAPWIDTH)leftbotton = ground[x - 1][y + 1];
	}

	if (y > 0)up = ground[x][y - 1];
	if (y + 1 < game_manager->MAPWIDTH)botton = ground[x][y + 1];

	if (x + 1 < game_manager->MAPHEIGHT) {
		if (y > 0)rightup = ground[x + 1][y - 1];
		right = ground[x + 1][y];
		if (y + 1 < game_manager->MAPWIDTH)rightbotton = ground[x + 1][y + 1];
	}
	if (up + right + botton + left + leftup + leftbotton + rightup + rightbotton >= 1)return true;
	else return false;

}

//------------------------------------------------------------------------------------------------------------
//�Z��������͂̒ʘH���m�F���A�`�悷�ׂ��摜�����Ԃ��擾����֐� 
//�����ƕ������Ȃ��ʘH�̍쐬�ɍۂ��āA�ʘH�̈ʒu�ɑ��̒ʘH���Ȃ����ǂ������`�F�b�N����֐�
int MapManager::CheckAroundWay(int y, int x)
{
	bool up = false;
	bool right = false;
	bool botton = false;
	bool left = false;

	bool leftup = false;
	bool leftbotton = false;
	bool rightup = false;
	bool rightbotton = false;

	//
	//8�ߖT�̃Z�����ʘH�Ȃ�true,�����łȂ����false����
	//CheckThisCell�֐����Ăяo���āA�w�肳�ꂽ���W�̎��͂̃}�b�v�`�b�v���ʘH���ǂ����𔻒肵�āA�Ή�����ϐ��ɑ�����܂��B
	//CheckThisCell�֐��́A�����œn���ꂽ���W�̃}�b�v�`�b�v�̒l��PASSWAY,STAIRS,SHOP�Ȃ�true���A
	//														     �����łȂ����false��Ԃ��֐�
	//
	
	if (y > 0) {
		if (x > 0)leftup = CheckThisCell(y - 1, x - 1);
		if (x > 0)left = CheckThisCell(y, x - 1);
		if (x + 1 < game_manager->MAPWIDTH)leftbotton = CheckThisCell(y - 1, x + 1);
	}

	if (y > 0)up = CheckThisCell(y - 1, x);
	if (y + 1 < game_manager->MAPHEIGHT)botton = CheckThisCell(y + 1, x);

	if (y + 1 < game_manager->MAPHEIGHT) {
		if (x > 0)rightup = CheckThisCell(y + 1, x - 1);
		if (x + 1 < game_manager->MAPWIDTH) right = CheckThisCell(y, x + 1);
		if (x + 1 < game_manager->MAPWIDTH)rightbotton = CheckThisCell(y + 1, x + 1);
	}

	//���ׂĂ̕ϐ��̒l�����ƂɁA�ʘH�̈ʒu�������̂ǂ̕����ɂ����邩�𔻒肵�āA�Ή�����萔��Ԃ�
	if (left && leftbotton && leftup && right && rightup && rightbotton)return MapChip::GraphicType::ROOMWALL;//8 �����̃u���b�N

	else if (up && rightbotton)return MapChip::GraphicType::ROOMWALL;
	else if (up && left)return MapChip::GraphicType::ROOMWALL;
	else if (left && right)return MapChip::GraphicType::ROOMWALL;
	else if (up && rightup && right)return MapChip::GraphicType::ROOMWALL;

	//��,�E��,������true->�ʘH�Ȃ� �����̏㑤
	else if (botton)return MapChip::GraphicType::ROOMTOP;//0

	else if (left)return MapChip::GraphicType::ROOMRIGHT;//1
	else if (up)return MapChip::GraphicType::ROOMBOTTOM;//2
	else if (right)return MapChip::GraphicType::ROOMLEFT;//3

	else if (rightbotton && !up && !botton && !right && !left && !rightup && !leftup && !leftbotton)return MapChip::GraphicType::ROOMLEFTTOP;//4
	else if (leftbotton && !up && !botton && !right && !left && !rightup && !leftup && !rightbotton)return MapChip::GraphicType::ROOMRIGHTTOP;//5
	else if (rightup && !up && !botton && !right && !left && !leftbotton && !leftup && !rightbotton)return MapChip::GraphicType::ROOMLEFTBOTTOM;//6
	else if (leftup && !up && !botton && !right && !left && !rightup && !rightbotton && !leftbotton)return MapChip::GraphicType::ROOMRIGHTBOTTOM;//7


	//�����ʘH�̈ʒu�������̂ǂ̕����ɂ�������Ȃ��ꍇ�́AOUTOFRANGE��Ԃ�
	else return OUTOFRANGE;
}

//------------------------------------------------------------------------------------------------------------
//�����œn���ꂽ���W�̃}�b�v�`�b�v�̒l��PASSWAY,STAIRS,SHOP�Ȃ�true���A�����łȂ����false��Ԃ��֐�
bool MapManager::CheckThisCell(int x, int y)
{
	if (ground[x][y] == MapChip::MapType::PASSWAY || ground[x][y] == MapChip::MapType::STAIRS || ground[x][y] == MapChip::MapType::SHOP)return true;
	else return false;
}

//------------------------------------------------------------------------------------------------------------
//���\������Ă���S�Ă̕����̏㉺���E���W������`�悷��Debug�֐�
void MapManager::DrawAllRoomPos(std::vector<std::vector<int>>RoomList)
{
	int drawPosX = 10;
	int drawPosY = 10;

	//�� �� �E �� id
	for (auto room : RoomList) {
		for (auto area : divideArea) {
			DrawStringEx(drawPosX + 200 * room[4], drawPosY, -1, "roomId:%d", room[4]);
			DrawStringEx(drawPosX + 200 * room[4], drawPosY + 20, -1, "areaColor:%s", GetColorName(colors[room[4]]).c_str());
			DrawStringEx(drawPosX + 200 * room[4], drawPosY + 40, -1, "Left:%d", room[0]);
			DrawStringEx(drawPosX + 200 * room[4], drawPosY + 60, -1, "Up:%d", room[1]);
			DrawStringEx(drawPosX + 200 * room[4], drawPosY + 80, -1, "Right:%d", room[2]);
			DrawStringEx(drawPosX + 200 * room[4], drawPosY + 100, -1, "Down:%d", room[3]);

			DrawStringEx(drawPosX + 200 * area[4], drawPosY + 140, -1, "areaId:%d", area[4]);
			DrawStringEx(drawPosX + 200 * area[4], drawPosY + 160, -1, "Left:%d", area[0]);
			DrawStringEx(drawPosX + 200 * area[4], drawPosY + 180, -1, "Up:%d", area[1]);
			DrawStringEx(drawPosX + 200 * area[4], drawPosY + 200, -1, "Right:%d", area[2]);
			DrawStringEx(drawPosX + 200 * area[4], drawPosY + 220, -1, "Down:%d", area[3]);


		}
	}
}
