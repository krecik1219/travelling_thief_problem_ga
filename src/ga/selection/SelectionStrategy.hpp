#pragma once

#include <vector>
#include <memory>

namespace ga {

template<class Individual>
class SelectionStrategy
{
public:
	using IndividualPtr = std::unique_ptr <Individual>;

	virtual const Individual& selectParent(const std::vector<IndividualPtr>& population) const = 0;
};

} // namespace ga
