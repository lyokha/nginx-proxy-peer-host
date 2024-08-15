nginx-proxy-peer-host
=====================

This Nginx HTTP module provides only one variable *&dollar;proxy_peer_host*
which is similar to variable *&dollar;proxy_host* from [Nginx HTTP proxy
module](https://nginx.org/en/docs/http/ngx_http_proxy_module.html). While the
latter variable contains the name of the upstream which is being proxied to,
variable *&dollar;proxy_peer_host* contains the name of the actual server inside
the upstream.

Let's test a simple configuration.

```nginx
user                    nobody;
worker_processes        1;

events {
    worker_connections  1024;
}

http {
    default_type        application/octet-stream;
    sendfile            on;

    upstream u1 {
        server 127.0.0.1:8080;
        server localhost:8080;
    }

    server {
        listen       8010;
        server_name  main;

        location /u1 {
            proxy_pass http://u1;
            echo_after_body "[u1] proxy_peer_host: $proxy_peer_host";
            echo_after_body "     proxy_host: $proxy_host";
        }
    }

    server {
        listen       8080;
        server_name  backend;

        location / {
            return 200;
        }
    }
}
```

The value of *&dollar;proxy_peer_host* must change in round-robin manner when
we do sequential HTTP requests to location */u1*.

```ShellSession
$ curl 'http://localhost:8010/u1'
[u1] proxy_peer_host: 127.0.0.1:8080
     proxy_host: u1
$ curl 'http://localhost:8010/u1'
[u1] proxy_peer_host: localhost:8080
     proxy_host: u1
$ curl 'http://localhost:8010/u1'
[u1] proxy_peer_host: localhost:8080
     proxy_host: u1
$ curl 'http://localhost:8010/u1'
[u1] proxy_peer_host: 127.0.0.1:8080
     proxy_host: u1
```

Variable *&dollar;proxy_peer_host* can be used in directive *proxy_ssl_name* to
verify names of backend servers against the names given in directives *server*
inside the upstream rather than the name of the upstream itself.

