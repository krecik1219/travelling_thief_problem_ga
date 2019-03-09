#pragma once

#include "GAlgConfig.hpp"


namespace config {

class GAlgConfigBase
{
public:
	explicit GAlgConfigBase(GAlgConfig&& gAlgConfig);

	const GAlgConfig& getConfig() const;

private:
	const GAlgConfig gAlgConfig;
};

} // namespace config
