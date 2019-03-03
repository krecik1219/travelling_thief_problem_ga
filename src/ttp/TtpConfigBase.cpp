#include "TtpConfigBase.hpp"

ttp::TtpConfigBase::TtpConfigBase(TtpConfig && ttpConfig)
	: ttpConfig(std::move(ttpConfig))
{
}
