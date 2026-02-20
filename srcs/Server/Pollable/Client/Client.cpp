#include "Client.hpp"
#include "httpExceptions.hpp"
#include "CgiSpawner.hpp"
#include "httpMimes.hpp"
#include "FileManager.hpp"
#include "FileUpload.hpp"
#include "Date.hpp"
#include "IndexOf.hpp"
#include "Logger.hpp"
#include "EventLoop.hpp"
#include "webserv.hpp"
#include "HttpAuth.hpp"
#include "httpHeaders.hpp"
#include "Logger.hpp"

Client::Client(int fd, const Config &config, VirtualServer &server) :
	Pollable(fd),
	m_config(&config),
	m_server(server)
{
	m_count = 0;
	reset();
	EventLoop::addHandler(this);
}

Client::~Client()
{
	if (m_fd != -1)
		close(m_fd);
}


const Map<HttpMethod, void (Client::*)(const std::string &ressourcePath)> Client::m_methodHandler =
	Client::initMethodHandler();

/**
 *
 * @brief Initializes the method handler map.
 *
 * This function creates a mapping between HTTP methods and their corresponding handler functions in the Client class.
 * It associates each HTTP method (e.g., GET, POST, DELETE, PUT, PATCH) with the appropriate member function that handles requests of that method type.
 *
 * @return A map where the keys are HTTP methods and the values are pointers to member functions of the Client class that handle those methods.
 *
 */
const Map<HttpMethod, void (Client::*)(const std::string &ressourcePath)> Client::initMethodHandler()
{
	Map<HttpMethod, void (Client::*)(const std::string &ressourcePath)> methodHandler;
	methodHandler.set(HttpMethod::GET, &Client::handleGet);
	methodHandler.set(HttpMethod::POST, &Client::handlePost);
	methodHandler.set(HttpMethod::DELETE, &Client::handleDelete);
	methodHandler.set(HttpMethod::PUT, &Client::handlePutPatch);
	methodHandler.set(HttpMethod::PATCH, &Client::handlePutPatch);
	return methodHandler;
}

const Set<HttpMethod> Client::handledMethods = Client::initHandledMethods();

/**
 *
 * @brief Initializes the set of handled HTTP methods.
 *
 * This function iterates through the method handler map
 * and collects the HTTP methods that have associated handlers.
 *
 * @return A set of HTTP methods that the client can handle.
 *
 */
const Set<HttpMethod> Client::initHandledMethods()
{
	Set<HttpMethod> handledMethods;

	Map<HttpMethod, void (Client::*)(const std::string &ressourcePath)>::const_iterator end = Client::m_methodHandler.end();
	Map<HttpMethod, void (Client::*)(const std::string &ressourcePath)>::const_iterator it	= Client::m_methodHandler.begin();
	while (it != end)
	{
		handledMethods.push(it->first);
		++it;
	}
	return handledMethods;
}

/**
 *
 * @brief Resets the client's state.
 *
 * This function clears the HTTP request and response objects,
 * resets the timeout state, and prepares the client for a new request.
 *
 * It is typically called after a response has been fully sent
 * and the client is ready to handle a new request.
 *
 */
void Client::reset()
{
	m_req.reset();
	m_res.reset();
	m_timeoutState = KEEP_ALIVE_TIMEOUT;
	m_shouldClose  = false;
	m_events	   = POLLIN;
	m_time		   = std::time(0);
}

/**
 *
 * @brief Gets the HTTP request associated with the client.
 *
 * @note It is used to access the request details and headers.
 *
 * @return A constant reference to the HttpRequest object.
 *
 */
const HttpRequest &Client::getRequest() const
{
	return m_req;
}

/**
 *
 * @brief Gets the HTTP response associated with the client.
 *
 * @note You can modify the response using the returned reference.
 * It is useful for preparing and sending responses to the client
 * from the CgiPipe or other components.
 *
 * @return A non-constant reference to the HttpResponse object.
 *
 */
HttpResponse &Client::getResponse()
{
	return m_res;
}

/**
 *
 * @brief Sets the events for the client.
 *
 * @note This method updates the event flags
 * used for polling the client socket.
 *
 * @see Pollable::setEvents(short events)
 *
 * @param events The events to be set.
 * (e.g., POLLIN, POLLOUT)
 *
 */
void Client::setEvents(const short events)
{
	m_events = events;
}

/**
 *
 * @brief Handles HTTP POST requests.
 *
 * This function processes the POST request,
 * retrieves the submitted data,
 * and prepares the HTTP response with the appropriate headers and body.
 *
 * @throws AHttpException if there are issues with processing the POST data.
 *
 */
void Client::handlePost(const std::string &ressourcePath)
{
	(void) ressourcePath;
	const std::string &target = m_req.getTarget();
	if (target == "/auth/register" || target == "/auth/login")
	{
		HttpAuth auth;
		if (target == "/auth/register")
		{
			auth.parse(m_req.getBody(), HttpAuth::REGISTER);
			const Map<std::string> &urlEncoded = auth.getUrlEncoded();
			m_res.setBody("Register Successful !");
			m_res.setHeader(HEADER_SET_COOKIE, m_server.createCookie(urlEncoded.get(AUTH_REGISTER_NAME_KEY), urlEncoded.get(AUTH_REGISTER_PSWD_KEY)));
			m_res.setStatusCode(201);
		}
		else if (target == "/auth/login")
		{
			auth.parse(m_req.getBody(), HttpAuth::LOGIN);
			const Map<std::string> &urlEncoded = auth.getUrlEncoded();
			m_res.setBody("Login Successful !");
			m_res.setHeader(HEADER_SET_COOKIE, m_server.findCookie(urlEncoded.get(AUTH_LOGIN_NAME_KEY), urlEncoded.get(AUTH_LOGIN_PSWD_KEY)));
			m_res.setStatusCode(200);
		}
		return;
	}
	FileUpload file(m_req);
	if (!m_req.hasUpload())
		throw Http403Exception();
	if (!file.isUploadValid())
		throw Http400Exception("Invalid file upload data");
	m_req.setBody("File uploaded successfully.");
	m_res.setHeader(HEADER_CONTENT_TYPE, MIME_PLAIN);
	m_res.setStatusCode(201);
}


/**
 *
 * @brief Handles HTTP PUT and PATCH requests.
 *
 * This function processes the PUT or PATCH request,
 * checks for the existence and permissions of the target resource,
 * and updates the file with the provided data.
 *
 * @param ressourcePath The path to the resource being updated.
 *
 * @throws AHttpException if there are issues with accessing or modifying the resource.
 *
 */
void Client::handlePutPatch(const std::string &ressourcePath)
{
	const char *fullPath = ressourcePath.c_str();
	if (!FileManager::exists(fullPath))
		throw Http404Exception();
	if (!FileManager::hasRWPerm(fullPath))
		throw Http403Exception();
	FileUpload file(m_req);
	file.createDiskFile(ressourcePath, m_req.getBody());
	m_res.setStatusCode(204);
	m_res.setBody("File edited with a lot of success");
	m_res.setHeader(HEADER_CONTENT_TYPE, MIME_PLAIN);
}



/**
 *
 * @brief Handles HTTP DELETE requests.
 *
 * This function processes the DELETE request,
 * checks for the existence and permissions of the target resource,
 * and deletes the file if allowed.
 *
 * @param ressourcePath The path to the resource being deleted.
 *
 * @throws AHttpException if there are issues with accessing or deleting the resource.
 *
 */
void Client::handleDelete(const std::string &ressourcePath)
{
	const char *fullPath = ressourcePath.c_str();
	if (!FileManager::exists(fullPath))
		throw Http404Exception();
	if (FileManager::isFolder(fullPath))
		throw Http403Exception();
	if (!FileManager::hasRWPerm(fullPath))
		throw Http403Exception();
	std::remove(fullPath);
	m_res.setStatusCode(204);
	m_res.setBody("File deleted successfully.");
	m_res.setHeader(HEADER_CONTENT_TYPE, MIME_PLAIN);
}


/**
 * @brief Serves a file to the client.
 *
 * This function reads the specified file, checks if it is a directory, validates the MIME type against the client's Accept header,
 * and prepares the HTTP response with the file content and appropriate headers.
 *
 * @param filePath The path to the file being served.
 * @param target The target URL requested by the client.
 * @param matchAccept A flag indicating whether to check the MIME type against the Accept header.
 *
 * @throws AHttpException if there are issues with accessing or reading the file,
 * or if the MIME type does not match the client's Accept header.
 *
 */
void Client::serveFile(
	const std::string &filePath,
	const std::string &target,
	const bool		   matchAccept)
{
	File file(filePath, target);
	if (file.getIsDirectory())
	{
		const size_t	  pos			   = String::rfindIC(filePath, target);
		const std::string redirectedTarget = filePath.substr(pos) + "/";
		throw Http301Exception(String::removeDuplicateSlashes(redirectedTarget));
	}
	const std::string &mime = file.getMime();
	if (matchAccept && !m_req.matchAccept(mime))
		throw Http406Exception();
	file.read();
	m_res.setBody(file.getContent());
	m_res.setHeader(HEADER_CONTENT_TYPE, mime);
	m_res.setHeader(HEADER_LAST_MODIFIED, file.getLastModified());
	m_res.setStatusCode(200);
}


void Client::serveIndexOf(const std::string &ressourcePath, const std::string &target)
{
	IndexOf index(ressourcePath, target, m_req.getAllowedMethods());
	index.scanDirectory().generate();
	m_res.setBody(index.getPage());
	m_res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
	m_res.setStatusCode(200);
}

/**
 *
 * @brief Finds an index file in the specified resource path.
 *
 * This function checks if the server configuration has defined index files for the target location.
 * If so, it iterates through the list of index files and checks if any of them exist in the resource path.
 *
 * If an index file is found, it updates the resource path to point to that file and returns true.
 * If no index file is found, it checks for a fallback index file and updates the resource path accordingly if it exists.
 *
 * @param ressourcePath The path to the resource being checked for index files. It may be modified to point to the found index file.
 * @param target The target URL requested by the client, used to determine the applicable index files from the server configuration.
 *
 * @return true if an index file was found and the resource path was updated, false otherwise.
 *
 */
bool Client::findIndexFile(std::string &ressourcePath, const std::string &target)
{
	if (m_config->hasIndexFiles(target))
	{
		const Vec<std::string> indexFiles = m_config->getIndexFiles(target);
		for (size_t i = 0; i < indexFiles.size(); ++i)
		{
			const std::string indexPath = String::removeDuplicateSlashes(ressourcePath + "/" + indexFiles[i]);
			if (FileManager::exists(indexPath))
			{
				ressourcePath = indexPath;
				return true;
			}
		}
	}
	const std::string fallbackPath = ressourcePath + "/" + FALLBACK_INDEX;
	if (FileManager::exists(fallbackPath))
	{
		ressourcePath = fallbackPath;
		return true;
	}
	return false;
}


/**
 *
 * @brief Handles HTTP GET requests.
 *
 * This function processes the GET request,
 * retrieves the requested resource,
 * and prepares the HTTP response with the appropriate headers and body.
 *
 * @throws AHttpException if the requested resource is not found
 * or if there are permission issues.
 *
 */
void Client::handleGet(const std::string &ressourcePath)
{

	bool			  isIndexFile = false;
	const std::string target	  = m_req.getTarget();

	if (target == "/cgi-bin/")
	{
		if (!m_req.hasCookieValue(COOKIE_KEY_TOKEN))
			throw Http401Exception();
		const std::string token = m_req.getCookieValue(COOKIE_KEY_TOKEN);
		if (!m_server.hasUser(token))
			throw Http401Exception();
	}
	if (target == "/auth/")
	{
		HttpAuth auth;
		auth.generatePage("/auth");
		m_res.setBody(auth.getPage());
		m_res.setStatusCode(200);
		m_res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
		return;
	}
	try
	{
		std::string path = ressourcePath;
		if (!FileManager::exists(ressourcePath))
			throw Http404Exception();
		if (FileManager::isFolder(ressourcePath))
		{
			const size_t targetLen		 = target.length();
			const bool	 isFolderRequest = targetLen && target[targetLen - 1] == '/';

			if (!isFolderRequest)
				throw Http301Exception(target + "/");
			isIndexFile = findIndexFile(path, target);
			if (!isIndexFile && m_req.getActiveAutoIndex())
				return serveIndexOf(ressourcePath, target);
			if (!isIndexFile)
				throw Http403Exception();
		}
		serveFile(path, target, !isIndexFile);
	}
	catch (const FileManager::FileException &e)
	{
		std::cerr << e.what() << '\n';
		throw e.getHttpException();
	}
}


void Client::serveReturn()
{
	const LocationReturn &locReturn	 = m_req.getReturn();
	const unsigned short  statusCode = locReturn.getStatusCode();
	m_res.setStatusCode(statusCode);
	if (m_req.isRedirectionStatus(statusCode))
		m_res.setHeader(HEADER_LOCATION, locReturn.getTarget());
	else
	{
		m_res.setBody(locReturn.getTarget());
		m_res.setHeader(HEADER_CONTENT_TYPE, MIME_PLAIN);
	}
}


/**
 *
 * @brief Prepares the HTTP response based on the client's request.
 *
 * This function determines the requested method (GET, POST, DELETE),
 * validates it, and calls the appropriate handler to process the request.
 *
 * It also checks if the request is for a CGI script
 * and spawns the CGI handler if necessary.
 *
 *
 * @throws AHttpException for various HTTP errors
 *
 * @return false if a CGI script is being handled, true otherwise.
 *
 */
bool Client::prepareResponse()
{
	if (m_req.hasReturn())
	{
		serveReturn();
		return true;
	}
	CgiSpawner cgiHandler(m_req, *m_config);
	if (!m_req.isAllowedMethod())
		throw Http403Exception();
	if (cgiHandler.isCgi())
	{
		cgiHandler.spawnCgi(this);
		m_events = 0;
		return false;
	}
	const HttpMethod &method = m_req.getMethod();
	if (!Client::m_methodHandler.has(method))
		throw Http405Exception(method.getStrType());

	const size_t	  segmentSize	= m_req.getSegmentSize();
	const std::string ressourcePath = m_req.buildResourcePath(segmentSize - 1);
	(this->*Client::m_methodHandler.get(method))(ressourcePath);
	return true;
}


/**
 *
 * @brief Finds the appropriate server configuration based on the client's request.
 *
 * This function checks the Host header in the client's request and compares it with the server configurations available in the VirtualServer.
 * If a matching server name is found, it returns that configuration. Otherwise, it returns the default configuration (the first one in the list).
 *
 * @return A pointer to the Config object that matches the client's request, or the default configuration if no match is found.
 *
 */
const Config *Client::findHostConfig() const
{
	const std::string		  &host	 = m_req.getHost();
	const Vec<const Config *> &confs = m_server.getConfigs();

	for (Vec<const Config *>::const_iterator it = confs.begin(); it != confs.end(); ++it)
	{
		const Config &config = *(*it);
		if (config.hasServerName() && config.getServerName() == host)
			return &config;
	}
	return confs.at(0);
}


/**
 *
 * @brief Handles HTTP exceptions and prepares the response accordingly.
 *
 * This function takes an AHttpException object, extracts the relevant information,
 * and prepares the HTTP response with the appropriate status code, headers, and body.
 *
 * It is used to centralize the handling of HTTP errors and ensure consistent responses
 * for different types of exceptions.
 *
 * @param e The AHttpException object containing details about the HTTP error.
 *
 */
void Client::onHttpException(const AHttpException &e)
{
	m_res.reset();
	m_res.setStatusCode(e.getStatusCode());
	if (e.isRedirection())
	{
		m_res.setHeader(HEADER_LOCATION, e.getRedirectionUrl());
		return;
	}
	m_res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
	m_res.setBody(e.getPage());
	if (e.getStatusCode() != 405)
		return;
	const Set<HttpMethod> *locMethods	  = m_req.getAllowedMethods();
	const Set<HttpMethod>  allowedMethods = locMethods ? *locMethods : Client::handledMethods;
	m_res.setHeader(HEADER_ALLOW, HttpMethod::toString(allowedMethods));
}


/**
 *
 * @brief Determines if the connection should be kept alive.
 *
 * This function checks the client's request headers and the response status code
 * to decide whether to keep the connection alive for further requests or to close it.
 *
 * @param statusCode The HTTP status code of the response.
 *
 * @note It stores the result in the m_shouldClose member variable to then be used in
 * the onPollOut() method to decide whether to close the connection after sending the response.
 *
 * @return true if the connection should be kept alive, false if it should be closed.
 *
 */
bool Client::determineKeepAlive(unsigned short statusCode)
{
	m_shouldClose = !m_req.getConnectionKeepAlive() && !m_res.isCloseStatus(statusCode);
	return !m_shouldClose;
}

/**
 *
 * @brief Handles reading data from the client socket.
 *
 * This function reads data from the client's socket,
 * checks if the complete HTTP request * has been received,
 * and prepares the response if so.
 * If the connection is closed by the client,
 * it updates the client state accordingly.
 *
 * @note This function is called when the client socket is ready for reading.
 *
 */
void Client::onPollIn()
{
	const ssize_t bytesRead = m_req.readOnce(m_fd);

	if (bytesRead == -1)
		return onPollErr();
	if (bytesRead == 0)
		return onPollHangUp();
	m_time						= std::time(0);
	const size_t maxBodySize	= m_config->getMaxBodySize();
	const size_t maxHeadersSize = m_config->getMaxHeadersSize();
	try
	{
		const bool hasCompleteRequest = m_req.isComplete(maxBodySize, maxHeadersSize);
		if (m_req.hasHost())
			m_config = findHostConfig();
		if (!hasCompleteRequest)
		{
			m_timeoutState = m_req.hasCompleteHeader() ? BODY_TIMEOUT : HEADER_TIMEOUT;
			return;
		}
		m_timeoutState = KEEP_ALIVE_TIMEOUT;
		m_req.parse(*m_config);
		if (!prepareResponse())
			return;
	}
	catch (const AHttpException &e)
	{
		onHttpException(e);
	}
	m_res.fill(!m_req.getConnectionKeepAlive(), m_config->getKeepAliveTimeout()).build();
	determineKeepAlive(m_res.getStatusCode());
	m_events = POLLOUT;
}




/**
 *
 * @brief Handles writing data to the client socket.
 *
 * This function sends the prepared HTTP response to the client.
 * If the entire response has been sent,
 * it updates the client state to CLOSED and resets the response object.
 *
 * @note This function is called when the client socket is ready for writing.
 *
 */
void Client::onPollOut()
{
	bool hasFullySent = false;
	try
	{
		hasFullySent = m_res.sendOnce(m_fd);
	}
	catch (const std::exception &e)
	{
		Logger::error("Error sending response to client on fd " + Logger::get(m_fd) + ": " + e.what());
		m_events		   = 0;
		m_connectionClosed = true;
		return;
	}
	if (hasFullySent)
	{
		if (m_shouldClose)
		{
			m_connectionClosed = true;
			m_events		   = 0;
			return;
		}
		reset();
		m_count++;
		return;
	}
	m_time	 = std::time(0);
	m_events = POLLOUT;
}


/**
 *
 * @brief Handles errors that occur during polling of the client socket.
 *
 * This function is called when an error event is detected on the client socket.
 * It logs the error details and updates the client state to indicate that the connection is closed.
 *
 * @note This function is called when a POLLERR event is detected on the client socket.
 *
 */
void Client::onPollErr()
{
	Logger::error("Poll error on client socket fd " + Logger::get(m_fd) + ", errno: " + Logger::get(errno) + ", strerror: " + strerror(errno));
	m_events		   = 0;
	m_connectionClosed = true;
}


/**
 *
 * @brief Handles the hang-up event on the client socket.
 *
 * This function is called when a POLLHUP event is detected on the client socket,
 * indicating that the client has disconnected. It logs the disconnection details
 * and updates the client state to indicate that the connection is closed.
 *
 * @note This function is called when a POLLHUP event is detected on the client socket.
 *
 */
void Client::onPollHangUp()
{

	Logger::info("client disconnected on fd " + Logger::get(m_fd) + ", errno: " + Logger::get(errno) + ", strerror: " + strerror(errno));
	m_events		   = 0;
	m_connectionClosed = true;
}



/**
 *
 * @brief Checks if the client connection has timed out.
 *
 * This function calculates the timeout based on the client's current timeout state
 * (keep-alive, header, or body) and the server's configured timeouts.
 * It compares the current time with the calculated timeout time to determine if the connection has timed out.
 *
 * @param currentTime The current time to compare against the timeout time.
 *
 * @return true if the connection has timed out, false otherwise.
 *
 */
bool Client::isTimedOut(const time_t currentTime) const
{
	time_t timeoutTime = m_time;

	if (m_timeoutState == KEEP_ALIVE_TIMEOUT)
		timeoutTime += m_config->getKeepAliveTimeout();
	if (m_timeoutState == HEADER_TIMEOUT)
		timeoutTime += m_config->getHeaderTimeout();
	if (m_timeoutState == BODY_TIMEOUT)
		timeoutTime += m_config->getBodyTimeout();
	return Date::getDiffTime(timeoutTime, currentTime) < 0;
}


/**
 *
 * @brief Handles the timeout event for the client connection.
 *
 * This function is called when the client's connection has timed out based on the current timeout state.
 * If the timeout occurred during the header or body reception, it prepares a 408 Request Timeout response.
 * If the timeout occurred during the keep-alive state, it simply closes the connection.
 *
 * @note This function is called when a timeout is detected for the client connection.
 *
 */
void Client::onTimeOut()
{
	if (m_timeoutState == HEADER_TIMEOUT || m_timeoutState == BODY_TIMEOUT)
	{
		m_res.reset();
		m_res.setStatusCode(408);
		m_res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
		m_res.setBody(Http408Exception().getPage());
		m_res.fill(true, 0).build();
		determineKeepAlive(m_res.getStatusCode());
		m_events = POLLOUT;
		return;
	}
	m_connectionClosed = true;
	m_events		   = 0;
}