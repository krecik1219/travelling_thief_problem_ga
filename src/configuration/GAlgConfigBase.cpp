#include "GAlgConfigBase.hpp"

namespace config {

GAlgConfigBase::GAlgConfigBase(GAlgConfig && gAlgConfig)
	: gAlgConfig(std::move(gAlgConfig))
{
}

const GAlgConfig & GAlgConfigBase::getConfig() const
{
	return gAlgConfig;
}

} // namespace config
