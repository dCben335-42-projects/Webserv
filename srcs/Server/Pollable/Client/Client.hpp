#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Pollable.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"

class VirtualServer;

class Client : public Pollable
{
	private:
		const Config &m_serverConfig;
		HttpRequest	  m_req;
		HttpResponse  m_res;

		void handleGetRequest(std::string ressourcePath);
		void handlePostRequest();
		void handleDeleteRequest(const std::string &ressourcePath);


		bool prepareResponse();

		bool isRequestComplete();

	public:
		Client(int fd, const Config &config);
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