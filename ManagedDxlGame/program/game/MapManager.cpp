//-------------------------------------------------------
//既存のもの
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
//コンストラクタ
MapManager::MapManager(int map_width, int map_height, std::vector<int>handles, GameManager* game_manager_)
{
	game_manager = game_manager_;
	
	allchip.resize(handles.size());
	allchip = handles;

	//床
	floor = allchip[0];
	//壁
	wall = allchip[1];
	//top
	up = allchip[2];
	//bottom
	botton = allchip[3];
	//left
	left = allchip[4];
	//right
	right = allchip[5];

	//オートタイル用配列
	auto_tile_chip[0] = wall;
	auto_tile_chip[1] = up;
	auto_tile_chip[2] = right;
	auto_tile_chip[3] = botton;
	auto_tile_chip[4] = left;
	auto_tile_chip[5] = wall;
	auto_tile_chip[6] = wall;
	auto_tile_chip[7] = wall;
	auto_tile_chip[8] = wall;

	//ここもcsvから読み込んだほうがきれいな気がする
	stair = game_manager->LoadGraphEx("testgraphics/Stairs_.png");//階段

	shop = game_manager->LoadGraphEx("testgraphics/shop.png");//shop

	voidgh = game_manager->LoadGraphEx("testgraphics/void.png");

	//-------------------------------------------------------------------------------------------
	//minimap用
	miniMapChip[0] = game_manager->LoadGraphEx("testgraphics/mini_PassWay.png");
	miniMapChip[1] = game_manager->LoadGraphEx("testgraphics/mini_Stair.png");
	miniMapChip[2] = game_manager->LoadGraphEx("testgraphics/mini_shop.png");

	miniPlayer = game_manager->LoadGraphEx("testgraphics/mini_Player.png");
	//miniEnemy = game_manager->LoadGraphEx("graphics/mini_Enemy.png");

	//-------------------------------------------------------------------------------------------
	width = map_width; //default値は0
	height = map_height;//default値は0

	//縦の初期化
	ground.resize(height);
	visited.resize(height);
	for (auto v : ground)v.resize(width);
	for (auto v : visited)v.resize(width);

	for (int i = 0; i < height; i++) {
		ground[i].resize(width);
		visited[i].resize(width);
	}

	//-------------------------------------------------------------------------------------------
	//すべてを壁にする
	for (int i = 0; i < height; i++) {
		for (int k = 0; k < width; k++) {
			ground[i][k] = MapChip::MapType::WALL;
			visited[i][k] = false;
		}
	}

	//-------------------------------------------------------------------------------------------
	


}

//------------------------------------------------------------------------------------------------------------
//デストラクタ
MapManager::~MapManager()
{
	ground.clear();

}

//------------------------------------------------------------------------------------------------------------
// ミニマップ用部屋表示判定
// ローカル座標を引数にとって、その座標に対応する部屋を訪問済みにする関数
void MapManager::ChangeRoomVisit(tnl::Vector3 localpos)
{
	// 引数に渡された座標がマップ内のどこかを検索
	int check = CheckIsThere(localpos.x, localpos.y);
	// 範囲外(マップ範囲外)の場合return
	if (check == OUTOFRANGE)return;

	// ローカル座標に対応する、左上と、右下の座標を取得する
	SetCornerPos(check, leftupper, rightbotton);

	// rightbotton.yがマップの高さより小さく、rightbotton.xがマップの幅より小さければ
	// 右下の座標がマップの高さと幅よりも小さい場合は、左上と右下の座標を中心に、2マス分の余裕を持って、
	// visited配列の値をtrueに更新する 
	// これは、部屋の周囲の壁や通路も訪問済みにするため
	if (rightbotton.y < game_manager->MAPHEIGHT && rightbotton.x < game_manager->MAPWIDTH) {
		for (int i = leftupper.y - 2; i < rightbotton.y + 2; i++) {
			for (int k = leftupper.x - 2; k < rightbotton.x + 2; k++) {
				// visited配列の値をtrueに更新する
				visited[i][k] = true;
			}
		}
	}

	// 右下の座標がマップの高さや幅を超えている場合は、左上と右下の座標の範囲内で、
	// visited配列の値をtrueに更新します。
	// これは、マップの端にある部屋の場合に、範囲外の座標にアクセスしないようにするため
	else {
		for (int i = leftupper.y; i < rightbotton.y; i++) {
			for (int k = leftupper.x; k < rightbotton.x; k++) {
				// visited配列の値をtrueに更新する
				visited[i][k] = true;
			}
		}
	}

}

//------------------------------------------------------------------------------------------------------------
// ミニマップ用通路表示判定
// ローカル座標を引数にとって、その座標の周囲の通路を訪問済みにする関数
void MapManager::ChangeWayVisit(tnl::Vector3 localpos)
{
	//引数に渡された座標がマップ内のどこかを検索
	int check = CheckIsThere(localpos.x, localpos.y);

	//範囲外(マップ範囲外)の場合return
	if (check != OUTOFRANGE)return;

	//周囲2マスが範囲外であればreturn(通路のため2マスチェックで大丈夫)
	//左、下チェック
	if (IsOutOfRange(localpos.x - 2, localpos.y - 2))return;
	//右、上チェック
	if (IsOutOfRange(localpos.x + 2, localpos.y + 2))return;

	
	//x1 : 左　x2 : 右

	int x1 = localpos.x - 2;
	int x2 = localpos.x + 2;
	
	//y1 : 下　y2 : 上

	int y1 = localpos.y - 2;
	int y2 = localpos.y + 2;

	//左上と右下の座標の範囲内で、visited配列の値をtrueに更新する
	//これは、通路の一部を訪問済みにするため
	for (int i = y1; i <= y2; i++) {
		for (int k = x1; k <= x2; k++) {
			//visited配列の値をtrueに更新する
			visited[i][k] = true;
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//指定された範囲のマップチップをすべて通路に設定する関数
void MapManager::SetAllChip(int left, int up, int right, int down)
{
	//引数で渡された左上と右下の座標をもとに、二重のfor文で範囲内のすべてのマップチップにアクセスする
	for (int i = up; i <= down; i++) {
		for (int k = left; k <= right; k++) {

			//ResetChip関数を呼び出して、各マップチップの種類をPASSWAY（通路）に変更する
			ResetChip(k, i, MapChip::MapType::PASSWAY);
		}
	}
}

//------------------------------------------------------------------------------------------------------------
//マップの分割と部屋の作成、通路の作成を行う関数
void MapManager::AreaDivideStart(int Width, int Height, std::shared_ptr<MapManager> map)
{
	//マップ分割
	AreaDivide();
	//分割された空間に部屋を制作
	CreateRoom();

	//部屋の数だけウェイポイントのベクターを初期化
	game_manager->InitWayPointVector(GetRoomNum());
	DebugManager::DebugString("\n通路作成前デバッグ\n");

	CreatePassWay();
	
	for (auto room : divideRoom) {
		SetAllChip(room[0], room[1], room[2], room[3]);
	}
	sumRoomNum = divideRoom.size();
}

//------------------------------------------------------------------------------------------------------------
void MapManager::SetShop()
{

	//設置座標をローカル座標として取得する
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
//特定のマップ座標が部屋のどこかに存在するか確認する関数 返り値は部屋番号
int MapManager::CheckIsThere(int x, int y)
{
	bool isThere = false;
	//全ての部屋の座標をチェック
	//x,yがその範囲内にあるならその部屋に存在する
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
	//配列を一旦リセット
	chips.clear();

	//部屋の縦の大きさで配列の大きさを設定
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
//それぞれの対応したマップチップをセットする
void MapManager::ResetChip(int x, int y, int SetChip)
{
	if (IsOutOfRange(x, y))return;
	ground[y][x] = SetChip;
}

//------------------------------------------------------------------------------------------------------------
//マップ描画
void MapManager::MapDraw(const hm::Camera& camera, float graphicssize, double ratio)
{
	//mapの描画座標の計算

	//デバッグモードの時----------------------------------------------------
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



	//通常の処理の時----------------------------------------------------
	//マップチップを格納しているリストを回す(チップ情報、チップ元座標を持っている)
	for (auto chip : game_manager->GetObjectManager()->GetAllMapChipList()) {
		chip->MapChipDraw(camera, graphicssize, ratio, chip->start_map_handle);
	}
		

	//debug
	if (game_manager->GetIsDebug()) {
		DrawAllRoomPos(divideRoom);
	}

}

//------------------------------------------------------------------------------------------------------------
//ミニマップの描画
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


	
	//minimapに描画するプレイヤーの座標をマップ座標に変換する計算
	//player_posはchara_in_mapと同じ
	tnl::Vector3 player_pos = game_manager->WorldToLocalPos(SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos());
	//ミニマップにプレイヤーの位置を描画
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
//エリア分割
void MapManager::SetDivideArea(int Left, int Up, int Right, int Down, int Id)
{
	divideArea.emplace_back();
	int size = divideArea.size();
	//区画に情報を格納
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
	//区画に情報を格納
	dumpDivideArea[size - 1].emplace_back(Left);
	dumpDivideArea[size - 1].emplace_back(Up);
	dumpDivideArea[size - 1].emplace_back(Right);
	dumpDivideArea[size - 1].emplace_back(Down);
	dumpDivideArea[size - 1].emplace_back(Id);
}

//------------------------------------------------------------------------------------------------------------
//線分割
void MapManager::SetDivideLine(int Start_x, int Start_y, int Goal_x, int Goal_y, int Dir)
{
	divideLine.emplace_back();
	int size = divideLine.size();
	//分割線の情報を格納
	divideLine[size - 1].emplace_back(Start_x);
	divideLine[size - 1].emplace_back(Start_y);
	divideLine[size - 1].emplace_back(Goal_x);
	divideLine[size - 1].emplace_back(Goal_y);
	divideLine[size - 1].emplace_back(Dir);
}

//------------------------------------------------------------------------------------------------------------
//部屋分割
void MapManager::SetDivideRoom(int Left, int Up, int Right, int Down, int RoomId)
{
	divideRoom.emplace_back();
	int size = divideRoom.size();
	//部屋の情報を格納
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
			//部屋の最低幅より狭かったら分割しない
			if (dumpDivideArea[roomId - 1][2] - dumpDivideArea[roomId - 1][0] < 2 * ROOMMINWIDTH + 4) {
				break;
			}
			if (dumpDivideArea[roomId - 1][3] - dumpDivideArea[roomId - 1][1] < 2 * ROOMMINHEIGHT + 4) {
				break;
			}
		}
		//最大部屋数に達したら分割をやめる
		if (roomId > ROOMMAXNUM) {
			break;
		}


		//分割座標
		int dividePoint = 0;

		//分割線から部屋までは最低2離す

		//最初の分割だったら
		if (!doneFirstDivide) {
			//もし横が縦より大きかったら
			if ((upperWidth - lowerWidth) > (upperHeight - lowerHeight)) {
				//横のどこかで縦に切る
				dividePoint = game_manager->GetRandValue((lowerWidth + 2 + ROOMMINWIDTH), (upperWidth - ROOMMINWIDTH - 2));

				//半分より左なら →左の部屋が小さく、右の部屋は広い
				if (dividePoint < (upperWidth / 2)) {
					//小さい方を登録する
					SetDivideArea(lowerWidth + 1, lowerHeight + 1, dividePoint - 2, upperHeight - 1, roomId);
					//大きい方を登録する
					SetLargeDivideArea(dividePoint + 1, lowerHeight - 1, upperWidth - 1, upperHeight - 1, roomId);
				}
				//半分より右なら →右の部屋が小さく、左の部屋は広い
				else {
					//小さい方を登録する
					SetDivideArea(dividePoint + 2, lowerHeight + 1, upperWidth - 1, upperHeight - 1, roomId);
					//大きい方を登録する
					SetLargeDivideArea(lowerWidth + 1, lowerHeight - 1, dividePoint - 1, upperHeight - 1, roomId);
				}
				//分割線を引く
				SetDivideLine(dividePoint, lowerHeight + 1, dividePoint, upperHeight - 1, VERTICAL);
			}
			//もし縦が横より大きかったら
			else {
				//縦のどこかで横にきる
				dividePoint = game_manager->GetRandValue((lowerHeight + 2 + ROOMMINHEIGHT), (upperHeight - ROOMMINHEIGHT - 2));
				//半分より上なら →上の部屋が小さく、下の部屋は広い
				if (dividePoint < (upperHeight / 2)) {
					//小さい方を登録する
					SetDivideArea(lowerWidth + 1, lowerHeight + 1, upperWidth - 1, dividePoint - 2, roomId);
					//大きい方を登録する
					SetLargeDivideArea(lowerWidth + 1, dividePoint + 1, upperWidth - 1, upperHeight - 1, roomId);

				}
				//半分より下なら →上の部屋が大きく、下の部屋は小さい
				else {
					//小さい方を登録する
					SetDivideArea(lowerWidth + 1, dividePoint + 2, upperWidth - 1, upperHeight - 1, roomId);
					//大きい方を登録する
					SetLargeDivideArea(lowerWidth + 1, lowerHeight + 1, upperWidth - 1, dividePoint - 1, roomId);
				}

				//分割線を引く
				SetDivideLine(lowerWidth + 1, dividePoint, upperWidth - 1, dividePoint, HORIZONTAL);
			}
			//最初の分割を終了する　以降はSetLargeDivideAreaに登録された部屋を更に分割していく
			doneFirstDivide = true;
			++roomId;
			tnl::DebugTrace("\nroonId:%d\n", roomId);
			continue;
		}

		else
		{
			//一つ前に登録された小さい部屋のデータ
			int lastLeft = divideArea[roomId - 1][0];
			int lastUp = divideArea[roomId - 1][1];
			int lastRight = divideArea[roomId - 1][2];
			int lastDown = divideArea[roomId - 1][3];

			int dir = divideLine[roomId - 1][4];

			/*int prev_2_Left = divideArea[roomId - 2][0];
			int prev_2_Up = divideArea[roomId - 2][1];
			int prev_2_Right = divideArea[roomId - 2][2];
			int prev_2_Down = divideArea[roomId - 2][3];*/

			//今回分割するエリアのデータ
			int left = dumpDivideArea[roomId - 1][0];
			int up = dumpDivideArea[roomId - 1][1];
			int right = dumpDivideArea[roomId - 1][2];
			int down = dumpDivideArea[roomId - 1][3];

			//もし横が縦より大きかったら
			if ((right - left) > (down - up)) {
				int midPoint = (left + right) / 2;
				//1つ前のエリアが左にあるか右にあるか確かめる
				//一つ前の分割が縦の場合は分割点で場合分け
				//一つ前が縦の分割　かつ　横が長い→前のエリアが隣り合うように切らなければいけない
				if (dir == VERTICAL) {

					//1つ前のエリアの右が分割するエリアの左より小さい→1つ前のエリアは左にある
					if (lastRight < left) {

						//分割点は分割するエリアの左側になければいけない
						dividePoint = game_manager->GetRandValue((left + 2 + ROOMMINWIDTH), (midPoint/* - roomMinWidth - 2*/));
						//必ず分割エリアの左側の部屋が小さくなる
						//小さい部屋の登録
						SetDivideArea(left + 1, up + 1, dividePoint - 2, down - 1, roomId);
						//大きい部屋の登録
						SetLargeDivideArea(dividePoint + 1, up + 1, right - 1, down - 1, roomId);
					}
					//1つ前のエリアが右にある
					else {
						//分割点は右側になければいけない
						dividePoint = game_manager->GetRandValue((midPoint /*+ 2 + roomMinWidth*/), (right - ROOMMINWIDTH - 2));
						//必ず右側の部屋が小さくなる
						//小さい方を登録する
						SetDivideArea(dividePoint + 2, up + 1, right - 1, down - 1, roomId);
						//大きい方を登録する
						SetLargeDivideArea(left + 1, up + 1, dividePoint - 1, down - 1, roomId);

					}
				}
				else {
					//一つ前が横の分割　かつ　横が長い→どう切っても隣り合う
					dividePoint = game_manager->GetRandValue((left + 2 + ROOMMINWIDTH), (right - ROOMMINWIDTH - 2));
					//半分より左なら →左の部屋が小さく、右の部屋は広い
					if (dividePoint < ((right + left) / 2)) {
						//小さい方を登録する
						SetDivideArea(left + 1, up + 1, dividePoint - 2, down - 1, roomId);
						//大きい方を登録する
						SetLargeDivideArea(dividePoint + 1, up + 1, right - 1, down - 1, roomId);
					}
					//半分より右なら →右の部屋が小さく、左の部屋は広い
					else {
						//小さい方を登録する
						SetDivideArea(dividePoint + 2, up + 1, right - 1, down - 1, roomId);
						//大きい方を登録する
						SetLargeDivideArea(left + 1, up + 1, dividePoint - 1, down - 1, roomId);
					}
				}
				//分割線を引く
				SetDivideLine(dividePoint, up + 1, dividePoint, down - 1, VERTICAL);
			}
			//もし縦が横より大きかったら
			else {
				int midPoint = (up + down) / 2;
				//1つ前のエリアが上にあるか下にあるか確かめる
				// 1つ前の分割が横なら分割点は1つ前のエリア寄り
				if (dir == HORIZONTAL) {


					//一つ前のエリアの上が分割エリアの下よりも大きければ一つ前のエリアは下にある
					if (down < lastUp) {
						//分割点は分割するエリアの下側になければいけない
						dividePoint = game_manager->GetRandValue((midPoint + 2 + ROOMMINHEIGHT), (down - ROOMMINHEIGHT - 2));
						//必ず分割エリアの下側の部屋が小さくなる
						//小さい部屋の登録

						SetDivideArea(left + 1, dividePoint + 2, right - 1, down - 1, roomId);
						//大きい部屋の登録
						SetLargeDivideArea(left + 1, up + 1, right - 1, dividePoint - 1, roomId);
					}
					//一つ前のエリアが上にある
					else {
						//分割点は分割するエリアの上側になければいけない
						dividePoint = game_manager->GetRandValue((up + 2 + ROOMMINHEIGHT), (midPoint - ROOMMINHEIGHT - 2));
						//必ず上側の部屋が小さくなる
						//小さい方を登録する
						SetDivideArea(left + 1, up + 1, right - 1, dividePoint - 2, roomId);
						//大きい方を登録する
						SetLargeDivideArea(left + 1, dividePoint + 1, right - 1, down - 1, roomId);

					}
				}
				// 1つ前の分割が縦なら分割点どこでもエリアは隣り合う
				else {
					dividePoint = game_manager->GetRandValue((up + 2 + ROOMMINWIDTH), (down - ROOMMINWIDTH - 2));
					//半分より下なら →下の部屋が小さく、上の部屋は広い
					if (dividePoint < ((up + down) / 2)) {
						//小さい方を登録する
						SetDivideArea(left + 1, up + 1, right - 1, dividePoint - 2, roomId);
						//大きい方を登録する
						SetLargeDivideArea(left + 1, dividePoint + 2, right - 1, down - 1, roomId);
					}
					//半分より上なら →上の部屋が小さく、下の部屋は広い
					else {
						//小さい方を登録する
						SetDivideArea(left + 1, dividePoint + 2, right - 1, down - 1, roomId);
						//大きい方を登録する
						SetLargeDivideArea(left + 1, up + 1, right - 1, dividePoint - 2, roomId);
					}
				}
				//分割線を引く
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
//マップに部屋同士をつなぐ通路を作成する関数
//通路の数は部屋の数+1
void MapManager::CreatePassWay()
{
	int count = 0;
	int size = divideLine.size() - 1;

	//divideLine配列をループして、各分割線の情報を取得する
	//分割線はマップを分割したときにできる線で、通路の基準にする
	for (auto line : divideLine) {
		tnl::DebugTrace("\n通過%d回目\n", count);
		if (count >= size)break;
		//分割線の両端の座標と方向（縦か横か）
		int startX = line[0];
		int startY = line[1];
		int goalX = line[2];
		int goalY = line[3];
		int dir = line[4];

		//部屋の情報の取得
		//divideRoom配列から、分割線の前後にある部屋の情報を取得する
		//部屋の情報には、左上と右下の座標と部屋の番号がある
		std::vector<int> roomBefore = divideRoom[count];
		std::vector<int> roomAfter = divideRoom[count + 1];

		//前
		//leftBefore  : 左
		int leftBefore = roomBefore[0];
		//upBefore	  : 上
		int upBefore = roomBefore[1];
		//rightBefore : 右
		int rightBefore = roomBefore[2];
		//downBefore  : 下
		int downBefore = roomBefore[3];
		

		//後
		//leftBefore  : 左
		int leftAfter = roomAfter[0];
		//upBefore	  : 上
		int upAfter = roomAfter[1];
		//rightBefore : 右
		int rightAfter = roomAfter[2];
		//downBefore  : 下
		int downAfter = roomAfter[3];
		

		//分割線が縦なら
		if (dir == VERTICAL) {
			//部屋から通路を生やす位置を決定
			//通路の位置は、部屋の内部にランダムに選ばれる
			//y座標
			int passWayBefore = game_manager->GetRandValue(upBefore + 1, downBefore - 1);
			int passWayAfter = game_manager->GetRandValue(upAfter + 1, downAfter - 1);

			//部屋の相対位置で場合分け
			//分割前の部屋が分割後の部屋より左にあれば
			if (leftBefore < leftAfter) {
				//通路の開始地点をSetRoomWayPoint関数で保存
				//前の部屋
				game_manager->SetRoomWayPoint(tnl::Vector3(rightBefore + 1, passWayBefore, 0), roomBefore[4]);
				//あとの部屋
				game_manager->SetRoomWayPoint(tnl::Vector3(leftAfter - 1, passWayAfter, 0), roomAfter[4]);

				//前の部屋から分割線に直交する線を引く
				SetAllChip(rightBefore, passWayBefore, startX, passWayBefore);
				//分割線からあとの部屋へ直交する線を引く
				SetAllChip(startX, passWayAfter, leftAfter, passWayAfter);
			}
			else {
				//通路の開始地点をSetRoomWayPoint関数で保存
				//前の部屋
				game_manager->SetRoomWayPoint(tnl::Vector3(leftBefore - 1, passWayBefore, 0), roomBefore[4]);
				//あとの部屋
				game_manager->SetRoomWayPoint(tnl::Vector3(rightAfter + 1, passWayAfter, 0), roomAfter[4]);
				//SetAllChip関数を呼び出して、部屋から分割線に向かって直線の通路を作成する
				//また、分割線から別の部屋に向かっても直線の通路を作成する
				SetAllChip(rightAfter, passWayAfter, startX, passWayAfter);
				SetAllChip(startX, passWayBefore, leftBefore, passWayBefore);
			}
			//分割線上にある通路の位置を調整して、分割線に沿って通路を作成する
			//通路の位置は、部屋から生やした通路の位置のうち、小さい方と大きい方に合わせる
			if (passWayBefore > passWayAfter)std::swap(passWayBefore, passWayAfter);
			SetAllChip(startX, passWayBefore, startX, passWayAfter);
		}
		//分割線が横なら
		else {
			//部屋から通路を生やす位置を決定
			//通路の位置は、部屋の内部にランダムに選ばれる
			//x座標
			int passWayBefore = game_manager->GetRandValue(leftBefore + 1, rightBefore - 1);
			int passWayAfter = game_manager->GetRandValue(leftAfter + 1, rightAfter - 1);

			//部屋の相対位置で場合分け
			//前の部屋よりあとの部屋が下にあれば
			if (upBefore < upAfter) {
				//通路の開始地点を保存する
				//前の部屋
				game_manager->SetRoomWayPoint(tnl::Vector3(passWayBefore, downBefore + 1, 0), roomBefore[4]);
				//あとの部屋
				game_manager->SetRoomWayPoint(tnl::Vector3(passWayAfter, upAfter - 1, 0), roomAfter[4]);
				//SetAllChip関数を呼び出して、部屋から分割線に向かって直線の通路を作成する
				//また、分割線から別の部屋に向かっても直線の通路を作成する
				SetAllChip(passWayBefore, downBefore, passWayBefore, startY);
				SetAllChip(passWayAfter, startY, passWayAfter, upAfter);
			}
			else {
				//通路の開始地点を保存する
				//前の部屋
				game_manager->SetRoomWayPoint(tnl::Vector3(passWayBefore, upBefore - 1, 0), roomBefore[4]);
				//あとの部屋
				game_manager->SetRoomWayPoint(tnl::Vector3(passWayAfter, downAfter + 1, 0), roomAfter[4]);
				//SetAllChip関数を呼び出して、部屋から分割線に向かって直線の通路を作成する
				//また、分割線から別の部屋に向かっても直線の通路を作成する
				SetAllChip(passWayAfter, downAfter, passWayAfter, startY);
				SetAllChip(passWayBefore, startY, passWayBefore, upBefore);
			}
			//分割線上にある通路の位置を調整して、分割線に沿って通路を作成する
			//通路の位置は、部屋から生やした通路の位置のうち、小さい方と大きい方に合わせる
			if (passWayBefore > passWayAfter)std::swap(passWayBefore, passWayAfter);
			SetAllChip(passWayBefore, startY, passWayAfter, startY);
		}
		count++;
	}
	DebugManager::DebugString("\n通常通路作成完了デバッグ\n");

	{	//最初の部屋と最後の部屋に対して、追加の通路を作成する
		//これは、マップの端にある部屋にも通路をつなげるため
		//RandomPoint関数で、部屋から通路を生やす位置をランダムに選ぶ
		//通路の方向は、部屋の位置に応じて決まる
		//CreateSecondWay関数を呼び出して、追加の通路を作成する
		//この関数は、通路の方向に応じて、マップの端まで通路を伸ばし、他の通路と交差するところで止める
		// 
		//0:上, 1 : 右, 2 : 下, 3 : 左
		WayDir dir = WayDir::UP;
		//最初の部屋の出発点を取得する
		tnl::Vector3 start = RandomPoint(0, dir);

		//CreateSecondWay関数を呼び出して、追加の通路を作成する
		//この関数は、通路の方向に応じて、マップの端まで通路を伸ばし、他の通路と交差するところで止める
		CreateSecondWay(start.x, start.y, dir, 0);

		DebugManager::DebugString("\n最初の部屋追加通路作成完了デバッグ\n");

		//最後の部屋の出発点を取得する
		int lastroomId = divideRoom.back()[4];
		WayDir lastdir = WayDir::UP;
		tnl::Vector3 laststart = RandomPoint(lastroomId, lastdir);

		//CreateSecondWay関数を呼び出して、追加の通路を作成する
		//この関数は、通路の方向に応じて、マップの端まで通路を伸ばし、他の通路と交差するところで止める
		CreateSecondWay(laststart.x, laststart.y, lastdir, lastroomId);

	}
}

//------------------------------------------------------------------------------------------------------------
//0:上, 1 : 右, 2 : 下, 3 : 左
//最後の部屋の場合は取得する分割線は一つ前の分割線じゃないといけない
tnl::Vector3 MapManager::RandomPoint(int roomId, WayDir& dir)
{
	//*************************************************
	// 
	//				最後の部屋の場合
	// 
	//************************************************* 
	 
	//部屋番号が最初の部屋ではないかチェック
	if (roomId != 0) {
		//divideLine配列から、最後の部屋の直前にある分割線の方向を取得します
		if (divideLine[roomId - 2][4] == VERTICAL) {
			//分割線のx座標と部屋のx座標を比べる
			//分割線より部屋の左が大きければ分割線は左にある
			if (divideLine[roomId - 2][0] < divideRoom[roomId][0]) {
				int point = 0;

				int debug = 0;
				while (1) {
					//部屋の左端のどっかのy座標
					point = game_manager->GetRandValue(divideRoom[roomId][1], divideRoom[roomId][3]);
					//GetChip関数を呼び出して、通路の位置に隣接するマップチップが通路でないことを確認する
					if ((GetChip(divideRoom[roomId][0] - 1, point) == 0 && GetChip(divideRoom[roomId][0] - 1, point - 1) == 0 && GetChip(divideRoom[roomId][0] - 1, point + 1)) == 0) break;
					//もし通路があれば、適正な座標ではないとデバッグ出力して、別の位置を選ぶ
					else {
						debug++;
						tnl::DebugTrace("\n適正座標ではない%d回目(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
						if (debug > 50)break;
					}
				}
				//通路の方向を分割線と直交するように設定する
				//分割線が縦なら、通路は左か右に伸びる。分割線が横なら、通路は上か下に伸びる
				//SetRoomWayPoint関数で、通路の開始地点を保存する
				//これは、部屋に到達するためのウェイポイントとして使われる
				//これは、CreateSecondWay関数に渡されて、通路の作成を続ける
				dir = WayDir::LEFT;
				int pointX = divideRoom[roomId][0];
				game_manager->SetRoomWayPoint(tnl::Vector3(pointX - 1, point, 0), divideRoom[roomId][4]);
				//通路の開始地点をtnl::Vector3型で返します。
				return tnl::Vector3(pointX, point, 0);
			}
			else {
				int point = 0;
				int debug = 0;
				while (1) {
					//部屋の右端のどっかのy座標
					point = game_manager->GetRandValue(divideRoom[roomId][1], divideRoom[roomId][3]);
					//GetChip関数を呼び出して、通路の位置に隣接するマップチップが通路でないことを確認する
					if ((GetChip(divideRoom[roomId][0] + 1, point)) == 0 && (GetChip(divideRoom[roomId][0] + 1, point - 1)) == 0 && (GetChip(divideRoom[roomId][0] + 1, point + 1)) == 0) break;
					//もし通路があれば、適正な座標ではないとデバッグ出力して、別の位置を選ぶ
					else {
						debug++;
						tnl::DebugTrace("\n適正座標ではない%d回目(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
						if (debug > 50)break;
					}
				}
				//通路の方向を分割線と直交するように設定する
				//分割線が縦なら、通路は左か右に伸びる。分割線が横なら、通路は上か下に伸びる
				//SetRoomWayPoint関数で、通路の開始地点を保存する
				//これは、部屋に到達するためのウェイポイントとして使われる
				//これは、CreateSecondWay関数に渡されて、通路の作成を続ける
				dir = WayDir::RIGHT;
				int pointX = divideRoom[roomId][2];
				game_manager->SetRoomWayPoint(tnl::Vector3(pointX + 1, point, 0), divideRoom[roomId][4]);
				//通路の開始地点をtnl::Vector3型で返します。
				return tnl::Vector3(pointX, point, 0);
			}
		}

		//*********横なら上か下*********HORIZONTAL
		else {
			//分割線のy座標と部屋のy座標を比べる
			//分割線より部屋の上が大きければ分割線は上にある
			if (divideLine[roomId - 2][1] < divideRoom[roomId][1]) {
				int point = 0;
				int debug = 0;
				while (1) {
					//部屋の上端のどっかのx座標
					point = game_manager->GetRandValue(divideRoom[roomId][0], divideRoom[roomId][2]);
					//GetChip関数を呼び出して、通路の位置に隣接するマップチップが通路でないことを確認する
					if ((GetChip(point, divideRoom[roomId][1] - 1)) == 0 && (GetChip(point - 1, divideRoom[roomId][1] - 1)) == 0 && (GetChip(point + 1, divideRoom[roomId][1] - 1)) == 0) break;
					//もし通路があれば、適正な座標ではないとデバッグ出力して、別の位置を選ぶ
					else {
						debug++;
						tnl::DebugTrace("\n適正座標ではない%d回目(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
						if (debug > 50)break;
					}
				}
				//通路の方向を分割線と直交するように設定する
				//分割線が縦なら、通路は左か右に伸びる。分割線が横なら、通路は上か下に伸びる
				//SetRoomWayPoint関数で、通路の開始地点を保存する
				//これは、部屋に到達するためのウェイポイントとして使われる
				//これは、CreateSecondWay関数に渡されて、通路の作成を続ける
				dir = WayDir::UP;
				int pointY = divideRoom[roomId][1];
				game_manager->SetRoomWayPoint(tnl::Vector3(point, pointY - 1, 0), divideRoom[roomId][4]);
				//通路の開始地点をtnl::Vector3型で返します。
				return tnl::Vector3(point, pointY, 0);
			}
			else {
				int point = 0;
				int debug = 0;
				while (1) {
					//部屋の下端のどっかのx座標
					point = game_manager->GetRandValue(divideRoom[roomId][0], divideRoom[roomId][2]);
					//GetChip関数を呼び出して、通路の位置に隣接するマップチップが通路でないことを確認する
					if ((GetChip(point, divideRoom[roomId][3] + 1)) == 0 && (GetChip(point - 1, divideRoom[roomId][3] + 1)) == 0 && (GetChip(point + 1, divideRoom[roomId][3] + 1)) == 0) break;
					//もし通路があれば、適正な座標ではないとデバッグ出力して、別の位置を選ぶ
					else {
						debug++;
						tnl::DebugTrace("\n適正座標ではない%d回目(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
						if (debug > 50)break;
					}
				}
				//通路の方向を分割線と直交するように設定する
				//分割線が縦なら、通路は左か右に伸びる。分割線が横なら、通路は上か下に伸びる
				//SetRoomWayPoint関数で、通路の開始地点を保存する
				//これは、部屋に到達するためのウェイポイントとして使われる
				//これは、CreateSecondWay関数に渡されて、通路の作成を続ける
				dir = WayDir::DOWN;
				int pointY = divideRoom[roomId][3];
				game_manager->SetRoomWayPoint(tnl::Vector3(point, pointY + 1, 0), divideRoom[roomId][4]);
				//通路の開始地点をtnl::Vector3型で返します。
				return tnl::Vector3(point, pointY, 0);
			}
		}

		//*************************************************
		// 
		//				最後の部屋の場合
		// 
		//************************************************* 
	}
	


	//*************************************************
	// 
	//				最初の部屋の場合
	// 
	//************************************************* 

	//*********分割線が縦なら左か右***********
	//自分の部屋から見て分割線がどこにあるか調べる
	if (divideLine[roomId][4] == VERTICAL) {
		//分割線のx座標と部屋のx座標を比べる
		//分割線より部屋の左が大きければ分割線は左にある
		if (divideLine[roomId][0] < divideRoom[roomId][0]) {
			int point = 0;

			int debug = 0;
			while (1) {
				//部屋の左端のどっかのy座標
				point = game_manager->GetRandValue(divideRoom[roomId][1], divideRoom[roomId][3]);
				//GetChip関数を呼び出して、通路の位置に隣接するマップチップが通路でないことを確認する
				if ((GetChip(divideRoom[roomId][0] - 1, point)) == 0 && (GetChip(divideRoom[roomId][0] - 1, point + 1)) == 0 && (GetChip(divideRoom[roomId][0] - 1, point - 1)) == 0) break;
				else {
					debug++;
					tnl::DebugTrace("\n適正座標ではない%d回目(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
					if (debug > 50)break;
				}
			}
			//通路の方向を分割線と直交するように設定する
			//分割線が縦なら、通路は左か右に伸びる。分割線が横なら、通路は上か下に伸びる
			//SetRoomWayPoint関数で、通路の開始地点を保存する
			//これは、部屋に到達するためのウェイポイントとして使われる
			//これは、CreateSecondWay関数に渡されて、通路の作成を続ける
			dir = WayDir::LEFT;
			int pointX = divideRoom[roomId][0];
			game_manager->SetRoomWayPoint(tnl::Vector3(pointX - 1, point, 0), divideRoom[roomId][4]);
			//通路の開始地点をtnl::Vector3型で返します。
			return tnl::Vector3(pointX, point, 0);
		}
		else {
			int point = 0;

			int debug = 0;
			while (1) {
				//部屋の右端のどっかのy座標
				point = game_manager->GetRandValue(divideRoom[roomId][1], divideRoom[roomId][3]);
				//GetChip関数を呼び出して、通路の位置に隣接するマップチップが通路でないことを確認する
				if ((GetChip(divideRoom[roomId][2] + 1, point)) == 0 && (GetChip(divideRoom[roomId][2] + 1, point - 1)) == 0 && (GetChip(divideRoom[roomId][2] + 1, point + 1)) == 0) break;
				else {
					debug++;
					tnl::DebugTrace("\n適正座標ではない%d回目(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
					if (debug > 50)break;
				}
			}
			//通路の方向を分割線と直交するように設定する
			//分割線が縦なら、通路は左か右に伸びる。分割線が横なら、通路は上か下に伸びる
			//SetRoomWayPoint関数で、通路の開始地点を保存する
			//これは、部屋に到達するためのウェイポイントとして使われる
			//これは、CreateSecondWay関数に渡されて、通路の作成を続ける
			dir = WayDir::RIGHT;
			int pointX = divideRoom[roomId][2];
			game_manager->SetRoomWayPoint(tnl::Vector3(pointX + 1, point, 0), divideRoom[roomId][4]);
			//通路の開始地点をtnl::Vector3型で返します。
			return tnl::Vector3(pointX, point, 0);
		}
	}
	//*********横なら上か下*********HORIZONTAL
	else {
		//分割線のy座標と部屋のy座標を比べる
		//分割線より部屋の上が大きければ分割線は上にある
		if (divideLine[roomId][1] < divideRoom[roomId][1]) {
			int point = 0;
			int debug = 0;
			while (1) {
				//部屋の上端のどっかのy座標
				point = game_manager->GetRandValue(divideRoom[roomId][0], divideRoom[roomId][2]);
				//GetChip関数を呼び出して、通路の位置に隣接するマップチップが通路でないことを確認する
				if ((GetChip(point, divideRoom[roomId][1] - 1)) == 0 && (GetChip(point - 1, divideRoom[roomId][1] - 1)) == 0 && (GetChip(point + 1, divideRoom[roomId][1] - 1)) == 0) break;
				else {
					debug++;
					tnl::DebugTrace("\n適正座標ではない%d回目(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
					if (debug > 50)break;
				}
			}
			dir = WayDir::UP;
			//通路の方向を分割線と直交するように設定する
			//分割線が縦なら、通路は左か右に伸びる。分割線が横なら、通路は上か下に伸びる
			//SetRoomWayPoint関数で、通路の開始地点を保存する
			//これは、部屋に到達するためのウェイポイントとして使われる
			//これは、CreateSecondWay関数に渡されて、通路の作成を続ける
			int pointY = divideRoom[roomId][1];
			game_manager->SetRoomWayPoint(tnl::Vector3(point, pointY - 1, 0), divideRoom[roomId][4]);
			//通路の開始地点をtnl::Vector3型で返します。
			return tnl::Vector3(point, pointY, 0);
		}
		else {
			int point = 0;
			int debug = 0;
			while (1) {
				//部屋の下端のどっかのy座標
				point = game_manager->GetRandValue(divideRoom[roomId][0], divideRoom[roomId][2]);
				//GetChip関数を呼び出して、通路の位置に隣接するマップチップが通路でないことを確認する
				if ((GetChip(point, divideRoom[roomId][3] + 1)) == 0 && (GetChip(point - 1, divideRoom[roomId][3] + 1)) == 0 && (GetChip(point + 1, divideRoom[roomId][3] + 1)) == 0) break;
				else {
					debug++;
					tnl::DebugTrace("\n適正座標ではない%d回目(dir:%d,cell:(%d,%d))\n", debug, divideLine[roomId][4], divideRoom[roomId][0] - 1, point);
					if (debug > 50)break;
				}
			}
			dir = WayDir::DOWN;
			//通路の方向を分割線と直交するように設定する
			//分割線が縦なら、通路は左か右に伸びる。分割線が横なら、通路は上か下に伸びる
			//SetRoomWayPoint関数で、通路の開始地点を保存する
			//これは、部屋に到達するためのウェイポイントとして使われる
			//これは、CreateSecondWay関数に渡されて、通路の作成を続ける
			int pointY = divideRoom[roomId][3];
			game_manager->SetRoomWayPoint(tnl::Vector3(point, pointY + 1, 0), divideRoom[roomId][4]);
			//通路の開始地点をtnl::Vector3型で返します。
			return tnl::Vector3(point, pointY, 0);
		}

		//*************************************************
		// 
		//				最初の部屋の場合
		// 
		//************************************************* 
	}
	

	//return tnl::Vector3(-1, -1, -1);
}

//------------------------------------------------------------------------------------------------------------
//マップに追加の通路を作成する関数
bool MapManager::CreateSecondWay(int x, int y, WayDir dir, int roomId)
{
	if (x - 1 < 0 || y - 1 < 0)return true;
	if (x + 1 > game_manager->MAPWIDTH - 1 || y + 1 > game_manager->MAPHEIGHT - 1)return true;
	int chip = 0;
	//次に移動するチップの位置
	int setX = 0;
	int setY = 0;

	//上の場合はxは現在のxと変わらないが、y座標は、上に上がるため-1した値
	if (dir == WayDir::UP) {
		setX = x;
		setY = y - 1;
		//計算後の位置のチップを通路に変更
		ResetChip(setX, setY, MapChip::MapType::PASSWAY/* EXPASSWAY*/);
		//chip = GetChip(x, y - 1);
	}
	//右の場合は、x座標が右に1ずれる、y座標は現在のまま
	else if (dir == WayDir::RIGHT) {
		setX = x + 1;
		setY = y;
		//計算後の位置のチップを通路に変更
		ResetChip(setX, setY, MapChip::MapType::PASSWAY/*EXPASSWAY*/);
		//chip = GetChip(x + 1, y);
	}
	//下の場合はx座標は変化しないが、y座標は+1した位置に変化
	else if (dir == WayDir::DOWN) {
		setX = x;
		setY = y + 1;
		//計算後の位置のチップを通路に変更
		ResetChip(setX, setY, MapChip::MapType::PASSWAY /*EXPASSWAY*/);
		//chip = GetChip(x, (y + 1));
	}
	//左の場合は、x座標が-1した位置になり、y座標は変化しない
	else if (dir == WayDir::LEFT) {
		setX = x - 1;
		setY = y;
		//計算後の位置のチップを通路に変更
		ResetChip(setX, setY, MapChip::MapType::PASSWAY/* EXPASSWAY*/);
		//chip = GetChip((x - 1), y);
	}
	//もし通路の横のどちらかか、進行方向に通路があれば
	if (CheckChip(setX, setY, dir))
	{
		//関数を終了
		return true;
	}

	//
	//部屋番号に応じて、隣の部屋の辺と通路の座標が一致するかどうかをチェックする
	//もし一致する場合は、通路の方向を直角に曲げて、CreateSecondWay関数を再帰的に呼び出す
	//もし今のx,y座標と隣の部屋の辺のどこかのx,またはy座標が同じなら直角に曲げる
	//
 
	//最初の部屋なら
	if (roomId == 0) {
		//上か下なら
		if (dir == WayDir::UP || dir == WayDir::DOWN) {
			//部屋の範囲内にいるか確認
			if (divideRoom[roomId + 2][1]<setY && divideRoom[roomId + 2][3]>setY) {
				//部屋が左にあるなら(変化後のx座標より小さい場合)
				if (divideRoom[roomId + 2][2] < setX) {
					return CreateSecondWay(setX, setY, WayDir::DOWN, roomId);
				}
				else {
					return CreateSecondWay(setX, setY, WayDir::RIGHT, roomId);
				}
			}
		}
		//右か左なら
		else if (dir == WayDir::RIGHT || dir == WayDir::LEFT) {
			if (divideRoom[roomId + 2][0]<setX && divideRoom[roomId + 2][2]>setX) {
				//部屋が上にある
				if (divideRoom[roomId + 2][3] < setY) {
					return CreateSecondWay(setX, setY, WayDir::UP, roomId);
				}
				else {
					return CreateSecondWay(setX, setY, WayDir::DOWN, roomId);
				}
			}
		}
	}
	//最後のへやなら
	else if (roomId == divideRoom.back()[4]) {
		if (dir == WayDir::UP || dir == WayDir::DOWN) {
			//上
			if (divideRoom[roomId - 2][1]<setY && divideRoom[roomId - 2][3]>setY) {
				//部屋が左にある
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
				//部屋が上にある
				if (divideRoom[roomId - 2][3] < setY) {
					return CreateSecondWay(setX, setY, WayDir::UP, roomId);
				}
				else {
					return CreateSecondWay(setX, setY, WayDir::DOWN, roomId);
				}
			}
		}
	}
	//一致しない場合は、通路の方向をそのままにして、CreateSecondWay関数を再帰的に呼び出す
	return CreateSecondWay(setX, setY, dir, roomId);
}

//------------------------------------------------------------------------------------------------------------
//マップに追加の通路を作成するときに、通路の位置に他の通路や部屋があるかどうかをチェックする関数
bool MapManager::CheckChip(int x, int y, WayDir nextDir)
{
	//座標がマップの範囲外になる場合は、trueを返して関数を終了
	if (x + 1 > game_manager->MAPWIDTH - 1 || y + 1 > game_manager->MAPHEIGHT - 1 || x <= 1 || y <= 1)return true;

	//通路の位置に他の通路や部屋があるかどうかのフラグ
	bool isThere = false;
	if (nextDir == WayDir::UP) {
		//隣接する位置につうろがあればフラグをtureに変化させる
		if (GetChip(x - 1, y) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x + 1, y) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x, y - 1) == MapChip::MapType::PASSWAY)isThere = true;

		int num = 0;
		//部屋の有無をチェック
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
		//隣接する位置につうろがあればフラグをtureに変化させる
		if (GetChip(x, y - 1) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x, y + 1) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x + 1, y) == MapChip::MapType::PASSWAY)isThere = true;

		int num = 0;
		//部屋の有無をチェック
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
		//隣接する位置につうろがあればフラグをtureに変化させる
		if (GetChip(x - 1, y) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x + 1, y) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x, y + 1) == MapChip::MapType::PASSWAY)isThere = true;

		int num = 0;
		//部屋の有無をチェック
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
		//隣接する位置につうろがあればフラグをtureに変化させる
		if (GetChip(x, y - 1) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x, y + 1) == MapChip::MapType::PASSWAY)isThere = true;
		else if (GetChip(x - 1, y) == MapChip::MapType::PASSWAY)isThere = true;

		int num = 0;
		//部屋の有無をチェック
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
//部屋と部屋をつなぐ通路の作成に際して、通路の位置に他の通路がないかどうかをチェックする関数
bool MapManager::CheckPassWay(int roomPos_set, int roomPos_moveStart, int roomPos_moveGoal, WayDir dir)
{
	//通路の位置にほかの通路がないかどうかのフラグ
	bool check = true;
	for (int i = roomPos_moveStart; i < roomPos_moveGoal; ++i) {
		//通路があればfor文を抜ける
		if (dir == WayDir::UP) {
			//左から右へ調査
			if (roomPos_set - 1 < 0)break;
			if (GetChip(i, roomPos_set - 1) == MapChip::MapType::PASSWAY) {
				check = false;
				break;
			}
		}
		else if (dir == WayDir::RIGHT) {
			//上から下へ調査
			if (roomPos_set + 1 > width)break;
			if (GetChip(roomPos_set + 1, i) == MapChip::MapType::PASSWAY) {
				check = false;
				break;
			}
		}
		else if (dir == WayDir::DOWN) {
			//左から右へ調査
			if (roomPos_set + 1 > height)break;
			if (GetChip(i, roomPos_set + 1) == MapChip::MapType::PASSWAY) {
				check = false;
				break;
			}
		}
		else if (dir == WayDir::LEFT) {
			//上から下へ調査
			if (roomPos_set - 1 < 0)break;
			if (GetChip(roomPos_set - 1, i) == MapChip::MapType::PASSWAY) {
				check = false;
				break;
			}
		}
	}
	//for文を抜けたら、checkの値を返します。checkがtrueなら、通路の位置に他の通路がないことを表す
	//checkがfalseなら、通路の位置に他の通路があることを表す
	return check;
}

//------------------------------------------------------------------------------------------------------------
std::string MapManager::GetColorName(int code)
{
	if (code == color_red)return "赤";
	else if (code == color_green)return "緑";
	else if (code == color_blue)return "青";
	else if (code == color_yellow)return "黄";
	else if (code == color_purple)return "紫";

	else return "エラー";
}

//------------------------------------------------------------------------------------------------------------
//セルから見て周囲に通路があるかどうか確かめる関数
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
	//if文で、指定された座標がマップの範囲内にあるかどうかを確認する、もし範囲外なら、処理をスキップする
	//groundという二次元配列から、指定された座標の周囲のマップチップの値を取得して、対応する変数に代入
	//すべての変数の値を合計して、1以上ならtrueを返します。これは、指定された座標の周囲に通路や部屋があることを表します。
	//そうでなければ、falseを返します。これは、指定された座標の周囲に通路や部屋がないことを表します。
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
//セルから周囲の通路を確認し、描画すべき画像が何番か取得する関数 
//部屋と部屋をつなぐ通路の作成に際して、通路の位置に他の通路がないかどうかをチェックする関数
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
	//8近傍のセルが通路ならtrue,そうでなければfalseを代入
	//CheckThisCell関数を呼び出して、指定された座標の周囲のマップチップが通路かどうかを判定して、対応する変数に代入します。
	//CheckThisCell関数は、引数で渡された座標のマップチップの値がPASSWAY,STAIRS,SHOPならtrueを、
	//														     そうでなければfalseを返す関数
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

	//すべての変数の値をもとに、通路の位置が部屋のどの部分にあたるかを判定して、対応する定数を返す
	if (left && leftbotton && leftup && right && rightup && rightbotton)return MapChip::GraphicType::ROOMWALL;//8 ただのブロック

	else if (up && rightbotton)return MapChip::GraphicType::ROOMWALL;
	else if (up && left)return MapChip::GraphicType::ROOMWALL;
	else if (left && right)return MapChip::GraphicType::ROOMWALL;
	else if (up && rightup && right)return MapChip::GraphicType::ROOMWALL;

	//下,右下,左下がtrue->通路なら 部屋の上側
	else if (botton)return MapChip::GraphicType::ROOMTOP;//0

	else if (left)return MapChip::GraphicType::ROOMRIGHT;//1
	else if (up)return MapChip::GraphicType::ROOMBOTTOM;//2
	else if (right)return MapChip::GraphicType::ROOMLEFT;//3

	else if (rightbotton && !up && !botton && !right && !left && !rightup && !leftup && !leftbotton)return MapChip::GraphicType::ROOMLEFTTOP;//4
	else if (leftbotton && !up && !botton && !right && !left && !rightup && !leftup && !rightbotton)return MapChip::GraphicType::ROOMRIGHTTOP;//5
	else if (rightup && !up && !botton && !right && !left && !leftbotton && !leftup && !rightbotton)return MapChip::GraphicType::ROOMLEFTBOTTOM;//6
	else if (leftup && !up && !botton && !right && !left && !rightup && !rightbotton && !leftbotton)return MapChip::GraphicType::ROOMRIGHTBOTTOM;//7


	//もし通路の位置が部屋のどの部分にもあたらない場合は、OUTOFRANGEを返す
	else return OUTOFRANGE;
}

//------------------------------------------------------------------------------------------------------------
//引数で渡された座標のマップチップの値がPASSWAY,STAIRS,SHOPならtrueを、そうでなければfalseを返す関数
bool MapManager::CheckThisCell(int x, int y)
{
	if (ground[x][y] == MapChip::MapType::PASSWAY || ground[x][y] == MapChip::MapType::STAIRS || ground[x][y] == MapChip::MapType::SHOP)return true;
	else return false;
}

//------------------------------------------------------------------------------------------------------------
//今表示されている全ての部屋の上下左右座標をする描画するDebug関数
void MapManager::DrawAllRoomPos(std::vector<std::vector<int>>RoomList)
{
	int drawPosX = 10;
	int drawPosY = 10;

	//左 上 右 下 id
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
