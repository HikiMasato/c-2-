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
#include "MyCamera.h"
#include "Player.h"
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


//------------------------------------------------------------------------------------------------------------
// ���t���[�����s����܂�
void SceneEnd::Update(float delta_time) {

	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		auto mgr = SceneManager::GetInstance();
		mgr->ChangeScene(new SceneTitle);
	}
}

//------------------------------------------------------------------------------------------------------------

void SceneEnd::Draw() {
	DrawStringEx(10, 10, -1, "SceneEnd");
}