*This project has been created as part of the 42 curriculum by bcabocel, abonneau, tlutz*

## Description

This is Webserv, a school project where the goal is to reproduce an HTTP web server in c++ from scratch.
We are asked to apply and understand the basics of the web development and especially the HTTP 1.0 protocol.

## Instructions

How to use : Compile the project with make, then launch it with the executable called 'webserv'.

It can handle an optional config file as an CLI argument, if no argument is provided a default config file in the repo will be used.

If the config file is valid, the webserv will launch and you will be able to access it with the ip:port set up in the config file.

Additionally, an error and access log file will be created, the server will fill them with useful information during the process.

Note: This webserv handles some features of the HTTP 1.1 protocol.

## Resources

A lot of resources have been used to build this project, especially since we applied a protocol heavily used in the world.


#### The main source of documentation and rules about the HTTP protocol : [RFC-HTTP](https://datatracker.ietf.org/doc/html/rfc2616)

The IETF, (Internet Engineering Task Force) is the reference for internet related protocols and rules.


#### Same for CGI Protocol : [RFC-CGI](https://datatracker.ietf.org/doc/html/rfc3875)


#### For the AutoIndex and Session management feature, some html and css code was created, here are the main sources of documentation for those two languages : [W3Schools](https://www.w3schools.com/) [MDN](https://developer.mozilla.org/fr/)


#### 


AI tools were used to clarify specific behaviors that were difficult to find in documentation and to automate repetitive tasks such as generating boilerplate code (for example, getters and setters).