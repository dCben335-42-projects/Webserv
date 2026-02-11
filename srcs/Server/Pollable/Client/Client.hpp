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
	public:
		enum TimeoutState
		{
			KEEP_ALIVE_TIMEOUT,
			HEADER_TIMEOUT,
			BODY_TIMEOUT
		};

	private:
		const Config  &m_serverConfig;
		VirtualServer &m_server;
		HttpRequest	   m_req;
		HttpResponse   m_res;
		time_t		   m_time;
		TimeoutState   m_timeoutState;
		bool		   m_shouldClose;
		size_t		   m_count;

		void reset();

		void handleGetRequest(std::string ressourcePath);
		void handlePostRequest();
		void handleDeleteRequest(const std::string &ressourcePath);
		void handlePutRequest(const std::string &ressourcePath);
		void handlePatchRequest(const std::string &ressourcePath);
		void checkFilePerms(const char *fullPath) const;
		bool prepareResponse();

		bool hasRequestTimeout(const time_t &currentTime) const;

	public:
		Client(int fd, const Config &config, VirtualServer &server);
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
		void onTimeOut();
};


#endif