#include "TtpConfigBase.hpp"

namespace ttp {

TtpConfigBase::TtpConfigBase(TtpConfig && ttpConfig)
	: ttpConfig(std::move(ttpConfig))
{
}

const TtpConfig& ttp::TtpConfigBase::getConfig() const
{
	return ttpConfig;
}

} // namespace ttp
