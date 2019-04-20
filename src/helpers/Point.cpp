#include "Point.hpp"

namespace helpers {


Point::Point() = default;

Point::Point(const double x, const double y)
	: x(x)
	, y(y)
{
}

Point::Point(const Point&) = default;

Point& Point::operator=(const Point&) = default;

Point::Point(Point&&) = default;

Point& Point::operator=(Point&&) = default;

Point::~Point() = default;

double Point::getX() const
{
	return x;
}

double Point::getY() const
{
	return y;
}

void Point::setX(const double newX)
{
	x = newX;
}

void Point::setY(const double newY)
{
	y = newY;
}

} // namespace helpers

