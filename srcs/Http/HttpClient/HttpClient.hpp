
#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <poll.h>

enum ClientState
{
	READING,
	WRITING,
	CLOSED
};

class HttpClient
{
	private:
		HttpRequest	 m_req;
		HttpResponse m_res;
		int			 m_fd;
		ClientState	 m_state;

		bool shouldClose() const;

		void prepareResponse();

	public:
		HttpClient(int fd);
		~HttpClient();

		int			getFd() const;
		ClientState getState() const;
		void		setState(ClientState state);

		void onRead();
		void onWrite(pollfd &pollFd);
};


#endif
