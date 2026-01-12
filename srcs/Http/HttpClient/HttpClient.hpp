
#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <poll.h>

/**
 *
 * @enum ClientState
 *
 * @brief Represents the state of an HTTP client.
 *
 * READING: The client is currently reading data.
 * WRITING: The client is currently writing data.
 * CLOSED: The client connection is closed.
 *
 * @note This enum is used to manage the state of an HTTP client
 * during its lifecycle.
 *
 */
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
