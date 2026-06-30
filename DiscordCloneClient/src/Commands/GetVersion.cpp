#include "Commands/GetVersion.hpp"

#include <iostream>

#include <c_api.h>

#include "Utils.hpp"

constexpr std::string_view commandName = "get_version";

namespace commands
{
	bool GetVersion::run(std::istream& stream)
	{
		std::cout << utils::callApiFunction(&::getVersion) << std::endl;

		return true;
	}

	uint32_t GetVersion::getChecks() const
	{
		return NULL;
	}

	GetVersion::GetVersion(const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks)
	{

	}
}
