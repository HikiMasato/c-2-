//-------------------------------------------------------
//既存のもの
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//個人的に追加した機能
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
//コンストラクタ
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
//毎フレーム実行
void ActionEffect::Update(float delta_time)
{

}

//------------------------------------------------------------------------------------------------------------
//描画
void ActionEffect::Draw(const hm::Camera& camera)
{	
	//スキルエフェクト描画
	ef_manager->Draw(camera);
}

