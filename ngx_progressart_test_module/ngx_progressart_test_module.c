/*
 * Copyright (C) Smirnov Vladimir
 * Copyright (C) PROGRESSART, Inc.
 */

#include <nginx.h>

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

char *ngx_http_progressart_test_conf(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

void *ngx_http_progressart_test_create_loc_conf(ngx_conf_t *cf);

char *ngx_http_progressart_test_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t ngx_http_progressart_test(ngx_conf_t *cf);

static ngx_int_t ngx_http_progressart_test_handler(ngx_http_request_t *r);

typedef struct {
    ngx_flag_t  enable;
} ngx_http_progressart_test_loc_conf_t;

static ngx_command_t ngx_http_progressart_test_module_commands[] = {
    { ngx_string("progressart_test"),                   /*name derective**/
      NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      offsetof(ngx_http_progressart_test_loc_conf_t, enable),
      0,
      NULL },
    
      ngx_null_command
};

static ngx_http_module_t ngx_http_progressart_test_module_ctx = {
    NULL,                                               /* preconfiguration */
    ngx_http_progressart_test,                          /* postconfiguration */
    NULL,                                               /* create main configuration */
    NULL,                                               /* init main configuration */
    NULL,                                               /* create server configuration */
    NULL,                                               /* merge server configuration */
    ngx_http_progressart_test_create_loc_conf,          /* create location configuration */
    ngx_http_progressart_test_merge_loc_conf            /* merge location configuration */
};

ngx_module_t  ngx_http_progressart_test_module = {
    NGX_MODULE_V1,
    &ngx_http_progressart_test_module_ctx,              /* module context */
    ngx_http_progressart_test_module_commands,          /* module directives */
    NGX_HTTP_MODULE,                                    /* module type */
    NULL,                                               /* init master */
    NULL,                                               /* init module */
    NULL,                                               /* init process */
    NULL,                                               /* init thread */
    NULL,                                               /* exit thread */
    NULL,                                               /* exit process */
    NULL,                                               /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t
ngx_http_progressart_test(ngx_conf_t *cf)
{
    ngx_http_handler_pt *h;
    ngx_http_core_main_conf_t *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
    
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_progressart_test_handler;

    return NGX_OK;
}

void *
ngx_http_progressart_test_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_progressart_test_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_progressart_test_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;
    
    return conf;
}

char *
ngx_http_progressart_test_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_progressart_test_loc_conf_t  *prev =   parent;
    ngx_http_progressart_test_loc_conf_t  *conf =   child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);
    
    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_progressart_test_handler(ngx_http_request_t *r)
{
    ngx_int_t       rc;
    ngx_buf_t       *b;
    ngx_chain_t     out;
    size_t          len;

//    ngx_http_progressart_test_loc_conf_t  *ptlcf;
//    ptlcf = ngx_http_get_module_loc_conf(r, ngx_http_progressart_test_module);

    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Invalid request method");
        return NGX_HTTP_NOT_ALLOWED;
    }

    /*
     typedef stuct {
        ...
        ngx_uint_t            status;
        size_t                content_type_len;
        ngx_str_t             content_type;
        ngx_table_elt_t       *content_encoding;
        off_t                 content_length_n;
        time_t                date_time;
        time_t                last_modified_time;
        ..
     } ngx_http_headers_out_t;
     */
    len = sizeof("{\"r\":\"true\"}") - 1;
    
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_type.len = sizeof("application/json") - 1;
    r->headers_out.content_type.data = (u_char *) "application/json";
    r->headers_out.content_length_n = len;
    
    if (r->method == NGX_HTTP_HEAD) {
        rc = ngx_http_send_header(r);
        if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
            return rc;
        }
    }

    b = ngx_create_temp_buf(r->pool, len);
    if (b == NULL) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to allocate response buffer.");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    out.buf = b;
    out.next = NULL;
    
    b->last = ngx_cpymem(b->last, "{\"r\":\"true\"}", len);
    
    b->last_buf = 1;

    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    return ngx_http_output_filter(r, &out);
}
