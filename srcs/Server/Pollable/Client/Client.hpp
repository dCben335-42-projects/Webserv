#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Pollable.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"
#include <ctime>

#define ERROR_MSG_RESPONSE_SENDING_FAILED "An error occured while sending response to the client"
#define ERROR_MSG_UNKNOWN "An Unknown error occured"
#define LOG_MSG_CLIENT_PREFIX "Client - "
#define LOG_MSG_REPONSE_SENT LOG_MSG_CLIENT_PREFIX "Response sent"
#define LOG_MSG_REPONSE_READY LOG_MSG_CLIENT_PREFIX "Response ready"
#define LOG_MSG_CONNECTION_TERMINATED LOG_MSG_CLIENT_PREFIX "Connection terminated by a peer"
#define LOG_MSG_CLIENT_CONNECTED LOG_MSG_CLIENT_PREFIX "Client connected"
#define LOG_MSG_CLIENT_DISCONNECTED LOG_MSG_CLIENT_PREFIX "Client disconnected"

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
		const Config *m_config;
		bool		  m_isConfigSet;

		VirtualServer &m_server;

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

		bool prepareResponse();

		const Config *findHostConfig(const std::string &host) const;
		bool		  hasReceivedCompleteRequest();


	public:
		Client(int fd, const Config &config, VirtualServer &server);
		~Client();

		const HttpRequest &getRequest() const;
		HttpResponse	  &getResponse();

		void setEvents(const short events);

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();

		bool isTimedOut(const time_t currentTime) const;
		void onTimeOut();

		void onHttpException(const AHttpException &e);
		bool determineKeepAlive(unsigned short statusCode);
};


#endif