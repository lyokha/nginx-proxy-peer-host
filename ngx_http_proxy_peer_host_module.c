/*
 * =============================================================================
 *
 *          File:  ngx_http_proxy_peer_host_module.c
 *
 *   Description:  Variable $proxy_peer_host
 *
 *        Author:  Alexey Radkov
 *       Created:  15.08.2024 19:50
 *
 * =============================================================================
 */

#include <ngx_core.h>
#include <ngx_http.h>


static ngx_int_t ngx_http_proxy_peer_host_add_vars(ngx_conf_t *cf);
static ngx_int_t ngx_http_proxy_peer_host_handler(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);


static ngx_http_variable_t  ngx_http_proxy_peer_host_vars[] =
{
    { ngx_string("proxy_peer_host"), NULL, ngx_http_proxy_peer_host_handler, 0,
      NGX_HTTP_VAR_CHANGEABLE|NGX_HTTP_VAR_NOCACHEABLE|NGX_HTTP_VAR_NOHASH, 0 },

    { ngx_null_string, NULL, NULL, 0, 0, 0 }
};


static ngx_http_module_t  ngx_http_proxy_peer_host_module_ctx = {
    ngx_http_proxy_peer_host_add_vars,       /* preconfiguration */
    NULL,                                    /* postconfiguration */

    NULL,                                    /* create main configuration */
    NULL,                                    /* init main configuration */

    NULL,                                    /* create server configuration */
    NULL,                                    /* merge server configuration */

    NULL,                                    /* create location configuration */
    NULL                                     /* merge location configuration */
};


ngx_module_t  ngx_http_proxy_peer_host_module = {
    NGX_MODULE_V1,
    &ngx_http_proxy_peer_host_module_ctx,    /* module context */
    NULL,                                    /* module directives */
    NGX_HTTP_MODULE,                         /* module type */
    NULL,                                    /* init master */
    NULL,                                    /* init module */
    NULL,                                    /* init process */
    NULL,                                    /* init thread */
    NULL,                                    /* exit thread */
    NULL,                                    /* exit process */
    NULL,                                    /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_int_t
ngx_http_proxy_peer_host_add_vars(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;

    for (v = ngx_http_proxy_peer_host_vars; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_proxy_peer_host_handler(ngx_http_request_t *r,
                                 ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_upstream_rr_peer_t  *peer;

    if (r->upstream == NULL || r->upstream->peer.name == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    peer = (ngx_http_upstream_rr_peer_t *) ((char *) r->upstream->peer.name
                                - offsetof(ngx_http_upstream_rr_peer_t, name));

    v->len = peer->server.len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = peer->server.data;

    return NGX_OK;
}

