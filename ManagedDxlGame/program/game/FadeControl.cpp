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
#include "DungeonScene.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Others
#include "FadeControl.h"








//------------------------------------------------------------------------------------------------------------
void FadeControl::FadeOut()
{
	if (screen_fade > 0 && screen_fade <= 256) {
		SetDrawBright(screen_fade, screen_fade, screen_fade);

		screen_fade -= FADESPEED;
	}
	else {
		screen_fade = 0;
		done_fade = true;
	}
}

//------------------------------------------------------------------------------------------------------------
void FadeControl::FadeIn()
{
	if (screen_fade >= 0 && screen_fade < 256) {
		SetDrawBright(screen_fade, screen_fade, screen_fade);

		screen_fade += FADEINSPEED;
	}
	else {
		screen_fade = 256;
		done_fade = false;
	}
}
