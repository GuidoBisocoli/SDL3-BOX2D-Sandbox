#pragma once

#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "box2d/box2d.h"

#include "Texture.h"

class Sandbox
{
public:
	Sandbox();
	~Sandbox();

	bool setup();

	bool loop();

	static b2WorldId getWorldId() { return worldId; }
private:
	// constants
	const int INITIALWIDTH = 1280;
	const int INITIALHEIGHT = 800;

	// Game
	int screenWidth = INITIALWIDTH;
	int screenHeight = INITIALHEIGHT;
	Uint64 lastTime = 0u;
	Uint64 currentTime = 0u;
	float delta = 0.f;

	// SDL
	Texture background;

	// Box2D
	static b2WorldId worldId;
	b2BodyId floorBodyId = b2_nullBodyId;
	b2BodyId rightWallBodyId = b2_nullBodyId;

	// Sandbox
	void adjustLimitsForNewResolution();
	bool fullscreen = false;
	void toggleFullScreen();
	b2BodyId createStatic(b2Vec2 pos, b2Vec2 halfSize);

	// Mouse grab
	b2BodyId mouseBody = b2_nullBodyId;
	b2JointId mouseJointId = b2_nullJointId;
	void getBodyAtMousePos();
	void releaseBodyAtMousePos();
	void moveBodyAtMousePos(b2Vec2 p);

	// Other
	bool drawDebug = true;
	b2Vec2 getMousePos();
	void resizeScreen(int x, int y);
};

