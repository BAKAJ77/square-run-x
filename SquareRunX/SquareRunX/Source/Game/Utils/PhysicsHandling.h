#pragma once

enum class ColliderSide
{
	TOP, BOTTOM,
	LEFT, RIGHT,
	NONE
};

namespace PhysicsConstants
{
	static constexpr float GRAVITY = 0.0075f, TERMINAL_VELOCITY = 2.0f;
}

class ColliderBox
{
public:
	double x = 0.0, y = 0.0, w = 0.0, h = 0.0;
public:
	ColliderSide IsColliding(const ColliderBox& Other) const;

	double GetLeftSide() const;
	double GetRightSide() const;
	double GetTopSide() const;
	double GetBottomSide() const;
};