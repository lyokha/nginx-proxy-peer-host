# vi:filetype=

use Test::Nginx::Socket;

repeat_each(2);
plan tests => repeat_each() * (2 * (blocks()));

no_shuffle();
run_tests();

__DATA__

=== TEST 1: main server in upstream
--- http_config
    upstream u1 {
        server 127.0.0.1:8080;
        server localhost:8090 backup;
    }

    server {
        listen       8080;
        server_name  backend8080;

        location / {
            return 200;
        }

        location /503 {
            return 503;
        }
    }

    server {
        listen       8090;
        server_name  backend8090;

        location / {
            return 200;
        }
    }
--- config
        proxy_next_upstream error timeout http_503;

        location /u1 {
            proxy_pass http://u1;
            echo_after_body "[u1] proxy_peer_host: $proxy_peer_host";
            echo_after_body "     proxy_host: $proxy_host";
        }

        location /noproxy {
            echo "[noproxy] proxy_peer_host: $proxy_peer_host";
            echo "          proxy_host: $proxy_host";
        }

        location /503 {
            proxy_pass http://u1;
            echo_after_body "[u1] proxy_peer_host: $proxy_peer_host";
            echo_after_body "     proxy_host: $proxy_host";
        }
--- request
GET /u1
--- response_body
[u1] proxy_peer_host: 127.0.0.1:8080
     proxy_host: u1
--- error_code: 200

=== TEST 2: no proxy
--- request
    GET /noproxy
--- response_body
[noproxy] proxy_peer_host: 
          proxy_host: 
--- error_code: 200

=== TEST 3: disable main server in upstream
--- request
    GET /503
--- response_body
[u1] proxy_peer_host: localhost:8090
     proxy_host: u1
--- error_code: 200

=== TEST 4: backup server in upstream
--- request
    GET /u1
--- response_body
[u1] proxy_peer_host: localhost:8090
     proxy_host: u1
--- error_code: 200

