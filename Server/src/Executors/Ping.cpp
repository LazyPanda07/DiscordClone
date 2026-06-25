#include "Executors/Ping.hpp"

namespace executors
{
	void Ping::doGet(framework::HttpRequest& request, framework::HttpResponse& response)
	{
		response.setBody(request.getBody());
	}

	DEFINE_EXECUTOR(Ping)
}
