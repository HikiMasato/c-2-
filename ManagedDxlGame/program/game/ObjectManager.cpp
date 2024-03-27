//-------------------------------------------------------
//既存のもの
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
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
#include "EnemyManager.h"
#include "ItemManager.h"
#include "GameManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "SkillManager.h"
#include "EffectManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "Character.h"
#include "MyCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "MapChip.h"
//-------------------------------------------------------
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Skill.h"
#include "Debug.h"




//------------------------------------------------------------------------------------------------------------
//コンストラクタ
ObjectManager::ObjectManager(std::vector<std::vector<std::string>>& re_stetus, std::vector<std::vector<std::string>>& effect_csv, std::vector<std::vector<std::string>>re_item, GameManager* game_manager_)
{
	game_manager = game_manager_;
	item_data = re_item;
	chara_info.resize(re_stetus.size() + 2);

	//csvファイルの読み込み(Start_mapのもの)
	start_map_csv = tnl::LoadCsv<int>("csv/start_map.csv");
	start_map_second_csv = tnl::LoadCsv<int>("csv/start_map_second.csv");

	//start_map画像の分割
	LoadDivGraph("using_graphics/start_mapchip/start_map_chip.png", ALL_START_MAP_CHIP, map_graph_value_x, map_graph_value_y, MAPCHIPSIZE, MAPCHIPSIZE, start_map_handle);

	factory			= std::make_shared<Factory>(game_manager);
	stetus			= std::make_shared<Stetus>(re_stetus, effect_csv);


	//キャラのスキルデータをSkillManagerに渡す
	GiveSkillEffectData();
}

//------------------------------------------------------------------------------------------------------------
//デストラクタ
ObjectManager::~ObjectManager()
{
	//このクラスが解放されるときはすべてのオブジェクトも同時に解放される
	item_data.clear();
	chara_info.clear();
	AllChipListClear();
	AllListClear();
	delete game_manager;

}

//------------------------------------------------------------------------------------------------------------
//毎フレーム実行されます
void ObjectManager::Update(const float delta_time)
{
	
	//alive_enemyに何もなければ処理しない
	if (!alive_enemy.empty()) {
		//生存中の敵のUpdate
		for (auto enemy : alive_enemy) {
			enemy->Update(delta_time);
		}
	}

	//プレイヤー固有のUpdateを処理
	if (factory->GetPlayer() != nullptr) {
		factory->GetPlayer()->Update(delta_time);
	}

	
	//条件に合致するオブジェクトを削除する
	EraseObjectList();
}

//------------------------------------------------------------------------------------------------------------
//描画
void ObjectManager::Draw()
{
	//alive_itemに何もなければ処理しない
	if (!alive_item.empty()) {
		//マップに生存中のアイテムの描画
		for (auto& item : alive_item) {
			item->Draw(factory->camera);
		}
	}
	//object_listに何もなければ処理しない
	if (!object_list.empty()) {
		//object_listの描画
		for (auto& object : object_list) {
			object->Draw(factory->camera);
		}
	}
	//charactor_listに何もなければ処理しない
	if (!charactor_list.empty()) {
		//キャラクターの描画
		for (auto& chara : charactor_list) {
			chara->Draw(factory->camera);
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//Factoryクラスに生成を受注する関数(生成はFactoryが行うためここではステータスを渡すのと受注のみ行う)
void ObjectManager::GenerateOrders(Factory::PlayerSpawn now_spawn_, bool is_dungeon_level_up)
{

	//キャラステータスを取得(プレイヤーの整数型のステータスをすべて取得(Hp,Mp,Attack,Defence))
	std::vector<std::vector<int>>palam = std::get<std::vector<std::vector<int>>>(stetus->GetCharaStetus(Stetus::CharaStetus::PLAYERINT, 0));
	//キャラネームを取得
	//画像パスを取得
	std::vector<std::vector<std::string>> palam_string = std::get<std::vector<std::vector<std::string>>>(stetus->GetCharaStetus(Stetus::CharaStetus::PLAYERSTRING, 1));
	
	//二つの配列を一つにまとめる
	for (size_t i = 0; i < palam_string.size(); i++) {

		chara_info[i] = std::make_pair(palam[i], palam_string[i]);

	}


	//スタートマップの時
	//初期マップでは敵とアイテムの生成は必要ないので現状では生成しない(後々NPCなど置けるとなおいいかも(優先度 : 低))
	if (now_spawn_ == Factory::PlayerSpawn::STARTMAP) {
		//Playerの生成受注
		factory->GeneratePlayer(now_spawn_, start_pl_pos,
			chara_info[0].first[0], chara_info[0].first[1], chara_info[0].first[2], chara_info[0].first[3], chara_info[0].second[4].c_str(), chara_info[0].second[5].c_str(), chara_info[0].first[4], chara_info[0].first[5], charactor_list);

		game_manager->CameraReset();


		//プレイヤー生成時デバッグ
		tnl::DebugTrace("\n=====================StartMap用Player生成============================\n");
	
	}
	//ダンジョンシーンの時
	else if (now_spawn_ == Factory::PlayerSpawn::DUNGEON) {
		
		//ダンジョンの階層が上がるときはプレイヤーの生成は必要ない
		if (!is_dungeon_level_up) {
			//Playerの生成受注
			factory->GeneratePlayer(now_spawn_,
				game_manager->SetStartPosition(GameManager::SetStartPositionType::PLAYER),
				chara_info[0].first[0], chara_info[0].first[1], chara_info[0].first[2], chara_info[0].first[3], chara_info[0].second[4].c_str(), chara_info[0].second[5].c_str(), chara_info[0].first[4], chara_info[0].first[5], charactor_list);

			//プレイヤーのスキルをセット
			SetPlayerSkill();

			//プレイヤー生成時デバッグ
			tnl::DebugTrace("\n====================DungeonMap用Player生成===========================\n");

		}

		
		//指定回数実行
		for (int i = 0; i < GENERATE_ITEM; i++) {
			std::string item_name = GetRandKindItemValue();
			//アイテムランダム生成受注
			for (int k = 1; k < item_data.size(); k++) {
				//ランダムに決定した名前の敵を生成
				if (item_name == item_data[k][1].c_str()) {
					//Itemの生成受注
					factory->GenerateItem(
						game_manager->SetStartPosition(GameManager::SetStartPositionType::ITEM),
						std::stoi(item_data[k][0].c_str()),
						item_data[k][1].c_str(),
						item_data[k][2].c_str(),
						item_data[k][3].c_str(),
						std::stoi(item_data[k][4]),
						object_list,
						alive_item
					);

					//アイテム生成時デバッグ
					tnl::DebugTrace("\n=================アイテム生成%d回目====================\n", i);

					break;
				}

			}
		}

		//指定回数実行
		for (int i = 0; i < GENERATE_ENEMY; i++) {
			std::string enemy_name = GetRandKindEnemyValue();
			//敵ランダム生成受注
			for (int k = 0; k < chara_info.size() + 2; k++) {
				//ランダムに決定した名前の敵を生成
				if (enemy_name == chara_info[k].second[4].c_str()) {
					//Enemyの生成受注
					factory->GenerateEnemy(
						game_manager->SetStartPosition(GameManager::SetStartPositionType::ENEMY),	//postion
						chara_info[k].first[0],														//Hp
						chara_info[k].first[1],														//Mp
						chara_info[k].first[2],														//Atk
						chara_info[k].first[3],														//Def
						chara_info[k].second[4].c_str(),											//Name
						chara_info[k].second[5].c_str(),											//Gh_handle
						chara_info[k].first[4],														//Exp
						chara_info[k].first[5],														//Level
						charactor_list,
						alive_enemy
					);

					//敵スキルをセット
					SetEnemySkill(enemy_name);

					//敵生成時デバッグ
					tnl::DebugTrace("\n================敵生成%d回目====================\n", i);

					break;
				}
			}
		}

		game_manager->CameraReset();

	}
	

	//受注されたものをすべて生成したことを通知デバッグ
	tnl::DebugTrace("\n==================すべて生成完了======================\n");

}

//------------------------------------------------------------------------------------------------------------
//マップチップの生成を受注する
void ObjectManager::GenerateOrdersToMapChip()
{
	//groundに格納されているチップ情報をもとにマップチップクラスを生成

	for (int i = 0; i < game_manager->GetMapManager()->GetGround().size(); ++i) {
		for (int k = 0; k < game_manager->GetMapManager()->GetGround()[i].size(); ++k) {

			//マップチップの元座標(ゲーム内座標)
			old_chip_pos = { k * game_manager->GetNowGraphSize(), i * game_manager->GetNowGraphSize(), 0 };

			//壁ならオートタイルで選ぶ
			if (game_manager->GetMapManager()->GetGround()[i][k] == MapChip::MapType::WALL) {
				if (!game_manager->GetMapManager()->CheckAround(i, k))continue;
				int iti = game_manager->GetMapManager()->CheckAroundWay(i, k);
				factory->GenerateMapChip(old_chip_pos, game_manager->GetMapManager()->GetAutoTileChip((MapManager::GraphicType)iti), all_chip_list, wall_chip, true);

			}

			//道
			else if (game_manager->GetMapManager()->GetGround()[i][k] == MapChip::MapType::PASSWAY) {
				factory->GenerateMapChip(old_chip_pos, game_manager->GetMapManager()->GetFloor(), all_chip_list, wall_chip);
			}

			//階段
			else if (game_manager->GetMapManager()->GetGround()[i][k] == MapChip::MapType::STAIRS) {
				factory->GenerateMapChip(old_chip_pos, game_manager->GetMapManager()->GetStair(), all_chip_list, wall_chip);
			}

			//店
			else if (game_manager->GetMapManager()->GetGround()[i][k] == MapChip::MapType::SHOP) {
				factory->GenerateMapChip(old_chip_pos, game_manager->GetMapManager()->GetShop(), all_chip_list, wall_chip);
			}

			else {
				tnl::DebugTrace("\n============ダンジョンマップチップ生成エラー(%d,%d)===============\n", i, k);
			}
			

		}
	}
}

//------------------------------------------------------------------------------------------------------------
//スターとマップ用のマップの生成を受注する
void ObjectManager::GenerateOrdersToStartMapChip()
{
	//2次元配列でcsvに沿ってデータを格納
	for (int i = 0; i < start_map_csv.size(); i++) {
		for (int k = 0; k < start_map_csv[i].size(); k++) {
			//csvのデータが-1ならcontinue
			if (start_map_csv[i][k] == -1)continue;
			tnl::Vector3 chip_pos;
			chip_pos.x = { former_map_pos.x + k * MAPCHIPSIZE };
			chip_pos.y = { former_map_pos.y + i * MAPCHIPSIZE };
			factory->GenerateMapChip(chip_pos, start_map_handle[start_map_csv[i][k]], start_map_chip_list, wall_chip);
		}
	}
	//2次元配列でcsvに沿ってデータを格納
	for (int i = 0; i < start_map_second_csv.size(); i++) {
		for (int k = 0; k < start_map_second_csv[i].size(); k++) {
			//csvのデータが-1ならcontinue
			if (start_map_second_csv[i][k] == -1)continue;
			tnl::Vector3 chip_pos;
			chip_pos.x = { former_map_pos.x + k * MAPCHIPSIZE };
			chip_pos.y = { former_map_pos.y + i * MAPCHIPSIZE };
			factory->GenerateMapChip(chip_pos, start_map_handle[start_map_second_csv[i][k]], start_map_second_chip_list, wall_chip);
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//事前にすべてのアイテムの生成を受注しておく
void ObjectManager::GenerateAllItem()
{
	//初期化
	all_item.resize(0);

	//csvに書き込んであるアイテムの回数分回す
	for (int i = 1; i < item_data.size(); i++) {
		factory->AllItemCreate(
			game_manager->SetStartPosition(GameManager::SetStartPositionType::ITEM),	//postion
			std::stoi(item_data[i][0].c_str()),											//efficacy_stetus
			item_data[i][1].c_str(),													//item_name
			item_data[i][2].c_str(),													//item_handle
			item_data[i][3].c_str(),													//item_descrption
			std::stoi(item_data[i][4]),
			all_item																	//格納配列
		);
	}
	//インスタンス化
	item_manager = std::make_shared<ItemManager>(all_item);

	tnl::DebugTrace("\n==============GenerateAllItem完了=================\n");
}

//------------------------------------------------------------------------------------------------------------
//ショップに並べるアイテムを決める関数
std::string ObjectManager::GenerateShopItem()
{
	std::string item_name = GetRandKindItemValue();
	return item_name;
}

//------------------------------------------------------------------------------------------------------------
void ObjectManager::GenerateDropItem(tnl::Vector3 pos)
{
	std::string item_name = GetRandKindItemValue();
	//アイテムランダム生成受注
	for (int k = 1; k < item_data.size(); k++) {
		//ランダムに決定した名前の敵を生成
		if (item_name == item_data[k][1].c_str()) {
			//Itemの生成受注
			factory->GenerateItem(
				pos,
				std::stoi(item_data[k][0].c_str()),
				item_data[k][1].c_str(),
				item_data[k][2].c_str(),
				item_data[k][3].c_str(),
				std::stoi(item_data[k][4]),
				object_list,
				alive_item
			);

			//アイテム生成時デバッグ
			tnl::DebugTrace("\n=================アイテム生成%d回目====================\n");

			break;
		}

	}
}

//------------------------------------------------------------------------------------------------------------
//条件に合致するオブジェクトを削除する
void ObjectManager::EraseObjectList()
{

	//erase-removeイディオム
	//そのイテレータをeraseメソッドに渡して、移動させた要素を一括で削除
	//キャプチャリスト[&]は、ラムダ式の外側にある全ての変数を参照でキャプチャすることを意味する
	//そのため、ラムダ式内で外部の変数を使用することができるようになる
	
	{//オブジェクトリストから削除(aliveがtrueの場合に削除実行)	

		//空ではなければ
		if(!object_list.empty()) {

			//remove_ifメソッドで条件に合う要素をコンテナの末尾に移動させ、
			object_list.erase(remove_if(object_list.begin(), object_list.end(), [&](std::shared_ptr<Object>) {
				//条件
				return factory->GetItem()->GetIsAlive();
			}), object_list.end());
		
		}
	
	}
	
}

//------------------------------------------------------------------------------------------------------------
//条件に合致するキャラをリストから削除する
void ObjectManager::EraseCharaList()
{
#if 0
	{//キャラクターリストから削除(aliveがfalseの場合に削除)

		//空ではなければ
		if (!charactor_list.empty()) {

			//remove_ifメソッドで条件に合う要素をコンテナの末尾に移動させ、
			charactor_list.erase(remove_if(charactor_list.begin(), charactor_list.end(), [&](std::shared_ptr<Enemy>) {
				//条件
				return !factory->GetEnemy()->GetIsAlive();
				}), charactor_list.end());

		}

}


	{//生存している敵リストから削除(aliveがfalseの場合に削除)

		//空ではなければ
		if (!alive_enemy.empty()) {

			//remove_ifメソッドで条件に合う要素をコンテナの末尾に移動させ、
			alive_enemy.erase(remove_if(alive_enemy.begin(), alive_enemy.end(), [&](std::shared_ptr<Enemy>) {
				//条件
				!factory->GetEnemy()->GetIsAlive();

				}), alive_enemy.end());

		}

	}
#endif // 0


	////ここでnullチェック入れるのはありかも
	////敵生存リストと、キャラクタリストからaliveflagが下りているのキャラを消去する
	auto itr1 = alive_enemy.begin();
	auto itr2 = charactor_list.begin();

	for (int i = 0; i < alive_enemy.size(); ++i) {
		if (!(*itr1)->GetIsAlive()) {
			itr1 = alive_enemy.erase(itr1);
		}
		else {
			itr1++;
		}
	}
	for (int i = 0; i < charactor_list.size(); ++i) {
		if (!(*itr2)->GetIsAlive()) {
			itr2 = charactor_list.erase(itr2);
		}
		else {
			itr2++;
		}
	}
}

//------------------------------------------------------------------------------------------------------------
//すべてのリストをクリアする関数
void ObjectManager::AllListClear(bool player_deletion_flag)
{	

	//プレイヤーを生かす場合
	if (!player_deletion_flag) {
		
		//オブジェクトリストが空ではなければ
		if (!charactor_list.empty()) {
			std::list<std::shared_ptr<Character>>::iterator entity;

			for (entity = charactor_list.begin(); entity != charactor_list.end();) {
				//プレイヤー以外のキャラをobject_listから消去
				if ((*entity)->GetObjectType() != Object::ObjectType::PLAYER) {
					entity = charactor_list.erase(entity);
					continue;
				}
				entity++;
			}
		}
	}
	//プレイヤーを生かさない場合
	else {
		//キャラ格納リスト
		charactor_list.clear();
	}

	//オブジェクト格納リスト
	object_list.clear();
	//生存中格納リスト
	alive_enemy.clear();
	//生存中アイテム格納リスト
	alive_item.clear();
	
}

//------------------------------------------------------------------------------------------------------------
//マップチップリストと壁リストを削除する
void ObjectManager::AllChipListClear()
{
	all_chip_list.clear();
	wall_chip.clear();
}

//------------------------------------------------------------------------------------------------------------
//キャラクターのステータスを強化する
void ObjectManager::RainForceCharaStetus()
{
	//階層移動時に全キャラ一律強化か、敵のみ強化悩みどころ

	//キャラリストに格納されているキャラのステータスを強化する
	for (auto chara : charactor_list) {
		//敵の時のみ
		if (chara->GetObjectType() == Object::ObjectType::ENEMY) {
			chara->ReinforceStetus();
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//プレイヤーのスポーン場所を変更する関数
void ObjectManager::SetPlayerSpawnType(Factory::PlayerSpawn next_spawn)
{
	factory->SetPlayerSpawn(next_spawn);
}

//------------------------------------------------------------------------------------------------------------
//敵を生成するときにどの敵を生成するかをランダムに決める関数
std::string ObjectManager::GetRandKindEnemyValue()
{
	//ランダムに敵一覧の列挙型の中からランダムに値を返す
	int rand_name = game_manager->GetRandValue((int)EnemyManager::EnemyName::SKELETON, (int)EnemyManager::EnemyName::KING);
	//敵キャラ番号一覧
	//0 : Skeleton
	//1 : PoisonSkeleton
	//2 : BrackSkeleton
	//3 : Magician
	//4 : King
	std::string name[] = { "Skeleton" ,"PoisonSkeleton" ,"BrackSkeleton" ,"Magician" ,"King" };

	//戻り値は敵キャラ番号
	return name[rand_name];

}

//------------------------------------------------------------------------------------------------------------
std::string ObjectManager::GetRandKindItemValue()
{
	//リンゴ,薬草,防御力,ダイアモンド,Gold,ドロップアイテム,ポーション

	int rand_item = game_manager->GetRandValue((int)ItemManager::ItemName::APPLE, (int)ItemManager::ItemName::ATTACK);
	//アイテムの名前配列　
	std::string name[] = { "リンゴ","薬草","防御力","ダイアモンド","Gold","ドロップアイテム","ポーション","攻撃力"};

	return name[rand_item];
}

//------------------------------------------------------------------------------------------------------------
//キャラクターが使うスキルデータをSkillManagerに渡す
void ObjectManager::GiveSkillEffectData()
{
	//キャラステータスを取得(プレイヤーの整数型のステータスをすべて取得(Hp,Mp,Attack,Defence))
	std::vector<std::vector<int>>palam = std::get<std::vector<std::vector<int>>>(stetus->GetEffect(Stetus::EffectDataType::INT));
	//キャラネームを取得
	//画像パスを取得
	std::vector<std::vector<std::string>> palam_string = std::get<std::vector<std::vector<std::string>>>(stetus->GetEffect(Stetus::EffectDataType::STRING));

	skill_manager = std::make_shared<SkillManager>(palam, palam_string, game_manager);
}

//------------------------------------------------------------------------------------------------------------
//キャラクターのスキルを設定する関数
void ObjectManager::SetCharaSkill(std::vector<Skill*>& skill_list, SkillManager::UseEffectType type, SkillManager::EffectName name)
{
	skill_list.emplace_back(skill_manager->GetSkill(type, name));
}

//------------------------------------------------------------------------------------------------------------
//プレイヤーのスキルをセットする
void ObjectManager::SetPlayerSkill()
{
	//通常攻撃
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::NOMALATTCK);
	//水攻撃
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::WATER);
	//風攻撃
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::WIND);
	//岩攻撃
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::ROCK);
	//火攻撃
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::FIRE);
	//雷攻撃
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::THUNDER);
}

//------------------------------------------------------------------------------------------------------------
//敵スキルをセットする
void ObjectManager::SetEnemySkill(std::string name)
{
	//スキルセット
	if (name == "Magician") {
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::ENEMY, SkillManager::EffectName::BRACKMAGIC);
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::BOTHCHARACTOR, SkillManager::EffectName::RECOVERY);
	}
	else if (name == "Skeleton") {
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::ENEMY, SkillManager::EffectName::BRACKMAGIC);
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::BOTHCHARACTOR, SkillManager::EffectName::RECOVERY);
	}
	else if (name == "PoisonSkeleton") {
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::ENEMY, SkillManager::EffectName::BRACKMAGIC);
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::BOTHCHARACTOR, SkillManager::EffectName::RECOVERY);
	}
	else if (name == "BrackSkeleton") {
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::ENEMY, SkillManager::EffectName::BRACKMAGIC);
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::BOTHCHARACTOR, SkillManager::EffectName::RECOVERY);
	}
	else if (name == "King") {
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::ENEMY, SkillManager::EffectName::BRACKMAGIC);
		SetCharaSkill(factory->GetEnemy()->GetSkillList(), SkillManager::UseEffectType::BOTHCHARACTOR, SkillManager::EffectName::RECOVERY);
	}

}



