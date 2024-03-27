#pragma once
#include "../dxlib_ext/dxlib_ext.h"

namespace hm {

	class Camera {
	public:
		enum RunIndex {
			UP,
			DOWN,
			LEFT,
			RIGHT
		};


		Camera() {}
		Camera(int screen_w, int screen_h) {
			screen_w_ = screen_w;
			screen_h_ = screen_h;
			aspect_ = (float)screen_w_ / (float)screen_h_;
		}
		virtual ~Camera() {}

		int screen_w_ = 0;
		int screen_h_ = 0;

		
		
		// カメラの３次元座標
		tnl::Vector3 pos_ = tnl::Vector3(0, 200, -500.0f);
		// カメラが３次元のどこを画面の中央にとらえているか
		tnl::Vector3 target_ = tnl::Vector3(0, 0, 0);
		// カメラの『上』を定義するアッパーベクトル
		tnl::Vector3 up_ = tnl::Vector3(0, 1, 0);

		// カメラの画角 ( 度 )
		float angle_ = tnl::ToRadian(60.0f);
		// カメラのアスペクト比 ( 縦横比 )
		float aspect_ = 1.0f;
		// カメラに映る範囲の最近距離
		float near_ = 1.0f;
		// カメラに映る範囲の最遠距離
		float far_ = 50000.0f;

		tnl::Matrix view_;
		tnl::Matrix proj_;

		virtual inline tnl::Vector3 forward() { return tnl::Vector3::Normalize(target_ - pos_); }
		virtual inline tnl::Vector3 left() { return tnl::Vector3::Cross(forward(), { 0, 1, 0 }); }
		virtual inline tnl::Vector3 right() { return tnl::Vector3::Cross({ 0, 1, 0 }, forward()); }
		virtual inline tnl::Vector3 back() { return tnl::Vector3::Normalize(pos_ - target_); }

		void update() {
			view_ = tnl::Matrix::LookAtLH(pos_, target_, up_);
			proj_ = tnl::Matrix::PerspectiveFovLH(angle_, aspect_, near_, far_);
		}

		void render(float scale, uint32_t color = 0xffffff00);

		//----------------------------------------------------------------------------------------------
		//自分で追加したもの
		tnl::Vector3 cameraPos = pos_;
		float cam_weight_ = 1.0f;
		//カメラを移動に関する処理が書いてある関数
		void MoveCamera();
		void Zoom();
	};

}
