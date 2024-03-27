#pragma once

class SceneBase;

//GameManager classnoの定義
class SceneManager {
public:
	~SceneManager();

	//static型でstart_scene用のアドレスをnullptrで確保する
	static SceneManager* GetInstance(SceneBase* start_scene = nullptr);

	//デフォルト引数を持たせる
	void ChangeScene(SceneBase* next_scene_, float trans_time = 0.5f);
	void Update(float delta_time);

	SceneBase* next_scene = nullptr;

	template<class T>
	static T* CastScene() {
		T* scene = dynamic_cast<T*>(next_scene);
		return scene;
	}

private:

	//宣言
	SceneManager(SceneBase* start_scene);
	float trans_time_ = 0.5f;
	SceneBase* now_scene = nullptr;
	//トランジションの画像ハンドル
	int tansition_graph_hdl = 0;

	tnl::Sequence<SceneManager> sequence = tnl::Sequence<SceneManager>(this, &SceneManager::SeqTransIn);
	bool SeqTransIn(const float delta_time);
	bool SeqTransOut(const float delta_time);
	bool SeqRunScene(const float delta_time);
};
