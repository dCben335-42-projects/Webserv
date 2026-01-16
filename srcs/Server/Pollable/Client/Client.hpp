#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Pollable.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>

class VirtualServer;

class Client : public Pollable
{
	private:
		VirtualServer *m_server;
		HttpRequest	   m_req;
		HttpResponse   m_res;

		void handleGetRequest();
		void handlePostRequest();
		void handleDeleteRequest();

		void prepareResponse();
		void test(const std::string &root);

		bool isCgi();
		bool isRequestComplete();

	public:
		Client(int fd, VirtualServer *server);
		~Client();

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();

		void setEvents(short events)
		{
			m_events = events;
		}

		HttpRequest &getRequest()
		{
			return m_req;
		}
		HttpResponse &getResponse()
		{
			return m_res;
		}

		void spawnCgi(const std::string &script_path);
};


#endif