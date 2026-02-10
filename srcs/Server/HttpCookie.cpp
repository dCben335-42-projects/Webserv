#include "HttpCookie.hpp"
#include "Number.hpp"
#include <bits/stdc++.h>
#include <functional>
#include <memory>

HttpCookie::HttpCookie() : m_uname(""), m_pswd("")
{
}

HttpCookie::HttpCookie(const std::string name, const std::string pswd, const std::string serverName) : m_uname(name), m_pswd(pswd), m_serverName(serverName)
{
}

HttpCookie::~HttpCookie()
{
}

void HttpCookie::computeHash()
{
}