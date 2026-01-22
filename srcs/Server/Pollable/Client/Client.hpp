#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Pollable.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>
#include "Config.hpp"

class VirtualServer;

class Client : public Pollable
{
	private:
		VirtualServer *m_server;
		const Config  &m_serverConfig;
		HttpRequest	   m_req;
		HttpResponse   m_res;

		void handleGetRequest(const std::string &ressourcePath);
		void handlePostRequest();
		void handleDeleteRequest();

		void prepareResponse();

		bool isRequestComplete();

	public:
		Client(int fd, VirtualServer *server, const Config &config);
		~Client();

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();

		void setEvents(const short events);

		const HttpRequest &getRequest() const;
		HttpResponse	  &getResponse();
};


#endif