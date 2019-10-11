#include "TtpConfigBase.hpp"

namespace config {

TtpConfigBase::TtpConfigBase(TtpConfig && ttpConfig)
	: ttpConfig(std::move(ttpConfig))
{
}

const config::TtpConfig& TtpConfigBase::getConfig() const
{
	return ttpConfig;
}

} // namespace config
