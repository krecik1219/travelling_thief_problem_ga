#pragma once

#include <helpers/Point.hpp>

#include <algorithm>
#include <functional>
#include <vector>
#include <memory>

using namespace helpers;

namespace gecco {

class HyperVolume2DCalculator
{
public:
	template <class PointLike>
	double calculateHyperVolume(const std::vector<PointLike>& points) const;

private:

	template <class T>
	T* ptr(T* p) const;
	
	template <class T>
	T* ptr(T& p) const;
	
	template <class T>
	T* ptr(std::unique_ptr<T> p) const;

	template <class PointLike>
	std::vector<Point> normalize(const std::vector<PointLike>& points) const;
};

template<class PointLike>
double HyperVolume2DCalculator::calculateHyperVolume(const std::vector<PointLike>& points) const
{
	if (points.size() < 3)
		return 0.0;

	auto normalizedPoints = normalize(points);
	Point refPoint(1, 1);
	double hv = 0.0;
	for (auto i = 1u; i < normalizedPoints.size() - 1; i++)
	{
		double X = refPoint.getX() - normalizedPoints[i].getX();
		double Y = normalizedPoints[i - 1].getY() - normalizedPoints[i].getY();
		hv += X * Y;
	}

	return hv;
}

template<class T>
T * HyperVolume2DCalculator::ptr(T* p) const
{
	return p;
}

template<class T>
T * HyperVolume2DCalculator::ptr(T& p) const
{
	return &p;
}

template<class T>
T* HyperVolume2DCalculator::ptr(std::unique_ptr<T> p) const
{
	return p.get();
}

template<class PointLike>
std::vector<Point> HyperVolume2DCalculator::normalize(const std::vector<PointLike>& points) const
{
	std::vector<Point> pVec;
	pVec.reserve(points.size());
	std::transform(points.cbegin(), points.cend(), std::back_inserter(pVec),
		[this](const auto& p) {return Point(ptr(p)->getX(), ptr(p)->getY()); }
	);
	std::stable_sort(pVec.begin(), pVec.end(), [](const auto& lhs, const auto& rhs) {return lhs.getX() < rhs.getX(); });
	const Point nadirPoint(pVec.back().getX(), pVec[0].getY());
	const Point idealPoint(pVec[0].getX(), pVec.back().getY());
	const double xDiff = nadirPoint.getX() - idealPoint.getX();
	const double yDiff = nadirPoint.getY() - idealPoint.getY();
	std::for_each(pVec.begin(), pVec.end(),
		[&xDiff, &yDiff, &idealPoint](auto& p)
		{
			p = Point((p.getX() - idealPoint.getX()) / xDiff, (p.getY() - idealPoint.getY()) / yDiff);
		}
	);
	return pVec;
}

} // namespace gecco
