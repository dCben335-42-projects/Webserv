#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Pollable.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"
#include <ctime>

class VirtualServer;

class Client : public Pollable
{
	private:
		const Config &m_serverConfig;
		HttpRequest	  m_req;
		HttpResponse  m_res;
		time_t		  m_keepAlive;
		bool		  m_idle;
		bool		  m_shouldClose;

		void handleGetRequest(std::string ressourcePath);
		void handlePostRequest();
		void handleDeleteRequest(const std::string &ressourcePath);
		void handlePutRequest(const std::string &ressourcePath);
		void handlePatchRequest(const std::string &ressourcePath);
		void checkFilePerms(const char *fullPath) const;
		bool prepareResponse();

	public:
		Client(int fd, const Config &config);
		~Client();

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();

		void setEvents(const short events);

		bool			   determineKeepAlive(unsigned short statusCode);
		const HttpRequest &getRequest() const;
		HttpResponse	  &getResponse();

		bool isTimedOut(const time_t currentTime) const;
};


#endif