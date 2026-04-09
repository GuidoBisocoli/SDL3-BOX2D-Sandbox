#include "ShapeCreator.h"

#include "Globals.h"
#include "Sandbox.h"
#include "FakeRandom.h"
#include "Useful.h"

void ShapeCreator::createShape(b2Vec2 position, ShapeType st)
{
	std::string filename = getRandomShape(st);

	std::map<std::string, SpriteSheetData::SpriteData>::const_iterator results = SpriteSheetData::Instance().SpritesData.find(filename);
	SpriteSheetData::SpriteData data = results->second;

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = position;
	bodyDef.fixedRotation = false;

	b2BodyId bodyId = b2CreateBody(Sandbox::getWorldId(), &bodyDef);
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1.f;
	b2Polygon b2Shape = b2MakeBox(floatInBox2DWorld(data.size.x) / 2.f, floatInBox2DWorld(data.size.y) / 2.f); // toma half-widths
	b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &b2Shape);

	shapes.push_back(new Shape{ data, bodyId });
}

std::string ShapeCreator::getRandomShape(ShapeType st)
{
	// randomness
	bool createCrate = getRandomBool();
	int number = getRandom0_9();
	bool big = getRandomBool();

	if (st == ShapeType::any) goto any;
	else if (st == ShapeType::crate) goto crate;
	else if (st == ShapeType::box) goto box;
	else goto error;

any:
	if (createCrate) goto crate;
	else goto box;

crate:
	switch (number)
	{
	case 0: return "crate_0.png";
	case 1: return "crate_1.png";
	case 2: return "crate_2.png";
	case 3: return "crate_3.png";
	case 4: return "crate_4.png";
	case 5: return "crate_5.png";
	case 6: return "crate_6.png";
	case 7: return "crate_7.png";
	case 8: return "crate_8.png";
	case 9: return "crate_9.png";
	}

	goto error;
box:
	if (big) return "box_big.png";
	else return "box_small.png";

error:
	return "Error in Sandbox::getRandomShape(...)";
}

void ShapeCreator::drawShapes()
{
	SDL_FRect textureRect{ 0.f, 0.f, 0.f, 0.f };
	SDL_FRect posOnScreen{ 0.f, 0.f, 0.f, 0.f };

	for (Shape* s : shapes) {

		textureRect.h = s->data.size.y;
		textureRect.w = s->data.size.x;
		textureRect.x = s->data.position.x;
		textureRect.y = s->data.position.y;
		posOnScreen.h = textureRect.h;
		posOnScreen.w = textureRect.w;

		SDL_FPoint pos = vecToSDL_FPoint(b2Body_GetPosition(s->bodyId));

		// correction (move posOnScreen to center of the sprite instead of 0, 0) 
		posOnScreen.x = pos.x - s->data.size.x / 2.f;
		posOnScreen.y = pos.y - s->data.size.y / 2.f;;

		b2Rot rot = b2Body_GetRotation(s->bodyId);
		double angle = radiansToDegrees(b2Rot_GetAngle(rot));
		
		TEXTURE.render(textureRect, posOnScreen, angle);
	}
}

void ShapeCreator::repositionShapes(SDL_Point previousSize, SDL_Point newSize)
{
	float proportion_x = (float)newSize.x / (float)previousSize.x;
	float proportion_y = (float)newSize.y / (float)previousSize.y;

	float x;
	float y;
	for (Shape* s : shapes) {
		x = b2Body_GetPosition(s->bodyId).x * proportion_x;
		y = b2Body_GetPosition(s->bodyId).y * proportion_y;
		b2Body_SetTransform(s->bodyId, b2Vec2{ x, y }, b2Body_GetRotation(s->bodyId));
		b2Body_SetAwake(s->bodyId, true);
	}
}
