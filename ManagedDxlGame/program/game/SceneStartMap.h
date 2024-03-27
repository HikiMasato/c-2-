///****************Description********************
///�X�^�[�g�}�b�v(�����}�b�v)�N���X
///�_���W�������ɓ���O�̃L�����v�V�[��
/// 
///*********************************************** 
#pragma once
#include "MyCamera.h"

class SceneBase;
class Factory;
class MapChip;
class Player;
class hm::Camera;
class MenuWindow;

class SceneStartMap : public SceneBase {
public:
	SceneStartMap();
	~SceneStartMap();
	

private:

		
	//***************************�ϐ�*****************************
	hm::Camera camera;
	MenuWindow* map_in_ui = nullptr;

	
	//�X�^�[�ƃ}�b�v�̃v���C���[���W��ۑ����Ă���
	tnl::Vector3 player_pos_buff = { 0,0,0 };

	//�_���W�����ɓ���UI��\�������邩�ǂ����̃t���O
	bool map_in_ui_open = false;
	
	
	

	//***************************�֐�*****************************
	//SceneStartMap������
	void InitSceneStartMap();
	void Update(float delta_time) override;
	void Draw() override;
	
	//�_���W�����ɓ���UI��\������`�F�b�N�֐�
	bool CheckDungeonInUi(tnl::Vector3 pos);

	void ActiveKeyCheck(bool open_in);




};