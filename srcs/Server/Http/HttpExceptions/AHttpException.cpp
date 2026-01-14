#include "AHttpException.hpp"

const std::string AHttpException::HEAD_TEMPLATE =
	"<!DOCTYPE html>\n"
	"<html>\n"
	"<head>\n"
	"<title>{{TITLE}}</title>\n"
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
	"</style>\n"
	"</head>\n"
	"<body>\n";

const std::string AHttpException::HTML_TEMPLATE =
	"<div class='container'>\n"
	"  <header>\n"
	"    <h1>{{TITLE}}</h1>\n"
	"    <div class='code'>{{CODE}}</div>\n"
	"  </header>\n"
	"  <div class='content'>\n"
	"    <hr>\n"
	"    <div class='message'>\n"
	"      <p>The server encountered an error processing your request.</p>\n"
	"      <p><strong>Status:</strong> HTTP {{STATUS}}</p>\n"
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
	m_statusCode(code)
{
}

int AHttpException::getStatusCode() const
{
	return m_statusCode;
}

std::string AHttpException::getPage() const
{
	const std::string title = String::htmlEscape(what());
	const std::string code	= Number::toString(m_statusCode);
	std::string		  page	= HTML_TEMPLATE;
	std::string		  head	= HEAD_TEMPLATE;
	size_t			  pos;

	pos = page.find("{{TITLE}}");
	page.replace(pos, 9, title);
	pos = page.find("{{CODE}}");
	page.replace(pos, 8, code);
	pos = page.find("{{STATUS}}");
	page.replace(pos, 10, code + " " + title);

	pos = head.find("{{TITLE}}");
	head.replace(pos, 9, title);

	return head + page;
}
