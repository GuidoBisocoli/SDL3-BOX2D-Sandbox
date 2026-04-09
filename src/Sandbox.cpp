#include "Sandbox.h"

#include <optional>
#include <iostream>
#include <string>

#include "SDL3/SDL.h"
#include <SDL3_image/SDL_image.h>

#include "SpriteSheet.h"
#include "ShapeCreator.h"
#include "Texture.h"
#include "Globals.h"
#include "Useful.h"

#include "DebugDraw.h"

b2WorldId Sandbox::worldId = {};

//-----------//
/// SANDBOX ///
//-----------//
Sandbox::Sandbox() { }

Sandbox::~Sandbox()
{
	/// BOX2D
	if (B2_IS_NON_NULL(mouseJointId)) {
		b2DestroyJoint(mouseJointId);
		mouseJointId = b2_nullJointId;
	}

	if (B2_IS_NON_NULL(mouseJointId)) {
		b2DestroyBody(mouseBody);
		mouseBody = b2_nullBodyId;
	}

	b2DestroyWorld(worldId);
	worldId = b2_nullWorldId;

	/// SDL
	background.destroy();
	TextTexture.destroy();

	TTF_CloseFont(Font);
	Font = nullptr;

	SDL_DestroyRenderer(Renderer);
	Renderer = nullptr;
	SDL_DestroyWindow(Window);
	Window = nullptr;

	TTF_Quit();
	SDL_Quit();
}

bool Sandbox::setup()
{
	// SDL
	if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		SDL_Log("Error initializing SDL - SDL error: %s\n", SDL_GetError());
		return false;
	}

	if (SDL_CreateWindowAndRenderer("Sandbox SDL3 Box2D v3", screenWidth, screenHeight, 0, &Window, &Renderer) == false)
	{
		SDL_Log("Window could not be created - SDL error: %s\n", SDL_GetError());
		return false;
	}

	if (TTF_Init() == false)
	{
		SDL_Log("Error initializing SDL_ttf - SDL_ttf error: %s\n", SDL_GetError());
		return false;
	}

	//Load background texture
	if (background.loadFromFile("images/background.png") == false)
	{
		SDL_Log("Unable to load background image\n");
		return false;
	}

	//Load scene font
	std::string fontPath{ "arial.ttf" };
	if (Font = TTF_OpenFont(fontPath.c_str(), 20); Font == nullptr)
	{
		SDL_Log("Could not load %s SDL_ttf - Error: %s\n", fontPath.c_str(), SDL_GetError());
		return false;
	}
	else
	{
		//Load text
		SDL_Color textColor{ 0x00, 0x00, 0x00, 0xFF };
		if (TextTexture.loadFromRenderedText("C: spawn random crate - B: spawn random box - Space: spawn random crate or box - F: toggle fullscreen - Grab them with the mouse", textColor) == false)
		{
			SDL_Log("Could not load text texture %s - SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
			return false;
		}
	}

	lastTime = SDL_GetTicks();
	delta = 0.0f;

	// BOX2D
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = { 0.f, 9.8f };
	worldId = b2CreateWorld(&worldDef);

	debugDrawer = b2DefaultDebugDraw();
	debugDrawer.drawShapes = true;
	debugDrawer.DrawSolidPolygonFcn = drawSolidPolygon;

	// floor (bodyId needed for when resizing screen)
	floorBodyId = createStatic(b2Vec2{ 0.f , floatInBox2DWorld(screenHeight) + 1.f }, b2Vec2{ 200.f, 1.0f });
	// ceiling
	createStatic(b2Vec2{ 0.f, -1.f }, b2Vec2{ 200.f, 1.0f });
	// left wall
	createStatic(b2Vec2{ -1.f, 0.f }, b2Vec2{ 1.f, 200.f });
	// right wall (bodyId needed for when resizing screen)
	rightWallBodyId = createStatic(b2Vec2{ floatInBox2DWorld(screenWidth) + 1.f, 0.f }, b2Vec2{ 1.f, 200.f });

	return true;
}

bool Sandbox::loop()
{
	static bool running = true;

	SDL_Event e;
	SDL_zero(e);

	currentTime = SDL_GetTicks();
	delta = (currentTime - lastTime) / 1000.0f;
	lastTime = currentTime;

	// input
	while (SDL_PollEvent(&e) == true)
	{
		if (e.type == SDL_EVENT_QUIT)
			running = false;

		static bool ready = true; // avoid spamming keys
		if (e.type == SDL_EVENT_KEY_DOWN && ready) {			
			ready = false;

			const bool* keys = SDL_GetKeyboardState(NULL);
			if (e.key.key == SDLK_ESCAPE) running = false;
			if (e.key.key == SDLK_SPACE) SHAPECREATOR.createShape(getMousePos(), SHAPECREATOR.ShapeType::any);
			if (e.key.key == SDLK_C) SHAPECREATOR.createShape(getMousePos(), SHAPECREATOR.ShapeType::crate);
			if (e.key.key == SDLK_B) SHAPECREATOR.createShape(getMousePos(), SHAPECREATOR.ShapeType::box);
			if (e.key.key == SDLK_D) drawDebug = !drawDebug;
			if (e.key.key == SDLK_F) toggleFullScreen();
		}
		else if (e.type == SDL_EVENT_KEY_UP) ready = true;

		// mouse
		if ((e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)	&& (e.button.button == SDL_BUTTON_LEFT))
			getBodyAtMousePos();
		else if ((e.type == SDL_EVENT_MOUSE_BUTTON_UP) && (e.button.button == SDL_BUTTON_LEFT))
			releaseBodyAtMousePos();
		else if (e.type == SDL_EVENT_MOUSE_MOTION)
			moveBodyAtMousePos(getMousePos());
	}

	// loop
	if (running) {
		// Box2D
		b2World_Step(worldId, delta, 4);

		// draw
		SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(Renderer);
		background.renderAsBackground(screenWidth, screenHeight);

		// shapes
		SHAPECREATOR.drawShapes();

		// debug draw
		if (drawDebug) b2World_Draw(worldId, &debugDrawer); // Drawing collider lines for debugging

		SDL_FRect rect{ 0.f, 0.f, TextTexture.getSize().x, TextTexture.getSize().y };
		TextTexture.render(rect, rect, 0.f);

		// Update screen
		SDL_RenderPresent(Renderer);
	}

	return running;
}

void Sandbox::adjustLimitsForNewResolution()
{
	b2Vec2 newPos = b2Vec2{ floatInBox2DWorld(screenWidth / 2) , floatInBox2DWorld(screenHeight) + 1.f };
	b2Body_SetTransform(floorBodyId, newPos, b2Body_GetRotation(floorBodyId));

	newPos = b2Vec2{ floatInBox2DWorld(screenWidth) + 1.f , floatInBox2DWorld(screenHeight / 2) };
	b2Body_SetTransform(rightWallBodyId, newPos, b2Body_GetRotation(rightWallBodyId));
}

void Sandbox::toggleFullScreen()
{
	if (fullscreen) {
		SDL_SetWindowFullscreen(Window, false);

		resizeScreen(INITIALWIDTH, INITIALHEIGHT);

		fullscreen = false;
	}
	else {
		SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();

		const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(primaryDisplay);

		resizeScreen(mode->w, mode->h);

		fullscreen = true;

		SDL_SetWindowFullscreen(Window, true);
	}
}

b2BodyId Sandbox::createStatic(b2Vec2 pos, b2Vec2 halfSize)
{
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_staticBody;
	bodyDef.position = pos;
		
	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	b2Polygon box = b2MakeBox(halfSize.x, halfSize.y);
	b2ShapeDef shapeDef = b2DefaultShapeDef();

	bodyId = b2CreateBody(worldId, &bodyDef);

	b2CreatePolygonShape(bodyId, &shapeDef, &box);

	return bodyId;
}

//---------//
/// QUERY ///
//---------//
struct QueryContext
{
	b2Vec2 point;
	b2BodyId bodyId = b2_nullBodyId;
};

bool QueryCallback(b2ShapeId shapeId, void* context)
{
	QueryContext* queryContext = static_cast<QueryContext*>(context);

	b2BodyId bodyId = b2Shape_GetBody(shapeId);
	b2BodyType bodyType = b2Body_GetType(bodyId);
	if (bodyType != b2_dynamicBody)
	{
		// continue query
		return true;
	}

	bool overlap = b2Shape_TestPoint(shapeId, queryContext->point);
	if (overlap)
	{
		// found shape
		queryContext->bodyId = bodyId;
		return false;
	}

	return true;
}

void Sandbox::getBodyAtMousePos()
{
	b2Vec2 mousePos = getMousePos();

	// Make a small box.
	b2AABB aabb;
	b2Vec2 d{ 0.001f, 0.001f };
	aabb.lowerBound = mousePos - d;
	aabb.upperBound = mousePos + d;

	// Query the world for overlapping shapes.
	QueryContext queryContext = { mousePos, b2_nullBodyId };
	b2World_OverlapAABB(worldId, aabb, b2DefaultQueryFilter(), QueryCallback, &queryContext);

	if (B2_IS_NON_NULL(queryContext.bodyId))
	{
		b2BodyDef bodyDef = b2DefaultBodyDef();
		mouseBody = b2CreateBody(worldId, &bodyDef);

		b2MouseJointDef mouseDef = b2DefaultMouseJointDef();
		mouseDef.bodyIdA = mouseBody;
		mouseDef.bodyIdB = queryContext.bodyId;
		mouseDef.target = mousePos;
		mouseDef.hertz = 10.0f;
		mouseDef.dampingRatio = 0.7f;
		mouseDef.maxForce = 1000.0f * b2Body_GetMass(queryContext.bodyId) * b2Length(b2World_GetGravity(worldId));
		mouseJointId = b2CreateMouseJoint(worldId, &mouseDef);

		b2Body_SetAwake(queryContext.bodyId, true);
	}
}

void Sandbox::releaseBodyAtMousePos()
{
	if (B2_IS_NON_NULL(mouseJointId))
	{
		b2DestroyJoint(mouseJointId);
		mouseJointId = b2_nullJointId;

		b2DestroyBody(mouseBody);
		mouseBody = b2_nullBodyId;
	}
}

void Sandbox::moveBodyAtMousePos(b2Vec2 p)
{
	if (B2_IS_NON_NULL(mouseJointId))
	{
		b2MouseJoint_SetTarget(mouseJointId, p);
		b2BodyId bodyIdB = b2Joint_GetBodyB(mouseJointId);
		b2Body_SetAwake(bodyIdB, true);
	}
}

b2Vec2 Sandbox::getMousePos()
{
	float x = 0.f;
	float y = 0.f;
	SDL_GetMouseState(&x, &y);

	return vecInBox2DWorld(x, y);
}

void Sandbox::resizeScreen(int x, int y)
{
	int prev_x, prev_y;
	SDL_GetWindowSizeInPixels(Window, &prev_x, &prev_y);

	SDL_SetWindowSize(Window, x, y);

	screenWidth = x;
	screenHeight = y;

	// move floor and right wall
	adjustLimitsForNewResolution();

	// move the shapes
	SHAPECREATOR.repositionShapes(SDL_Point{ prev_x, prev_y }, SDL_Point{ x, y });
}
