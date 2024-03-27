//-------------------------------------------------------
//�����̂���
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//�l�I�ɒǉ������@�\
#include <string_view>
#include <random>         
#include <iostream>  
#include <variant>
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
#include "ResourceManager.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "SkillManager.h"
#include "SoundManager.h"
#include "ItemManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "Player.h"
#include "Enemy.h"
#include "MyCamera.h"
#include "MenuWindow.h"
#include "Item.h"
//-------------------------------------------------------
//Collision
#include "Collision.h" 
//-------------------------------------------------------
//others
#include "Stetus.h"
#include "Debug.h"
#include "Skill.h"
#include "Inventory.h"

Inventory::Inventory(int my_inventory_value_, GameManager* game_manager_)
{
	game_manager = game_manager_;
	cursor_handle = game_manager->LoadGraphEx("graphics/gamecursor.png");
	select_item_back_handle = game_manager->LoadGraphEx("testgraphics/selectItemBack.png");
	my_inventory_value = my_inventory_value_ + 1;

}

Inventory::~Inventory()
{
}

//�C���x���g���Ɉ����̃A�C�e����ǉ�����֐�
void Inventory::AddInventory(std::shared_ptr<Item> item)
{
	//���݂̃C���x���g���ɓ���ő吔(max_inventory�ƕϐ��������ق�����������)
	if (inventory_list.size() >= 10)return;
	//�C���x���g���ɃA�C�e����ǉ�
	inventory_list.emplace_back(item);
	//�A�C�e�������ǉ�
	item_value++;

}


//�J�[�\���̏㉺�ړ�����
void Inventory::CursorMove()
{
	//�J�[�\���̏㉺�ړ�
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_UP)) {
		item_value = GetItemCount();
		//�J�[�\���ړ������Ƃ���se��炷
		game_manager->GetSoundManager()->ChosePlaySystemSound(game_manager->GetSoundManager()->sound_csv[11]);
		select_cursor = (select_cursor + (item_value - 1)) % item_value;
	
	}
	//��
	else if (tnl::Input::IsKeyDownTrigger(eKeys::KB_DOWN)) {
		item_value = GetItemCount();
		//�J�[�\���ړ������Ƃ���se��炷
		game_manager->GetSoundManager()->ChosePlaySystemSound(game_manager->GetSoundManager()->sound_csv[11]);
		select_cursor = (select_cursor + 1) % item_value;

	}
}

//�J�[�\���̈ʒu����ԏ�Ƀ��Z�b�g����֐�
void Inventory::CursorReset()
{
	select_cursor = 0;
}


//�C���x���g�����̃A�C�e������`�悷��֐�
void Inventory::DrawInventory(const int x, const int y)
{
	int i = 0;
	for (auto item : inventory_list) {
		//�A�C�e�����̕`��
		DrawStringEx(x + 80, y + 10 + 30 * i, -1, "%s", item->GetItemName().c_str());
		i++;
	}
	DrawRotaGraph(x + 30, y + 20 + select_cursor * 30, 0.7, 0, cursor_handle, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 30);
	DrawRotaGraph(x + 220, y + 20 + select_cursor * 30, 1, 0, select_item_back_handle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);


}


//�A�C�e���̐�����`�悷��֐�
void Inventory::DrawItemDesc(const int x, const int y)
{
	SetFontSize(14);

	//�C���x���g�����̃A�C�e�����Ȃ���Ώ����X���[
	if (inventory_list.empty())return;
	auto item_desc = inventory_list.begin();
	for (int i = 0; i < select_cursor; i++) {
		if (item_desc == inventory_list.end()) {
			break;
		}
		item_desc++;
	}
	//�A�C�e��������`��
	(*item_desc)->DrawItemStringData(x, y);
}


//�V���b�v�C���x���g���p(�w���z�A���p�z)
void Inventory::DrawNeedCoin(int x, int y)
{
	if (inventory_list.empty())return;
	auto item = inventory_list.begin();
	for (int i = 0; i < select_cursor; i++) {
		//�C�e���[�^���Ō�̃R���e�i�ɂ���Ȃ�
		if (item == inventory_list.end()) {
			break;
		}
		item++;
	}
	//�K�v�R�C����
	int need_coin = 0;
	//�A�C�e���̉��i�擾
	need_coin = (*item)->GetItemPrice();
	SetFontSize(25);
	DrawStringEx(x + 10, y + 10, -1, "�w���K�v�R�C��:%d", need_coin);
	DrawStringEx(x + 10, y + 60, -1, "�����R�C��:%d", game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::GOLD));
	SetFontSize(16);
}

int Inventory::GetCursorValue()
{
	return select_cursor;
}

//�J�[�\���̈ʒu���Z�b�g����
void Inventory::SetCursorValue(const int move_num)
{
	if (select_cursor != inventory_list.size())return;
	select_cursor += move_num;
	if (select_cursor < 0)select_cursor = 0;
}

