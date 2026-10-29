# Webserv

_This project was created as part of the 42 curriculum by dCben335 (Benoit Cabocel), abonneau/AzallLab/Chen (Antoine Bonneau) and tlutz (Thyanoui Lutz). Project lead: dCben335._

## Overview

Webserv is a from-scratch HTTP web server written in C++98. The goal of the project is to understand and reproduce the core behavior of an HTTP server: socket handling, request parsing, virtual hosts, routing, static file serving, CGI execution and event-driven I/O.

The server is single-process and event-driven. It uses `poll()` to multiplex listening sockets, client connections and CGI pipes without blocking on any single client. Configuration is validated at startup against a custom BNF grammar before any socket is opened.

Although the target is HTTP/1.0, a substantial subset of HTTP/1.1 is implemented, including persistent connections, the `Host` header, virtual hosting, content negotiation and `Content-Length` request bodies.

## Team

This project was built by a team of three, led by dCben335.

| Member          | Aliases                  | Role         |
| --------------- | ------------------------ | ------------ |
| Benoit Cabocel  | dCben335, bcabocel       | Project lead |
| Antoine Bonneau | abonneau, AzallLab, Chen | Team member  |
| Thyanoui Lutz   | tlutz                    | Team member  |

### Feature Ownership

The table below identifies the primary owner of each area based on the project's git history. Most features were reviewed, integrated and debugged collaboratively.

| Feature / Area | Primary contributor(s) |
| --- | --- |
| Project leadership, architecture, event loop, virtual servers and client lifecycle | Benoit (dCben335) |
| HTTP request/response pipeline, routing, keep-alive and timeouts | Benoit (dCben335) |
| Static file serving, index resolution, autoindex and `Accept` negotiation | Benoit (dCben335) |
| Configuration data model and runtime integration | Benoit (dCben335) |
| Custom BNF grammar engine, configuration language and configuration parser | Antoine (abonneau) |
| CGI subsystem (`CgiSpawner`, `CgiPipe`, `CgiOutput`) | Thyanoui (tlutz) |
| Multipart file uploads and `PUT` / `PATCH` / `DELETE` handling | Thyanoui (tlutz) |
| Authentication, sessions, cookies and login/register pages | Thyanoui (tlutz) |
| Logging module | Benoit (dCben335) |
| Logging integration in the server, clients and event loop | Thyanoui (tlutz), Benoit (dCben335) |
| Utility kit (containers, `String`, `Number`, `Date`, `IPAddress`) | Benoit (dCben335) |
| Signal handling and graceful shutdown | Thyanoui (tlutz), with Benoit's early-startup handler |
| Build system, default configuration and documentation | Benoit (dCben335), with team contributions |
| Code formatting | Team-wide formatting using `.clang-format` |

## Features

### HTTP

- HTTP/1.0 and HTTP/1.1 request handling.
- Supported methods: `GET`, `POST`, `PUT`, `PATCH`, `DELETE`.
- Persistent connections with `keep-alive` and configurable timeouts.
- `Host`-based virtual hosting, including IPv6 hosts.
- Request parsing with header validation, `Content-Length` checks and body size limits.
- `Accept` content negotiation with quality values (`q=`).
- Custom response headers, redirects and custom error pages.
- Status codes covering 2xx, 3xx, 4xx and 5xx responses.

### Configuration

- Nginx-like `http { server { location { ... } } }` hierarchy.
- Multiple virtual servers on the same or different addresses and ports.
- IPv4, IPv6 and hostname addresses.
- Directive inheritance from `http` to `server` and `location` blocks.
- `root`, `alias`, `index`, `autoindex`, `allowed_methods`, `return`, `error_page`, `add_header`, `cgi_pass`, `auth_required`, `sendfile` and more.
- Configuration is validated by a custom BNF engine before the server starts.
- See [CONFIG.md](CONFIG.md) for the full directive reference.

### File Serving

- Static file serving with automatic MIME type detection.
- Directory index resolution and optional autoindex pages.
- Automatic trailing-slash redirects for directories.
- File permission checks for read, write and execute operations.
- File uploads through `multipart/form-data`.
- Upload, edit and delete actions from generated autoindex pages.

### Authentication

- Built-in register/login page generation for protected locations.
- Password hashing with per-user salts.
- Session tokens stored in `HttpOnly` cookies with expiration.
- Per-server user storage and token validation.
- `auth_required` enforcement on configured locations.

### CGI

- CGI/1.1 execution for Python, PHP, shell and compiled executables.
- Extension-to-interpreter mapping through `cgi_pass`.
- Non-blocking CGI stdin/stdout handled as pollable pipes.
- CGI environment variables built from the incoming request.
- CGI timeouts with configurable limits.
- Sample CGI applications are provided under `files/usr/bin/`:
  - guestbook (C)
  - markdown renderer (C)
  - ROT13 converter (C++)
  - system stats (C)
  - URL shortener and other Python examples
  - shell utilities

### Logging

- Access and error logs, configurable per server.
- Plain-text and JSON log formats.
- Colored console output with indentation levels.
- Automatic fallback to stdout/stderr when log files cannot be opened.
- Request, response, timeout and CGI lifecycle logging.

### Robustness

- Graceful shutdown on `SIGINT`, `SIGQUIT` and `SIGTERM`.
- Early signal handling during startup.
- Poll-based timeout management for headers, bodies, keep-alive and CGI.
- Dedicated HTTP exception hierarchy for error responses.
- Defensive path normalization and duplicate-slash handling.

## Architecture

```
main
 └── ConfigManager
      ├── Lexer (custom BNF engine)
      └── Parser -> HttpNode / ServerNode / LocationNode -> Config
 └── EventLoop (poll)
      ├── VirtualServer  (listening socket, virtual host dispatch)
      │    └── Client    (HTTP request/response state machine)
      │         ├── CgiSpawner -> CgiPipe (CGI process I/O)
      │         └── FileManager / FileUpload / IndexOf / HttpAuth
      └── Signals         (graceful shutdown)
```

The main components are:

- **`srcs/ConfigManager`** — lexes and parses the configuration using the BNF grammar, resolves locations and produces per-server configuration objects.
- **`srcs/EventLoop`** — owns the poll file descriptor set and dispatches ready events to `Pollable` handlers.
- **`srcs/Server/Pollable/VirtualServer`** — creates, binds and listens on sockets, then accepts clients and dispatches requests by `Host` / `server_name`.
- **`srcs/Server/Pollable/Client`** — reads requests, selects the active configuration, handles methods, builds responses and manages keep-alive.
- **`srcs/Server/Pollable/CgiPipe`** — bridges CGI process pipes with the event loop.
- **`srcs/Server/Http`** — HTTP message parsing, headers, methods, cookies, users, authentication and the exception hierarchy.
- **`srcs/Server/Files`** — file operations, MIME detection, multipart uploads and directory listings.
- **`srcs/Signals`** — signal handling and graceful teardown.

## Reusable Modules

The project is split into three internal static libraries under `modules/`:

| Module        | Purpose                                                                                                                 |
| ------------- | ----------------------------------------------------------------------------------------------------------------------- |
| `modules/kit` | C++98 utility kit: `Vec`, `Map`, `Set`, `String`, `Number`, `Date`, `IPAddress` and type traits.                        |
| `modules/log` | Static logging facade with configurable targets, files, indentation and PlainText/JSON formats.                         |
| `modules/bnf` | Custom BNF grammar engine with a semantic descriptor and a path-exploring lexer, used to validate server configuration. |

The BNF module documents its own grammar rules in [modules/bnf/README.md](modules/bnf/README.md) and its internal workflow in [modules/bnf/workflow.md](modules/bnf/workflow.md).

## Repository Layout

```
.
├── config/
│   ├── conf.bnf              # BNF grammar for the configuration language
│   └── default.conf          # Default server configuration
├── files/
│   ├── home/uploads/         # Default upload directory
│   ├── logs/                 # Default access/error logs
│   ├── usr/bin/              # Sample CGI programs
│   └── www/                  # Demo websites and error pages
├── includes/                 # Shared headers and protocol constants
├── modules/
│   ├── bnf/                  # Custom BNF parsing engine
│   ├── kit/                  # C++98 containers and utility classes
│   └── log/                  # Logging library
├── srcs/
│   ├── ConfigManager/        # Configuration lexer, parser and model
│   ├── Server/               # HTTP server, clients, files and CGI
│   ├── Signals/              # Signal handling
│   ├── EventLoop.cpp         # Poll-based main loop
│   └── main.cpp              # Program entry point
├── .clang-format             # Shared C++ formatting rules
├── CONFIG.md                 # Configuration reference
├── Makefile
└── README.md
```

## Build

Requirements:

- A C++ compiler supporting C++98 (`c++`)
- GNU Make
- A POSIX/Linux environment (`poll`, `fork`, `pipe`)
- `clang-format` (optional, for applying the repository code style)

Build the project:

```sh
make
```

Other Makefile targets:

```sh
make re      # full rebuild
make clean   # remove object files and module libraries
make fclean  # remove objects, libraries and the webserv binary
make run     # build and run with the default config
make val     # run under valgrind
```

## Code Style

The team used a shared `.clang-format` file to keep the codebase consistent. It is based on the LLVM style with tab indentation, Allman braces, aligned declarations and assignments, and no column limit.

Apply it to the source files with:

```sh
clang-format -i $(git ls-files '*.cpp' '*.hpp' '*.tpp')
```

## Run

```sh
./webserv
```

This starts the server with the default configuration: `config/default.conf`.

To use another configuration file:

```sh
./webserv path/to/config.conf
```

The default configuration starts several virtual servers on `127.0.0.1`:

- `127.0.0.1:8017` — static demo site and upload endpoints
- `127.0.0.1:8018` — CGI and authentication demo
- `127.0.0.1:8019` — brasserie demo website

Example:

```sh
curl -i http://127.0.0.1:8017/
curl -i http://127.0.0.1:8017/team/
curl -i http://127.0.0.1:8018/cgi-bin/
```

## Configuration

Configuration files follow an Nginx-inspired syntax:

```nginx
http {
    server {
        listen 127.0.0.1:8080;
        server_name example.local;
        root files/www/website;
        index index.html;

        location /uploads {
            alias files/home/uploads;
            autoindex on;
            allowed_methods GET POST;
        }
    }
}
```

The file is read at startup and validated against `config/conf.bnf`. If the configuration is invalid, the server reports the error and exits without starting.

The complete list of blocks and directives is available in [CONFIG.md](CONFIG.md). A working example with virtual hosts, redirects, authentication and CGI is available in [config/default.conf](config/default.conf).

## Testing

Use `curl` for manual HTTP testing:

```sh
# Static file
curl -i http://127.0.0.1:8017/

# Directory listing
curl -i http://127.0.0.1:8017/uploads/

# Custom redirect
curl -i http://127.0.0.1:8017/herisson1

# Teapot
curl -i http://127.0.0.1:8017/teapot

# CGI demo
curl -i http://127.0.0.1:8018/cgi-bin/sysstats
```

Run the server under valgrind with:

```sh
make val
```

## Notes and Limitations

- The implementation targets C++98 and POSIX/Linux APIs.
- HTTPS/TLS is not implemented; the server handles plain HTTP only.
- Request bodies are handled with `Content-Length`; chunked transfer encoding is not a supported feature of the configuration language or request path.
- The BNF configuration language is intentionally strict and is not an arbitrary Nginx configuration parser.
- This is an educational project, not a production-ready web server.

## Resources

- HTTP/1.1 specification: [RFC 2616](https://datatracker.ietf.org/doc/html/rfc2616)
- CGI specification: [RFC 3875](https://datatracker.ietf.org/doc/html/rfc3875)
- HTML and CSS references: [MDN Web Docs](https://developer.mozilla.org/), [W3Schools](https://www.w3schools.com/)
- Linux manual pages for `poll`, `socket`, `bind`, `listen`, `accept`, `fork`, `pipe`, `dup2`, `sendfile` and `stat`

## AI Usage

AI tools were used to clarify specific protocol behaviors that were difficult to find in the documentation, and to automate repetitive tasks such as generating boilerplate code (for example, getters and setters).
