#pragma once

#include <vector>

#include "SpriteSheet.h"

#include "box2d/box2d.h"

#define SHAPECREATOR ShapeCreator::Instance()

class ShapeCreator
{
public:
	enum ShapeType { any, box, crate };

	static ShapeCreator& Instance() {
		static ShapeCreator theSC;
		return theSC;
	}

	void createShape(b2Vec2 position, ShapeType st);
	void drawShapes();
	void repositionShapes(SDL_Point previousSize, SDL_Point newSize);
private:
	ShapeCreator() {}
	ShapeCreator(ShapeCreator const&);
	ShapeCreator& operator= (ShapeCreator const&) {}
	~ShapeCreator() {}

	struct Shape {
		SpriteSheetData::SpriteData data;
		b2BodyId bodyId;
	};
	std::vector<Shape*> shapes;

	std::string getRandomShape(ShapeType st);
};