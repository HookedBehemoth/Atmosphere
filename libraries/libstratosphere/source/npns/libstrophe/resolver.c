/* resolver.h
 * strophe XMPP client library -- DNS resolver
 *
 * Copyright (C) 2015 Dmitry Podgorny <pasis.ua@gmail.com>
 *
 *  This software is provided AS-IS with no warranty, either express
 *  or implied.
 *
 *  This program is dual licensed under the MIT and GPLv3 licenses.
 */

/** @file
 *  DNS resolver.
 */

#include "resolver.h"

int resolver_srv_lookup_buf(xmpp_ctx_t *ctx, const unsigned char *buf,
                            size_t len, resolver_srv_rr_t **srv_rr_list)
{
    return 0;
}

int resolver_srv_lookup(xmpp_ctx_t *ctx, const char *service, const char *proto,
                        const char *domain, resolver_srv_rr_t **srv_rr_list)
{
    return 0;
}

void resolver_srv_free(xmpp_ctx_t *ctx, resolver_srv_rr_t *srv_rr_list)
{
    resolver_srv_rr_t *rr;

    while (srv_rr_list != NULL) {
        rr = srv_rr_list->next;
        xmpp_free(ctx, srv_rr_list);
        srv_rr_list = rr;
    }
}
