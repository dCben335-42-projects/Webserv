#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Pollable.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"
#include <ctime>

#define INVALID_PORT_HOST_HEADER "Invalid port in Host header"
#define INVALID_IPV6_HOST_HEADER "Invalid IPv6 address in Host header"
#define HTTPS_HOST_HEADER "HTTPS URL in Host header is not allowed"

class VirtualServer;
class AHttpException;

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
		VirtualServer &m_server;

		const Config *m_config;
		bool		  m_isConfigSet;

		std::string m_accessLog;
		std::string m_errorLog;

		HttpRequest	 m_req;
		HttpResponse m_res;

		time_t		 m_time;
		TimeoutState m_timeoutState;

		bool   m_shouldClose;
		size_t m_count;

		static const Map<HttpMethod, void (Client::*)(const std::string &ressourcePath)> initMethodHandler();
		static const Map<HttpMethod, void (Client::*)(const std::string &ressourcePath)> m_methodHandler;

		static const Set<HttpMethod> initHandledMethods();
		static const Set<HttpMethod> handledMethods;

		void reset();

		void serveFile(const std::string &filePath, const std::string &target, const bool matchAccept);
		void serveIndexOf(const std::string &ressourcePath, const std::string &target);
		bool findIndexFile(std::string &ressourcePath, const std::string &target);

		void serveReturn();

		void handleGet(const std::string &ressourcePath);
		void handlePost(const std::string &ressourcePath);
		void handleDelete(const std::string &ressourcePath);
		void handlePutPatch(const std::string &ressourcePath);

		bool hasPostAuth();

		bool prepareResponse();
		bool serveCustomErrorPage(const int statusCode);

		const Config *findHostConfig(const std::string &host) const;
		bool		  hasReceivedCompleteRequest();


	public:
		Client(int fd, const Config &config, VirtualServer &server);
		~Client();

		const HttpRequest &getRequest() const;
		HttpResponse	  &getResponse();
		const Config	  &getConfig() const;
		time_t			   getTime() const;

		void setEvents(const short events);

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();

		bool isTimedOut(const time_t currentTime) const;
		void onTimeOut();

		void onHttpException(const AHttpException &e);
		void buildResponse();
		bool determineKeepAlive(unsigned short statusCode);
};


#endif