#include "HttpAuth.hpp"
#include "String.hpp"
#include "httpExceptions.hpp"
#include "Logger.hpp"
#include "webserv.hpp"
#include "String.hpp"

const std::string HttpAuth::m_templatePage =
	"<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
	"    <meta charset=\"UTF-8\">\n"
	"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
	"    <title>Auth Page</title>\n"
	"\n"
	"    <style>\n"
	"        body {\n"
	"            font-family: system-ui, sans-serif;\n"
	"            margin: 2rem;\n"
	"            background: #f7f9fb;\n"
	"            color: #333;\n"
	"            min-height: 100vh;\n"
	"            display: flex;\n"
	"            justify-content: center;\n"
	"            gap: 2.5rem;\n"
	"            flex-wrap: wrap;\n"
	"        }\n"
	"        h1 {\n"
	"            margin: 0 0 1.5rem 0;\n"
	"            font-size: 2rem;\n"
	"            font-weight: 400;\n"
	"            color: #222;\n"
	"            text-align: center;\n"
	"            text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);\n"
	"        }\n"
	"        .container {\n"
	"            max-width: 600px;\n"
	"            width: 100%;\n"
	"            margin: auto 0;\n"
	"            background: white;\n"
	"            border-radius: 8px;\n"
	"            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);\n"
	"            padding: 40px;\n"
	"        }\n"
	"        .fields {\n"
	"            display: grid;\n"
	"            gap: 1rem;\n"
	"        }\n"
	"        form {\n"
	"            display: grid;\n"
	"            gap: 0.5rem;\n"
	"            width: 85%;\n"
	"            margin: 0 auto;\n"
	"        }\n"
	"        label {\n"
	"            display: grid;\n"
	"        }\n"
	"        button {\n"
	"            padding: 0.35rem 0.6rem;\n"
	"            font-size: 0.85rem;\n"
	"            border-radius: 4px;\n"
	"            border: none;\n"
	"            cursor: pointer;\n"
	"            transition: background 0.2s ease, filter 0.2s ease-in-out;\n"
	"        }\n"
	"        input {\n"
	"            width: 100%;\n"
	"            box-sizing: border-box;\n"
	"            padding: 12px;\n"
	"            border-radius: 4px;\n"
	"            border: 1px solid #ddd;\n"
	"            cursor: pointer;\n"
	"        }\n"
	"        .inputWrapper {\n"
	"            position: relative;\n"
	"        }\n"
	"        .inputWrapper button {\n"
	"            position: absolute;\n"
	"            top: 0;\n"
	"            right: 0;\n"
	"            bottom: 0;\n"
	"        }\n"
	"        small {\n"
	"            color: red;\n"
	"            margin: 0 0 0 5px;\n"
	"        }\n"
	"    </style>\n"
	"\n"
	"    <script>\n"
	"        document.addEventListener(\"DOMContentLoaded\", () => {\n"
	"            const eyeButtons = document.querySelectorAll(\".inputWrapper button\")\n"
	"            const inputPasswords = document.querySelectorAll(\".inputWrapper input[type=password]\")\n"
	"            eyeButtons.forEach((button, i) => {\n"
	"                button.addEventListener(\"click\", () => {\n"
	"                    const oldType = inputPasswords[i].getAttribute(\"type\")\n"
	"                    inputPasswords[i].setAttribute(\"type\", oldType === \"text\" ? \"password\" : \"text\")\n"
	"                })\n"
	"            })\n"
	"            const inputs = document.querySelectorAll(\"input\")\n"
	"            const url = new URLSearchParams(window.location.search)\n"
	"            inputs.forEach((input, i) => {\n"
	"                if (url.has(input.name)) {\n"
	"                    const errorMsg = document.querySelector(`small[data-name=${input.name}]`)\n"
	"                    errorMsg.textContent = url.get(input.name)\n"
	"                }\n"
	"            })\n"
	"			const registerForm = document.querySelector(\" .rform \")\n"
	"			const loginForm = document.querySelector(\" .lform \")\n"
	"			registerForm.addEventListener(\"submit\", (e) => handleSubmit(e, registerForm, \"" AUTH_TEMPLATE_AUTHLOC "/" AUTH_REGISTER_LOC "\"));\n"
	"			loginForm.addEventListener(\"submit\", (e) => handleSubmit(e, loginForm, \"" AUTH_TEMPLATE_AUTHLOC "/" AUTH_LOGIN_LOC "\"))\n"
	"			function handleSubmit(e,form,url) {\n"
	"				e.preventDefault()\n"
	"				const bodyData = new URLSearchParams(new FormData(form))\n"
	"				fetch(url, { method: 'POST', body: bodyData })\n"
	"					.then(r => {\n"
	"						if (r.status === 201 || r.status === 200) {\n"
	"							window.location = '/'\n"
	"						}\n"
	"						if (r.redirected) {\n"
	"							console.log(r)\n"
	"							window.location = r.url"
	"						}\n"
	"				})\n"
	"			}\n"
	"        })\n"
	"    </script>\n"
	"</head>\n"
	"\n"
	"<body>\n"
	"    <section class=\"container\">\n"
	"        <h1>Register Form</h1>\n"
	"        <form class=\"rform\" method=\"post\" action=\"" AUTH_TEMPLATE_AUTHLOC "/" AUTH_REGISTER_LOC "\">\n"
	"            <div class=\"fields\">\n"
	"                <div class=\"field\">\n"
	"                    <label>\n"
	"                        <b>New Username</b>\n"
	"                        <div class=\"inputWrapper\">\n"
	"                            <input type=\"text\" placeholder=\"Enter your Username\" name=\"" AUTH_REGISTER_NAME_KEY "\">\n"
	"                        </div>\n"
	"                    </label>\n"
	"                    <small data-name=\"" AUTH_REGISTER_NAME_KEY "\"></small>\n"
	"                </div>\n"
	"                <div class=\"field\">\n"
	"                    <label>\n"
	"                        <b>New Password</b>\n"
	"                        <div class=\"inputWrapper\">\n"
	"                            <input type=\"password\" placeholder=\"Enter your Password\" name=\"" AUTH_REGISTER_PSWD_KEY "\">\n"
	"                            <button type=\"button\">👁️</button>\n"
	"                        </div>\n"
	"                    </label>\n"
	"                    <small data-name=\"" AUTH_REGISTER_PSWD_KEY "\"></small>\n"
	"                </div>\n"
	"                <div class=\"field\">\n"
	"                    <label>\n"
	"                        <b>Confirm Password</b>\n"
	"                        <div class=\"inputWrapper\">\n"
	"                            <input type=\"password\" placeholder=\"Confirm your Password\" name=\"" AUTH_REGISTER_CONFIRM_KEY "\">\n"
	"                            <button type=\"button\">👁️</button>\n"
	"                        </div>\n"
	"                    </label>\n"
	"                    <small data-name=\"" AUTH_REGISTER_CONFIRM_KEY "\"></small>\n"
	"                </div>\n"
	"            </div>\n"
	"            <button type=\"submit\">Register</button>\n"
	"        </form>\n"
	"    </section>\n"
	"    <section class=\"container\">\n"
	"        <h1>Login Form</h1>\n"
	"        <form class=\"lform\" method=\"post\" action=\"" AUTH_TEMPLATE_AUTHLOC "/" AUTH_LOGIN_LOC "\">\n"
	"            <div class=\"fields\">\n"
	"                <div class=\"field\">\n"
	"                    <label>\n"
	"                        <b>Username</b>\n"
	"                        <div class=\"inputWrapper\">\n"
	"                            <input type=\"text\" placeholder=\"Enter your Username\" name=\"" AUTH_LOGIN_NAME_KEY "\">\n"
	"                        </div>\n"
	"                    </label>\n"
	"                    <small data-name=\"" AUTH_LOGIN_NAME_KEY "\"></small>\n"
	"                </div>\n"
	"                <div class=\"field\">\n"
	"                    <label>\n"
	"                        <b>Password</b>\n"
	"                        <div class=\"inputWrapper\">\n"
	"                            <input type=\"password\" placeholder=\"Enter your Password\" name=\"" AUTH_LOGIN_PSWD_KEY "\">\n"
	"                            <button type=\"button\">👁️</button>\n"
	"                        </div>\n"
	"                    </label>\n"
	"                    <small data-name=\"" AUTH_LOGIN_PSWD_KEY "\"></small>\n"
	"                </div>\n"
	"            </div>\n"
	"            <button type=\"submit\">Login</button>\n"
	"        </form>\n"
	"    </section>\n"
	"</body>\n"
	"</html>\n";


HttpAuth::HttpAuth(const std::string authLoc) : m_page(""), m_authLoc(authLoc)
{
}

HttpAuth::~HttpAuth()
{
}

void HttpAuth::generatePage()
{
	m_page = m_templatePage;

	String::replaceAll(m_page, AUTH_TEMPLATE_AUTHLOC, m_authLoc);
}

const std::string &HttpAuth::getPage() const
{
	return m_page;
}

void HttpAuth::parse(const std::string &body, AuthMode mode)
{
	Vec<std::string> bodySplit = String::split(body, FORM_FIELD_SEP);

	for (Vec<std::string>::iterator it = bodySplit.begin(); it != bodySplit.end(); it++)
	{
		Vec<std::string> fieldSplit = String::split((*it), COOKIE_VALUE_SEP);
		if (fieldSplit.size() != 2)
		{
			if (fieldSplit.size() == 0)
				throw Http400Exception(AUTH_INVALID_FIELD);
			throw Http301Exception(m_authLoc + "?" + fieldSplit[0] + "=" + AUTH_NEED_ONE_CHARACTER);
		}
		m_urlEncoded.set(fieldSplit[0], String::urlDecode(fieldSplit[1]));
	}
	if (mode == REGISTER)
	{
		checkField(AUTH_REGISTER_NAME_KEY);
		checkField(AUTH_REGISTER_PSWD_KEY);
		checkField(AUTH_REGISTER_CONFIRM_KEY);
		if (m_urlEncoded.get(AUTH_REGISTER_PSWD_KEY) != m_urlEncoded.get(AUTH_REGISTER_CONFIRM_KEY))
			throw Http301Exception(m_authLoc + "?" + AUTH_REGISTER_CONFIRM_KEY + "=" + AUTH_NEED_MATCH_PSWDS);
	}
	else
	{
		checkField(AUTH_LOGIN_NAME_KEY);
		checkField(AUTH_LOGIN_PSWD_KEY);
	}
}

void HttpAuth::checkField(const std::string field)
{
	if (!m_urlEncoded.has(field))
		throw Http400Exception();
	if (m_urlEncoded.get(field).empty())
		throw Http301Exception(m_authLoc + "?" + field + "=" + AUTH_NEED_ONE_CHARACTER);
}

const Map<std::string> &HttpAuth::getUrlEncoded() const
{
	return m_urlEncoded;
}
