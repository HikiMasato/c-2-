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
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "SkillManager.h"
#include "SoundManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "Player.h"
#include "Enemy.h"
#include "MyCamera.h"
#include "MenuWindow.h"
//-------------------------------------------------------
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//others
#include "Stetus.h"
#include "Debug.h"
#include "Skill.h"


//------------------------------------------------------------------------------------------------------------
//
Menu::Menu(int menu_window_x, int menu_window_y, int menu_window_width, int menu_window_height, std::string ui_gh_path)
{
	menu_x = menu_window_x;
	menu_y = menu_window_y;
	menu_width = menu_window_width;
	menu_height = menu_window_height;

	int menu = LoadDivGraph(ui_gh_path.c_str(), 9, 3, 3, 16, 16, menu_gh);
	menu_alive = false;
}

//------------------------------------------------------------------------------------------------------------
//
void Menu::MenuDraw()
{
	DrawGraph(menu_x, menu_y, menu_gh[0], TRUE);
	DrawExtendGraph(menu_x + 16, menu_y, menu_x + menu_width - 16, menu_y + 16, menu_gh[1], TRUE);
	DrawGraph(menu_x + menu_width - 16, menu_y, menu_gh[2], TRUE);

	DrawExtendGraph(menu_x, menu_y + 16, menu_x + 16, menu_y + menu_height - 16, menu_gh[3], TRUE);
	DrawExtendGraph(menu_x + 16, menu_y + 16, menu_x + menu_width - 16, menu_y + menu_height - 16, menu_gh[4], TRUE);
	DrawExtendGraph(menu_x + menu_width - 16, menu_y + 16, menu_x + menu_width, menu_y + menu_height - 16, menu_gh[5], TRUE);

	DrawGraph(menu_x, menu_y + menu_height - 16, menu_gh[6], TRUE);
	DrawExtendGraph(menu_x + 16, menu_y + menu_height - 16, menu_x + menu_width - 16, menu_y + menu_height, menu_gh[7], TRUE);
	DrawGraph(menu_x + menu_width - 16, menu_y + menu_height - 16, menu_gh[8], TRUE);

}


//------------------------------------------------------------------------------------------------------------
//
MenuWindow::MenuWindow(int menu_window_x, int menu_window_y, int menu_window_width, int menu_window_height, std::string ui_path, MenuType_t* type, int type_value_, double back_width)
	//���j���[�̑傫�������߂�(�J�n���W : ����,����,�c��)
	: Menu(menu_window_x, menu_window_y, menu_window_width, menu_window_height, ui_path)
	, select_value(0)
	, menu_start_opend(false)
	, read_menu_x(menu_window_x)
	, read_menu_y(menu_window_y)
	, read_menu_type_value(type_value_)
	, read_menu_type(type)
	, back_gh_width(back_width)
{
	//�O�Ŏg���ꍇ�͂���Ȋ����ŌĂяo��
	/*MenuElement_t* p = new MenuElement_t[]{
		{40,30,"�X�e�[�^�X",0},
		{40,80,"������",1},
		{40,130,"�K�E�Z",2},
		{40,180,"�}�b�v",3},
		{40,230,"�^�C�g����",4},
	};

	MenuWindow* p = new MenuWindow(10, 10, 10, 10, p, 5);*/

	menutype = type;
	type_value = type_value_;
	string_color_black = GetColor(255, 255, 255);
	cursor_x = menutype[0].x - 20;
	cursor_gh = SceneTitle::game_manager->LoadGraphEx("using_graphics/gamecursor.png");
	select_item_back = SceneTitle::game_manager->LoadGraphEx("testgraphics/selectItemBack.png");

}

//------------------------------------------------------------------------------------------------------------
//�f�X�g���N�^
MenuWindow::~MenuWindow()
{
	//�z��̔j��
	if (menutype) {
		delete[] menutype;
	}
}

//------------------------------------------------------------------------------------------------------------
//�~�j�}�b�v��(�N��)alive = true, start_opend = true, select_value = true,
void MenuWindow::MenuOpen()
{
	menu_alive = true;
	menu_start_opend = true;
	select_value = 0;
	menu_init = false;
}

//------------------------------------------------------------------------------------------------------------
//�~�j�}�b�v�̕`��
void MenuWindow::MenuRead()
{
	if (menu_init == false) {

		cursor_y = menutype[0].y + 8;
		menu_init = true;
	}
	if (menu_start_opend == true)return;
	if (manage_select_flag != false) {

		//���j���[��ʂ̑���
		//�I��ԍ������炷
		if (tnl::Input::IsKeyDownTrigger(eKeys::KB_DOWN)) {
			select_value = (select_value + 1) % read_menu_type_value;
			//����炷
		
			SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);

		}
		//��
		else if (tnl::Input::IsKeyDownTrigger(eKeys::KB_UP)) {
			select_value = (select_value + (read_menu_type_value - 1)) % read_menu_type_value;
			//����炷
			SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);
		}

		//�J�[�\���ړ�
		if (tnl::Input::IsKeyDownTrigger(eKeys::KB_DOWN) || tnl::Input::IsKeyDownTrigger(eKeys::KB_UP)) {
			for (int i = 0; i < type_value; i++) {
				if (i == select_value) {
					cursor_y = menutype[i].y + 8;
				}
			}
		}

	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
	DrawRotaGraph3(cursor_x + 10, cursor_y - 15, 0, 0, back_gh_width, 1, 0, select_item_back, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	for (int i = 0; i < type_value; i++) {
		DrawFormatString(menutype[i].x, menutype[i].y, GetColor(255, 255, 255), menutype[i].name.c_str());
	}
	DrawRotaGraph(cursor_x, cursor_y, 0.5, 0, cursor_gh, true);
}

//------------------------------------------------------------------------------------------------------------
//�ꊇ�ŕ`��A�I���������Ăяo��
void MenuWindow::MenuAll()
{
	if (menu_alive) {
		MenuDraw();
		MenuRead();
		menu_start_opend = false;
	}
}


