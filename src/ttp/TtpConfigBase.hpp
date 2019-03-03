#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "TtpConfig.hpp"


namespace ttp {

class TtpConfigBase
{
public:
	explicit TtpConfigBase(TtpConfig&& ttpConfig);
		
private:
	const TtpConfig ttpConfig;
};

} // namespace ttp
