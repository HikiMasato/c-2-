#pragma once

class SceneBase;
class DungeonScene;

class ScenePlay : public SceneBase {
public:
	ScenePlay();
	~ScenePlay();
	

	void Update(float delta_time) override;
	void Draw() override;
	
	DungeonScene* seq_dungeon = nullptr;
};

