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
#include "EnemyManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MyCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "MapChip.h"
//-------------------------------------------------------
//Collisition
#include "Collision.h"
//-------------------------------------------------------
//Others
#include "Debug.h"


//------------------------------------------------------------------------------------------------------------
//�����蔻��̃y�A���X�g�����ׂč폜����
void Collision::ClearIntersectList()
{
	intersect_list.clear();
}

//------------------------------------------------------------------------------------------------------------
//
void Collision::ClearIntersectMap()
{
	intersect_map.clear();
}
