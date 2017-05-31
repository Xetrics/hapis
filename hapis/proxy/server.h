#pragma once

/* Server component of proxy */

#define SERVER_PORT 5678
#define SERVER_MAX_CONNECTIONS 10

namespace Proxy {

	class Server {
	public:
		void* RakNetServer;
		Server();
	};

}