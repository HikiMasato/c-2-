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
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "SceneManager.h"
#include "GameManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
#include "EffectManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MyCamera.h"
#include "Player.h"
//-------------------------------------------------------
//Effect
#include "ActionEffect.h" 
//-------------------------------------------------------
//Others
#include "Stetus.h"

//------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
ActionEffect::ActionEffect(std::vector<std::vector<std::string>>& effect_csv)
{
	ef_manager = std::make_shared<EffectManager>(effect_csv);
}

//------------------------------------------------------------------------------------------------------------
//
void ActionEffect::PlayAttack()
{
	//EffectManager* ef_manager = new EffectManager()
}

//------------------------------------------------------------------------------------------------------------
//���t���[�����s
void ActionEffect::Update(float delta_time)
{

}

//------------------------------------------------------------------------------------------------------------
//�`��
void ActionEffect::Draw(const hm::Camera& camera)
{	
	//�X�L���G�t�F�N�g�`��
	ef_manager->Draw(camera);
}

