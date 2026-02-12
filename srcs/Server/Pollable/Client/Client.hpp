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
		const Config &m_serverConfig;
		HttpRequest	  m_req;
		HttpResponse  m_res;
		time_t		  m_time;
		TimeoutState  m_timeoutState;
		bool		  m_shouldClose;
		size_t		  m_count;

		void reset();

		void serveFile(const std::string &filePath, const std::string &target, const bool matchAccept);
		void serveIndexOf(const std::string &ressourcePath, const std::string &target);
		bool findIndexFile(std::string &ressourcePath, const std::string &target);

		void handleGet(std::string ressourcePath);
		void handlePost();
		void handleDelete(const std::string &ressourcePath);
		void handlePutPatch(const std::string &ressourcePath);


		bool prepareResponse();

	public:
		Client(int fd, const Config &config);
		~Client();

		const HttpRequest &getRequest() const;
		HttpResponse	  &getResponse();

		void setEvents(const short events);

		bool determineKeepAlive(unsigned short statusCode);

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();

		bool isTimedOut(const time_t currentTime) const;
		void onTimeOut();
};


#endif