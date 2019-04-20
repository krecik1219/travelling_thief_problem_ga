#pragma once

namespace helpers {

class Point
{
public:
	Point();
	Point(const double x, const double y);
	Point(const Point&);
	Point& operator=(const Point&);
	Point(Point&&);
	Point& operator=(Point&&);
	~Point();

	double getX() const;
	double getY() const;
	void setX(const double newX);
	void setY(const double newY);

private:
	double x;
	double y;
};

} // namespace helpers
