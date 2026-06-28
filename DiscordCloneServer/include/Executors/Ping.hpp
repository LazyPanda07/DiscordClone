#pragma once

#include <Executors/StatelessExecutor.hpp>

namespace executors
{
	class Ping : public framework::StatelessExecutor
	{
	public:
		void doGet(framework::HttpRequest& request, framework::HttpResponse& response) override;
	};
}
