#pragma once


class SceneBase {
public:
	
	virtual ~SceneBase() {}

	virtual void Update(float delta_time) = 0;
	virtual void Draw() = 0;


};