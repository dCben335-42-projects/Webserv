# Webserver Configuration Reference

## Overview

The default configuration file is `default.conf`. To override default settings, create a copy of this file with any name you choose.

---

## Configuration Structure

A configuration file is organized into **blocks**, each enclosed in `{ }`. Blocks contain **directives** — key-value pairs that control server behavior.

```
http {
    # Global HTTP settings

    server {
        # Virtual host settings

        location /path {
            # Per-route settings
        }
    }
}
```

---

## Blocks

### `http`
Global settings that apply to the entire HTTP server.

**Allowed directives:**
[`keepalive_timeout`](#keepalive_timeout), [`client_body_timeout`](#client_body_timeout), [`client_header_timeout`](#client_header_timeout), [`client_max_body_size`](#client_max_body_size), [`default_type`](#default_type), [`default_cgi_type`](#default_cgi_type), [`sendfile`](#sendfile)

---

### `server`
Settings for a specific virtual host.

**Allowed directives:**
[`listen`](#listen), [`server_name`](#server_name), [`root`](#root), [`index`](#index), [`autoindex`](#autoindex), [`error_page`](#error_page), [`add_header`](#add_header), [`cgi_pass`](#cgi_pass), [`return`](#return)

---

### `location`
Settings for a specific URL path within a server block.

**Allowed directives:**
[`root`](#root), [`alias`](#alias), [`index`](#index), [`autoindex`](#autoindex), [`allowed_methods`](#allowed_methods), [`return`](#return)

---

## Directives Reference

### Network

#### `listen`
The IP address and port the server listens on for incoming connections.
```
listen <ip>:<port>;
```
```nginx
listen 0.0.0.0:8080;
```

#### `server_name`
One or more hostnames used to match incoming HTTP requests to this server block.
```
server_name <name> [<name> ...];
```
```nginx
server_name example.com www.example.com;
```

#### `allowed_methods`
Restricts which HTTP methods are accepted for a given location.
```
allowed_methods <method> [<method> ...];
```
```nginx
allowed_methods GET POST;
```

---

### File Serving

#### `root`
The base directory from which files are served.
```
root <path>;
```
```nginx
root /var/www/html;
```

#### `alias`
Serves files from an alternative directory for a specific URL path, bypassing the document root.
```
alias <path>;
```
```nginx
alias /srv/static/images;
```

#### `index`
The default file(s) to serve when a directory is requested. Multiple filenames can be specified — the server tries each in order and serves the first one found.

If a filename starts with `/`, it is treated as an absolute path relative to the server's `root`, regardless of the current location's path.
```
index <filename> [<filename> ...];
```
```nginx
index index.html index.htm;   # tries index.html first, then index.htm
index /fallback.html;         # resolved against server root
index index.html /fallback.html;
```

#### `autoindex`
When enabled, generates a directory listing if no index file is found.
```
autoindex on | off;
```

#### `sendfile`
Uses the `sendfile()` syscall to send static files directly from disk to the network, bypassing user space for better performance. Optionally restrict to a specific file path.
```
sendfile on [<path>] | off;
```
```nginx
sendfile on;
sendfile on /var/www/html/large-file.bin;
```

---

### Responses & Redirects

#### `return`
Immediately returns a response with the given status code. Optionally redirects the client to a URL.
```
return <status_code> [<url>];
```
```nginx
return 301 https://example.com/new-path;
return 404;
```

#### `add_header` *(repeatable)*
Adds a custom header to the HTTP response. Can be specified multiple times.
```
add_header <header-name> <value>;
```
```nginx
add_header X-Frame-Options DENY;
add_header Cache-Control "no-store";
```

#### `error_page` *(repeatable)*
Specifies a custom error page for a given HTTP status code. Can be specified multiple times.
```
error_page <status_code> <path>;
```
```nginx
error_page 404 /errors/404.html;
error_page 500 /errors/500.html;
```

---

### CGI

#### `cgi_pass` *(repeatable)*
Maps a URL path to a CGI script to execute. Can be specified multiple times for different paths.
```
cgi_pass <url_path> <script_path>;
```
```nginx
cgi_pass /api/submit /usr/lib/cgi-bin/submit.py;
```

---

### MIME Types

#### `default_type`
The fallback MIME type for files with no recognized extension.
```
default_type <mime-type>;
```
```nginx
default_type application/octet-stream;
```

#### `default_cgi_type`
The fallback MIME type for CGI responses with no recognized content type.
```
default_cgi_type <mime-type>;
```
```nginx
default_cgi_type text/html;
```

---

### Timeouts

All timeout values must be integers between **1 and 999** (in seconds).

#### `keepalive_timeout`
How long to keep an idle connection open for reuse between requests.
```
keepalive_timeout <seconds>;
```

#### `client_body_timeout`
How long to wait for the client to finish sending the request body.
```
client_body_timeout <seconds>;
```

#### `client_header_timeout`
How long to wait for the client to finish sending the request headers.
```
client_header_timeout <seconds>;
```

#### `cgi_timeout`
How long to wait for a CGI script to complete before terminating it and returning an error.
```
cgi_timeout <seconds>;
```

---

### Request Limits

#### `client_max_body_size`
Maximum allowed size of the client request body. Accepts a byte count or a number with a unit suffix.
```
client_max_body_size <size>;
```
```nginx
client_max_body_size 10M;   # 10 megabytes
client_max_body_size 1024;  # 1024 bytes
```

---

### Authentication

#### `auth_loc`
The URL of the authentication endpoint. Used together with `auth_required` to protect specific locations.
```
auth_loc <url>;
```
```nginx
auth_loc /auth/login;
```

#### `auth_required`
Enables or disables authentication enforcement for this location.
```
auth_required on | off;
```

---

### Logging

#### `access_log`
File path where request and response activity is recorded.
```
access_log <path>;
```
```nginx
access_log /var/log/webserver/access.log;
```

#### `error_log`
File path where server errors and processing failures are recorded.
```
error_log <path>;
```
```nginx
error_log /var/log/webserver/error.log;
```

---

## Quick Reference

| Directive | Block(s) | Repeatable |
|---|---|---|
| [`listen`](#listen) | `server` | No |
| [`server_name`](#server_name) | `server` | No |
| [`root`](#root) | `server`, `location` | No |
| [`alias`](#alias) | `location` | No |
| [`index`](#index) | `server`, `location` | No |
| [`autoindex`](#autoindex) | `server`, `location` | No |
| [`allowed_methods`](#allowed_methods) | `location` | No |
| [`return`](#return) | `server`, `location` | No |
| [`sendfile`](#sendfile) | `http` | No |
| [`add_header`](#add_header) | `server` | **Yes** |
| [`cgi_pass`](#cgi_pass) | `server` | **Yes** |
| [`error_page`](#error_page) | `server` | **Yes** |
| [`default_type`](#default_type) | `http` | No |
| [`default_cgi_type`](#default_cgi_type) | `http` | No |
| [`keepalive_timeout`](#keepalive_timeout) | `http` | No |
| [`client_body_timeout`](#client_body_timeout) | `http` | No |
| [`client_header_timeout`](#client_header_timeout) | `http` | No |
| [`cgi_timeout`](#cgi_timeout) | `http` | No |
| [`client_max_body_size`](#client_max_body_size) | `http` | No |
| [`auth_loc`](#auth_loc) | `server`, `location` | No |
| [`auth_required`](#auth_required) | `server`, `location` | No |
| [`access_log`](#access_log) | `http`, `server` | No |
| [`error_log`](#error_log) | `http`, `server` | No |
