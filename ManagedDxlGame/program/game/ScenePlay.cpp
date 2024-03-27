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
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MyCamera.h"
#include "Player.h"
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "DungeonScene.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "GameManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
//-------------------------------------------------------
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//Others
#include "Debug.h"


//------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
ScenePlay::ScenePlay() {

	
	seq_dungeon = new DungeonScene();

}
ScenePlay::~ScenePlay() {
	delete seq_dungeon;
	seq_dungeon = nullptr;
}
//------------------------------------------------------------------------------------------------------------
// ���t���[�����s����܂�
void ScenePlay::Update(float delta_time) {

	
	seq_dungeon->Update(delta_time);
	
}

//------------------------------------------------------------------------------------------------------------
//�`��֐�
void ScenePlay::Draw() {
	//DrawStringEx(10, 10, -1, "ScenePlay");
	seq_dungeon->Draw();
}