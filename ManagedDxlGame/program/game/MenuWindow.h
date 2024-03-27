///********************************
///UI�Ɋւ���N���X
///�������ɑ傫�������肳��LoadDivGraph�ɂ���ĕ����A
///�������ꂽ�摜��UI�Ƃ��ĕ\�������
///
///******************************** 
#pragma once

class Menu {
public:
	Menu() {}
	virtual ~Menu(){}
	//���j���[���W�A������
	int menu_x, menu_y, menu_width, menu_height = 0;
	int menu_gh[9] = { 0 };
	bool menu_alive ;

	//new����Ƃ��ɑ傫����gh��path�������œn��
	Menu(int menu_window_x, int menu_window_y, int menu_window_width, int menu_window_height, std::string ui_gh_path);

	void MenuDraw();
};

class MenuWindow : public Menu {
public:
	typedef struct {
		int x;
		int y;
		std::string name;
		int menu_value;
	}MenuType_t;

	MenuWindow(int menu_window_x, int menu_window_y, int menu_window_width, int menu_window_height, std::string ui_path, MenuType_t* type, int type_value, double back_width);
	~MenuWindow();

	//���j���[���J��
	void MenuOpen();
	void MenuRead();
	void MenuAll();

	//���݂̑I��ԍ�
	int select_value;
	bool menu_init = false;

	int read_menu_x = 0;
	int read_menu_y = 0;
	int read_menu_type_value = 0;

	MenuType_t* read_menu_type = nullptr;

	bool second_menu_flag = false;
	//read�֐���select_value�𓮂������ǂ���
	//true�Ȃ瓮����
	bool manage_select_flag = true;


private:

	int string_color_black = 0;
	int type_value;
	bool menu_start_opend;
	MenuType_t* menutype = nullptr;

	int cursor_gh = 0;
	int cursor_x = 0;
	int cursor_y = 0;

	int select_item_back = 0;
	double back_gh_width = 0;

};