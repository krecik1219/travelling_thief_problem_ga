#pragma once

#include "TtpConfig.hpp"


namespace config {

class TtpConfigBase
{
public:
	explicit TtpConfigBase(TtpConfig&& ttpConfig);
	
	const config::TtpConfig& getConfig() const;
		
private:
	const config::TtpConfig ttpConfig;
};

} // namespace config
