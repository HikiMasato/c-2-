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
#include "GameManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "EffectManager.h"
#include "ItemManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MyCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
//-------------------------------------------------------
//Effect
#include "ActionEffect.h" 
//-------------------------------------------------------
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Debug.h"

//------------------------------------------------------------------------------------------------------------
//�`��
ItemManager::~ItemManager()
{
}

//------------------------------------------------------------------------------------------------------------
//�����R���X�g���N�^
ItemManager::ItemManager(std::vector<std::shared_ptr<Item>>& all_item_list)
{
	all_item.resize(all_item_list.size());
	all_item = all_item_list;
}

//------------------------------------------------------------------------------------------------------------
//�����ɓn���ꂽ�A�C�e�����ƈ�v����A�C�e����Ԃ�
std::shared_ptr<Item> ItemManager::GetItemData(std::string name)
{
	for (auto item : all_item) {
		//���O����v����ꍇ
		if (item->GetItemName() == name) {
			return item;
		}
	}
	return nullptr;
}

//------------------------------------------------------------------------------------------------------------
//����̃A�C�e���̃|�C���^��Ԃ�
std::shared_ptr<Item> ItemManager::GetSpecificItem(std::string name)
{
	for (int i = 0; i, all_item.size(); i++) {
		//�A�C�e���̖��O�������Ȃ�
		if (name == all_item[i]->GetItemName()) {
			return all_item[i];
		}
	}
	return nullptr;
}


