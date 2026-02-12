#include "AHttpException.hpp"

const std::string AHttpException::HEAD_TEMPLATE =
	"<!DOCTYPE html>\n"
	"<html>\n"
	"<head>\n"
	"<title>{{ TITLE }}</title>\n"
	"<style>\n"
	"  body { font-family: 'Segoe UI', Tahoma, Geneva, sans-serif; "
	"         margin: 0; padding: 40px; background: #f4f4f4; color: #333; }\n"
	"  .container { max-width: 800px; margin: 0 auto; background: white; "
	"               border-radius: 8px; box-shadow: 0 4px 12px rgba(0,0,0,0.15); }\n"
	"  header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); "
	"           color: white; padding: 30px; text-align: center; }\n"
	"  h1 { margin: 0; font-size: 3em; font-weight: 300; "
	"       text-shadow: 0 2px 4px rgba(0,0,0,0.3); }\n"
	"  .code { background: rgba(255,255,255,0.2); padding: 10px 20px; "
	"          border-radius: 6px; font-family: 'Courier New', monospace; "
	"          font-size: 4em; font-weight: bold; margin: 10px 0; }\n"
	"  .content { padding: 40px; }\n"
	"  hr { border: none; height: 2px; background: linear-gradient(to right, "
	"        transparent, #ddd, transparent); margin: 30px 0; }\n"
	"  .message { background: #fff3cd; border: 1px solid #ffeaa7; "
	"             border-radius: 6px; padding: 20px; margin: 20px 0; }\n"
	"  .links { text-align: center; margin-top: 30px; }\n"
	"  .links a { display: inline-block; margin: 0 15px; padding: 12px 24px; "
	"             background: #667eea; color: white; text-decoration: none; "
	"             border-radius: 6px; transition: all 0.3s; }\n"
	"  .links a:hover { background: #5a67d8; transform: translateY(-2px); }\n"
	"  .hidden { display: none; }\n"
	"</style>\n"
	"</head>\n"
	"<body>\n";

const std::string AHttpException::HTML_TEMPLATE =
	"<div class='container'>\n"
	"  <header>\n"
	"    <h1>{{ TITLE }}</h1>\n"
	"    <div class='code'>{{ CODE }}</div>\n"
	"  </header>\n"
	"  <div class='content'>\n"
	"    <hr>\n"
	"    <div class='message'>\n"
	"      <p>The server encountered an error processing your request.</p>\n"
	"      <p><strong>Status:</strong> HTTP {{ STATUS }}</p>\n"
	"      <p class='{{ IS_HIDDEN }}'><strong>Details:</strong> {{ EXTRA_MESSAGE }}</p>\n"
	"    </div>\n"
	"    <div class='links'>\n"
	"      <a href='javascript:history.back()'>Go Back</a>\n"
	"      <a href='/'>Return Home</a>\n"
	"    </div>\n"
	"  </div>\n"
	"</div>\n"
	"</body>\n"
	"</html>\n";


AHttpException::AHttpException(const std::string &message, int code) :
	std::runtime_error(message),
	m_statusCode(code),
	m_extraMessage(),
	m_isRedirection(false)
{
}


AHttpException::~AHttpException() throw()
{
}



/**
 *
 * @brief Retrieves the HTTP status code associated with the exception.
 *
 * This function returns the HTTP status code that corresponds to the specific error represented by the exception.
 * The status code is set during the construction of the exception and can be used to determine the type of error
 * that occurred (e.g., 404 for Not Found, 500 for Internal Server Error).
 *
 * @return An integer representing the HTTP status code of the error.
 *
 */
int AHttpException::getStatusCode() const
{
	return m_statusCode;
}


/**
 *
 * @brief Retrieves any additional message associated with the exception.
 *
 * This function returns an extra message that provides more details about the error. This message can be set
 * when constructing the exception or through other means, and it is intended to give more context to the user
 * about what went wrong. If no extra message is provided, this function will return an empty string.
 *
 * @return A constant reference to a string containing the extra message associated
 * with the exception, or an empty string if no extra message is set.
 *
 */
const std::string &AHttpException::getExtraMessage() const
{
	return m_extraMessage;
}


/**
 *
 * @brief Checks if the exception represents a redirection error.
 *
 * This function returns a boolean value indicating whether the exception corresponds
 * to an HTTP redirection status code (3xx).
 * If the exception is a redirection, it means that the client should be redirected
 * to a different URL, which can be obtained using the getRedirectionUrl() method.
 *
 * @return true if the exception represents a redirection error, false otherwise.
 *
 */
bool AHttpException::isRedirection() const
{
	return m_isRedirection;
}


/**
 *
 * @brief Retrieves the URL for redirection if the exception represents a redirection error.
 *
 * This function returns the URL to which the client should be redirected
 * if the exception corresponds to an HTTP redirection status code (3xx).
 * If the exception is not a redirection, this function will return an empty string.
 *
 * @return A constant reference to a string containing the redirection URL
 * if the exception is a redirection error, or an empty string otherwise.
 *
 */
const std::string &AHttpException::getRedirectionUrl() const
{
	return m_redirectionUrl;
}

/**
 *
 * @brief Generates an HTML page representing the HTTP error.
 * This function constructs an HTML page using predefined templates and the error information
 * contained in the exception.
 * It replaces placeholders in the templates with the actual error message, status code,
 * and any additional information provided.
 * The resulting HTML page is designed to be visually appealing and informative for the user.
 *
 * @note If no extra message is provided, the details section will be hidden in the generated HTML page.
 *
 * @return A string containing the complete HTML page to be sent as the response body for the HTTP error.
 *
 */
const std::string AHttpException::getPage() const
{
	const std::string title	 = String::htmlEscape(what());
	const std::string code	 = Number::toString(m_statusCode);
	std::string		  result = HTML_TEMPLATE + HEAD_TEMPLATE;

	String::replaceAll(result, "{{ TITLE }}", title);
	String::replaceAll(result, "{{ CODE }}", code);
	String::replaceAll(result, "{{ STATUS }}", code + " " + title);
	if (!m_extraMessage.empty())
		String::replaceAll(result, "{{ EXTRA_MESSAGE }}", String::htmlEscape(m_extraMessage));
	String::replaceAll(result, "{{ IS_HIDDEN }}", m_extraMessage.empty() ? "hidden" : "");
	return result;
}
