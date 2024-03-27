//-------------------------------------------------------
//�����̂���
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
//�R���X�g���N�^
ObjectManager::ObjectManager(std::vector<std::vector<std::string>>& re_stetus, std::vector<std::vector<std::string>>& effect_csv, std::vector<std::vector<std::string>>re_item, GameManager* game_manager_)
{
	game_manager = game_manager_;
	item_data = re_item;
	chara_info.resize(re_stetus.size() + 2);

	//csv�t�@�C���̓ǂݍ���(Start_map�̂���)
	start_map_csv = tnl::LoadCsv<int>("csv/start_map.csv");
	start_map_second_csv = tnl::LoadCsv<int>("csv/start_map_second.csv");

	//start_map�摜�̕���
	LoadDivGraph("using_graphics/start_mapchip/start_map_chip.png", ALL_START_MAP_CHIP, map_graph_value_x, map_graph_value_y, MAPCHIPSIZE, MAPCHIPSIZE, start_map_handle);

	factory			= std::make_shared<Factory>(game_manager);
	stetus			= std::make_shared<Stetus>(re_stetus, effect_csv);


	//�L�����̃X�L���f�[�^��SkillManager�ɓn��
	GiveSkillEffectData();
}

//------------------------------------------------------------------------------------------------------------
//�f�X�g���N�^
ObjectManager::~ObjectManager()
{
	//���̃N���X����������Ƃ��͂��ׂẴI�u�W�F�N�g�������ɉ�������
	item_data.clear();
	chara_info.clear();
	AllChipListClear();
	AllListClear();
	delete game_manager;

}

//------------------------------------------------------------------------------------------------------------
//���t���[�����s����܂�
void ObjectManager::Update(const float delta_time)
{
	
	//alive_enemy�ɉ����Ȃ���Ώ������Ȃ�
	if (!alive_enemy.empty()) {
		//�������̓G��Update
		for (auto enemy : alive_enemy) {
			enemy->Update(delta_time);
		}
	}

	//�v���C���[�ŗL��Update������
	if (factory->GetPlayer() != nullptr) {
		factory->GetPlayer()->Update(delta_time);
	}

	
	//�����ɍ��v����I�u�W�F�N�g���폜����
	EraseObjectList();
}

//------------------------------------------------------------------------------------------------------------
//�`��
void ObjectManager::Draw()
{
	//alive_item�ɉ����Ȃ���Ώ������Ȃ�
	if (!alive_item.empty()) {
		//�}�b�v�ɐ������̃A�C�e���̕`��
		for (auto& item : alive_item) {
			item->Draw(factory->camera);
		}
	}
	//object_list�ɉ����Ȃ���Ώ������Ȃ�
	if (!object_list.empty()) {
		//object_list�̕`��
		for (auto& object : object_list) {
			object->Draw(factory->camera);
		}
	}
	//charactor_list�ɉ����Ȃ���Ώ������Ȃ�
	if (!charactor_list.empty()) {
		//�L�����N�^�[�̕`��
		for (auto& chara : charactor_list) {
			chara->Draw(factory->camera);
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//Factory�N���X�ɐ������󒍂���֐�(������Factory���s�����߂����ł̓X�e�[�^�X��n���̂Ǝ󒍂̂ݍs��)
void ObjectManager::GenerateOrders(Factory::PlayerSpawn now_spawn_, bool is_dungeon_level_up)
{

	//�L�����X�e�[�^�X���擾(�v���C���[�̐����^�̃X�e�[�^�X�����ׂĎ擾(Hp,Mp,Attack,Defence))
	std::vector<std::vector<int>>palam = std::get<std::vector<std::vector<int>>>(stetus->GetCharaStetus(Stetus::CharaStetus::PLAYERINT, 0));
	//�L�����l�[�����擾
	//�摜�p�X���擾
	std::vector<std::vector<std::string>> palam_string = std::get<std::vector<std::vector<std::string>>>(stetus->GetCharaStetus(Stetus::CharaStetus::PLAYERSTRING, 1));
	
	//��̔z�����ɂ܂Ƃ߂�
	for (size_t i = 0; i < palam_string.size(); i++) {

		chara_info[i] = std::make_pair(palam[i], palam_string[i]);

	}


	//�X�^�[�g�}�b�v�̎�
	//�����}�b�v�ł͓G�ƃA�C�e���̐����͕K�v�Ȃ��̂Ō���ł͐������Ȃ�(��XNPC�Ȃǒu����ƂȂ���������(�D��x : ��))
	if (now_spawn_ == Factory::PlayerSpawn::STARTMAP) {
		//Player�̐�����
		factory->GeneratePlayer(now_spawn_, start_pl_pos,
			chara_info[0].first[0], chara_info[0].first[1], chara_info[0].first[2], chara_info[0].first[3], chara_info[0].second[4].c_str(), chara_info[0].second[5].c_str(), chara_info[0].first[4], chara_info[0].first[5], charactor_list);

		game_manager->CameraReset();


		//�v���C���[�������f�o�b�O
		tnl::DebugTrace("\n=====================StartMap�pPlayer����============================\n");
	
	}
	//�_���W�����V�[���̎�
	else if (now_spawn_ == Factory::PlayerSpawn::DUNGEON) {
		
		//�_���W�����̊K�w���オ��Ƃ��̓v���C���[�̐����͕K�v�Ȃ�
		if (!is_dungeon_level_up) {
			//Player�̐�����
			factory->GeneratePlayer(now_spawn_,
				game_manager->SetStartPosition(GameManager::SetStartPositionType::PLAYER),
				chara_info[0].first[0], chara_info[0].first[1], chara_info[0].first[2], chara_info[0].first[3], chara_info[0].second[4].c_str(), chara_info[0].second[5].c_str(), chara_info[0].first[4], chara_info[0].first[5], charactor_list);

			//�v���C���[�̃X�L�����Z�b�g
			SetPlayerSkill();

			//�v���C���[�������f�o�b�O
			tnl::DebugTrace("\n====================DungeonMap�pPlayer����===========================\n");

		}

		
		//�w��񐔎��s
		for (int i = 0; i < GENERATE_ITEM; i++) {
			std::string item_name = GetRandKindItemValue();
			//�A�C�e�������_��������
			for (int k = 1; k < item_data.size(); k++) {
				//�����_���Ɍ��肵�����O�̓G�𐶐�
				if (item_name == item_data[k][1].c_str()) {
					//Item�̐�����
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

					//�A�C�e���������f�o�b�O
					tnl::DebugTrace("\n=================�A�C�e������%d���====================\n", i);

					break;
				}

			}
		}

		//�w��񐔎��s
		for (int i = 0; i < GENERATE_ENEMY; i++) {
			std::string enemy_name = GetRandKindEnemyValue();
			//�G�����_��������
			for (int k = 0; k < chara_info.size() + 2; k++) {
				//�����_���Ɍ��肵�����O�̓G�𐶐�
				if (enemy_name == chara_info[k].second[4].c_str()) {
					//Enemy�̐�����
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

					//�G�X�L�����Z�b�g
					SetEnemySkill(enemy_name);

					//�G�������f�o�b�O
					tnl::DebugTrace("\n================�G����%d���====================\n", i);

					break;
				}
			}
		}

		game_manager->CameraReset();

	}
	

	//�󒍂��ꂽ���̂����ׂĐ����������Ƃ�ʒm�f�o�b�O
	tnl::DebugTrace("\n==================���ׂĐ�������======================\n");

}

//------------------------------------------------------------------------------------------------------------
//�}�b�v�`�b�v�̐������󒍂���
void ObjectManager::GenerateOrdersToMapChip()
{
	//ground�Ɋi�[����Ă���`�b�v�������ƂɃ}�b�v�`�b�v�N���X�𐶐�

	for (int i = 0; i < game_manager->GetMapManager()->GetGround().size(); ++i) {
		for (int k = 0; k < game_manager->GetMapManager()->GetGround()[i].size(); ++k) {

			//�}�b�v�`�b�v�̌����W(�Q�[�������W)
			old_chip_pos = { k * game_manager->GetNowGraphSize(), i * game_manager->GetNowGraphSize(), 0 };

			//�ǂȂ�I�[�g�^�C���őI��
			if (game_manager->GetMapManager()->GetGround()[i][k] == MapChip::MapType::WALL) {
				if (!game_manager->GetMapManager()->CheckAround(i, k))continue;
				int iti = game_manager->GetMapManager()->CheckAroundWay(i, k);
				factory->GenerateMapChip(old_chip_pos, game_manager->GetMapManager()->GetAutoTileChip((MapManager::GraphicType)iti), all_chip_list, wall_chip, true);

			}

			//��
			else if (game_manager->GetMapManager()->GetGround()[i][k] == MapChip::MapType::PASSWAY) {
				factory->GenerateMapChip(old_chip_pos, game_manager->GetMapManager()->GetFloor(), all_chip_list, wall_chip);
			}

			//�K�i
			else if (game_manager->GetMapManager()->GetGround()[i][k] == MapChip::MapType::STAIRS) {
				factory->GenerateMapChip(old_chip_pos, game_manager->GetMapManager()->GetStair(), all_chip_list, wall_chip);
			}

			//�X
			else if (game_manager->GetMapManager()->GetGround()[i][k] == MapChip::MapType::SHOP) {
				factory->GenerateMapChip(old_chip_pos, game_manager->GetMapManager()->GetShop(), all_chip_list, wall_chip);
			}

			else {
				tnl::DebugTrace("\n============�_���W�����}�b�v�`�b�v�����G���[(%d,%d)===============\n", i, k);
			}
			

		}
	}
}

//------------------------------------------------------------------------------------------------------------
//�X�^�[�ƃ}�b�v�p�̃}�b�v�̐������󒍂���
void ObjectManager::GenerateOrdersToStartMapChip()
{
	//2�����z���csv�ɉ����ăf�[�^���i�[
	for (int i = 0; i < start_map_csv.size(); i++) {
		for (int k = 0; k < start_map_csv[i].size(); k++) {
			//csv�̃f�[�^��-1�Ȃ�continue
			if (start_map_csv[i][k] == -1)continue;
			tnl::Vector3 chip_pos;
			chip_pos.x = { former_map_pos.x + k * MAPCHIPSIZE };
			chip_pos.y = { former_map_pos.y + i * MAPCHIPSIZE };
			factory->GenerateMapChip(chip_pos, start_map_handle[start_map_csv[i][k]], start_map_chip_list, wall_chip);
		}
	}
	//2�����z���csv�ɉ����ăf�[�^���i�[
	for (int i = 0; i < start_map_second_csv.size(); i++) {
		for (int k = 0; k < start_map_second_csv[i].size(); k++) {
			//csv�̃f�[�^��-1�Ȃ�continue
			if (start_map_second_csv[i][k] == -1)continue;
			tnl::Vector3 chip_pos;
			chip_pos.x = { former_map_pos.x + k * MAPCHIPSIZE };
			chip_pos.y = { former_map_pos.y + i * MAPCHIPSIZE };
			factory->GenerateMapChip(chip_pos, start_map_handle[start_map_second_csv[i][k]], start_map_second_chip_list, wall_chip);
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//���O�ɂ��ׂẴA�C�e���̐������󒍂��Ă���
void ObjectManager::GenerateAllItem()
{
	//������
	all_item.resize(0);

	//csv�ɏ�������ł���A�C�e���̉񐔕���
	for (int i = 1; i < item_data.size(); i++) {
		factory->AllItemCreate(
			game_manager->SetStartPosition(GameManager::SetStartPositionType::ITEM),	//postion
			std::stoi(item_data[i][0].c_str()),											//efficacy_stetus
			item_data[i][1].c_str(),													//item_name
			item_data[i][2].c_str(),													//item_handle
			item_data[i][3].c_str(),													//item_descrption
			std::stoi(item_data[i][4]),
			all_item																	//�i�[�z��
		);
	}
	//�C���X�^���X��
	item_manager = std::make_shared<ItemManager>(all_item);

	tnl::DebugTrace("\n==============GenerateAllItem����=================\n");
}

//------------------------------------------------------------------------------------------------------------
//�V���b�v�ɕ��ׂ�A�C�e�������߂�֐�
std::string ObjectManager::GenerateShopItem()
{
	std::string item_name = GetRandKindItemValue();
	return item_name;
}

//------------------------------------------------------------------------------------------------------------
void ObjectManager::GenerateDropItem(tnl::Vector3 pos)
{
	std::string item_name = GetRandKindItemValue();
	//�A�C�e�������_��������
	for (int k = 1; k < item_data.size(); k++) {
		//�����_���Ɍ��肵�����O�̓G�𐶐�
		if (item_name == item_data[k][1].c_str()) {
			//Item�̐�����
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

			//�A�C�e���������f�o�b�O
			tnl::DebugTrace("\n=================�A�C�e������%d���====================\n");

			break;
		}

	}
}

//------------------------------------------------------------------------------------------------------------
//�����ɍ��v����I�u�W�F�N�g���폜����
void ObjectManager::EraseObjectList()
{

	//erase-remove�C�f�B�I��
	//���̃C�e���[�^��erase���\�b�h�ɓn���āA�ړ��������v�f���ꊇ�ō폜
	//�L���v�`�����X�g[&]�́A�����_���̊O���ɂ���S�Ă̕ϐ����Q�ƂŃL���v�`�����邱�Ƃ��Ӗ�����
	//���̂��߁A�����_�����ŊO���̕ϐ����g�p���邱�Ƃ��ł���悤�ɂȂ�
	
	{//�I�u�W�F�N�g���X�g����폜(alive��true�̏ꍇ�ɍ폜���s)	

		//��ł͂Ȃ����
		if(!object_list.empty()) {

			//remove_if���\�b�h�ŏ����ɍ����v�f���R���e�i�̖����Ɉړ������A
			object_list.erase(remove_if(object_list.begin(), object_list.end(), [&](std::shared_ptr<Object>) {
				//����
				return factory->GetItem()->GetIsAlive();
			}), object_list.end());
		
		}
	
	}
	
}

//------------------------------------------------------------------------------------------------------------
//�����ɍ��v����L���������X�g����폜����
void ObjectManager::EraseCharaList()
{
#if 0
	{//�L�����N�^�[���X�g����폜(alive��false�̏ꍇ�ɍ폜)

		//��ł͂Ȃ����
		if (!charactor_list.empty()) {

			//remove_if���\�b�h�ŏ����ɍ����v�f���R���e�i�̖����Ɉړ������A
			charactor_list.erase(remove_if(charactor_list.begin(), charactor_list.end(), [&](std::shared_ptr<Enemy>) {
				//����
				return !factory->GetEnemy()->GetIsAlive();
				}), charactor_list.end());

		}

}


	{//�������Ă���G���X�g����폜(alive��false�̏ꍇ�ɍ폜)

		//��ł͂Ȃ����
		if (!alive_enemy.empty()) {

			//remove_if���\�b�h�ŏ����ɍ����v�f���R���e�i�̖����Ɉړ������A
			alive_enemy.erase(remove_if(alive_enemy.begin(), alive_enemy.end(), [&](std::shared_ptr<Enemy>) {
				//����
				!factory->GetEnemy()->GetIsAlive();

				}), alive_enemy.end());

		}

	}
#endif // 0


	////������null�`�F�b�N�����̂͂��肩��
	////�G�������X�g�ƁA�L�����N�^���X�g����aliveflag������Ă���̃L��������������
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
//���ׂẴ��X�g���N���A����֐�
void ObjectManager::AllListClear(bool player_deletion_flag)
{	

	//�v���C���[�𐶂����ꍇ
	if (!player_deletion_flag) {
		
		//�I�u�W�F�N�g���X�g����ł͂Ȃ����
		if (!charactor_list.empty()) {
			std::list<std::shared_ptr<Character>>::iterator entity;

			for (entity = charactor_list.begin(); entity != charactor_list.end();) {
				//�v���C���[�ȊO�̃L������object_list�������
				if ((*entity)->GetObjectType() != Object::ObjectType::PLAYER) {
					entity = charactor_list.erase(entity);
					continue;
				}
				entity++;
			}
		}
	}
	//�v���C���[�𐶂����Ȃ��ꍇ
	else {
		//�L�����i�[���X�g
		charactor_list.clear();
	}

	//�I�u�W�F�N�g�i�[���X�g
	object_list.clear();
	//�������i�[���X�g
	alive_enemy.clear();
	//�������A�C�e���i�[���X�g
	alive_item.clear();
	
}

//------------------------------------------------------------------------------------------------------------
//�}�b�v�`�b�v���X�g�ƕǃ��X�g���폜����
void ObjectManager::AllChipListClear()
{
	all_chip_list.clear();
	wall_chip.clear();
}

//------------------------------------------------------------------------------------------------------------
//�L�����N�^�[�̃X�e�[�^�X����������
void ObjectManager::RainForceCharaStetus()
{
	//�K�w�ړ����ɑS�L�����ꗥ�������A�G�̂݋����Y�݂ǂ���

	//�L�������X�g�Ɋi�[����Ă���L�����̃X�e�[�^�X����������
	for (auto chara : charactor_list) {
		//�G�̎��̂�
		if (chara->GetObjectType() == Object::ObjectType::ENEMY) {
			chara->ReinforceStetus();
		}
	}

}

//------------------------------------------------------------------------------------------------------------
//�v���C���[�̃X�|�[���ꏊ��ύX����֐�
void ObjectManager::SetPlayerSpawnType(Factory::PlayerSpawn next_spawn)
{
	factory->SetPlayerSpawn(next_spawn);
}

//------------------------------------------------------------------------------------------------------------
//�G�𐶐�����Ƃ��ɂǂ̓G�𐶐����邩�������_���Ɍ��߂�֐�
std::string ObjectManager::GetRandKindEnemyValue()
{
	//�����_���ɓG�ꗗ�̗񋓌^�̒����烉���_���ɒl��Ԃ�
	int rand_name = game_manager->GetRandValue((int)EnemyManager::EnemyName::SKELETON, (int)EnemyManager::EnemyName::KING);
	//�G�L�����ԍ��ꗗ
	//0 : Skeleton
	//1 : PoisonSkeleton
	//2 : BrackSkeleton
	//3 : Magician
	//4 : King
	std::string name[] = { "Skeleton" ,"PoisonSkeleton" ,"BrackSkeleton" ,"Magician" ,"King" };

	//�߂�l�͓G�L�����ԍ�
	return name[rand_name];

}

//------------------------------------------------------------------------------------------------------------
std::string ObjectManager::GetRandKindItemValue()
{
	//�����S,��,�h���,�_�C�A�����h,Gold,�h���b�v�A�C�e��,�|�[�V����

	int rand_item = game_manager->GetRandValue((int)ItemManager::ItemName::APPLE, (int)ItemManager::ItemName::ATTACK);
	//�A�C�e���̖��O�z��@
	std::string name[] = { "�����S","��","�h���","�_�C�A�����h","Gold","�h���b�v�A�C�e��","�|�[�V����","�U����"};

	return name[rand_item];
}

//------------------------------------------------------------------------------------------------------------
//�L�����N�^�[���g���X�L���f�[�^��SkillManager�ɓn��
void ObjectManager::GiveSkillEffectData()
{
	//�L�����X�e�[�^�X���擾(�v���C���[�̐����^�̃X�e�[�^�X�����ׂĎ擾(Hp,Mp,Attack,Defence))
	std::vector<std::vector<int>>palam = std::get<std::vector<std::vector<int>>>(stetus->GetEffect(Stetus::EffectDataType::INT));
	//�L�����l�[�����擾
	//�摜�p�X���擾
	std::vector<std::vector<std::string>> palam_string = std::get<std::vector<std::vector<std::string>>>(stetus->GetEffect(Stetus::EffectDataType::STRING));

	skill_manager = std::make_shared<SkillManager>(palam, palam_string, game_manager);
}

//------------------------------------------------------------------------------------------------------------
//�L�����N�^�[�̃X�L����ݒ肷��֐�
void ObjectManager::SetCharaSkill(std::vector<Skill*>& skill_list, SkillManager::UseEffectType type, SkillManager::EffectName name)
{
	skill_list.emplace_back(skill_manager->GetSkill(type, name));
}

//------------------------------------------------------------------------------------------------------------
//�v���C���[�̃X�L�����Z�b�g����
void ObjectManager::SetPlayerSkill()
{
	//�ʏ�U��
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::NOMALATTCK);
	//���U��
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::WATER);
	//���U��
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::WIND);
	//��U��
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::ROCK);
	//�΍U��
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::FIRE);
	//���U��
	SetCharaSkill(factory->GetPlayer()->GetSkillList(), SkillManager::UseEffectType::PLAYER, SkillManager::EffectName::THUNDER);
}

//------------------------------------------------------------------------------------------------------------
//�G�X�L�����Z�b�g����
void ObjectManager::SetEnemySkill(std::string name)
{
	//�X�L���Z�b�g
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



