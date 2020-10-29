/* tls_libnx.c
** strophe XMPP client library -- TLS abstraction libnx impl.
**
** Copyright (C) 2005-2009 Collecta, Inc.
**
**  This software is provided AS-IS with no warranty, either express
**  or implied.
**
**  This program is dual licensed under the MIT and GPLv3 licenses.
*/

/** @file
 *  TLS implementation with libnx SSL wrapper.
 */
#include <switch.h>
#include <string.h>

#include "common.h"
#include "tls.h"
#include "sock.h"

struct _tls {
    xmpp_ctx_t *ctx;
    sock_t sock;
    SslContext c;
    SslConnection conn;
    int fd;
    int error;
};

void tls_initialize(void)
{
    /* ... */
}

void tls_shutdown(void)
{
    /* ... */
}

tls_t *tls_new(xmpp_conn_t *conn)
{
    Result rc = 0;
    tls_t *p = NULL;

    p = xmpp_alloc(conn->ctx, sizeof(*p));

    if (p) {
        memset(p, 0, sizeof(*p));
        p->ctx = conn->ctx;
        p->sock = conn->sock;

        rc = sslCreateContext(&p->c, SslVersion_Auto);
        if (R_SUCCEEDED(rc)) {
            u64 id = 0;
            sslContextRegisterInternalPki(
                &p->c, SslInternalPki_DeviceClientCertDefault, &id);
            rc = sslContextCreateConnection(&p->c, &p->conn);
            if (R_SUCCEEDED(rc)) {
                u32 verify_option =
                    hosversionBefore(5, 0, 0)
                        ? 0
                        : SslVerifyOption_PeerCa | SslVerifyOption_HostName;
                rc = sslConnectionSetVerifyOption(&p->conn, verify_option);
                if (R_SUCCEEDED(rc)) {
                    p->fd = socketSslConnectionSetSocketDescriptor(&p->conn,
                                                                   conn->sock);
                    if (p->fd < 0)
                        rc = socketGetLastResult();
                    if (R_SUCCEEDED(rc)) {
                        rc = sslConnectionSetIoMode(&p->conn,
                                                    SslIoMode_NonBlocking);
                        if (R_SUCCEEDED(rc))
                            return p;
                    }
                }
                sslConnectionClose(&p->conn);
            }
            sslContextClose(&p->c);
        }
        xmpp_free(p->ctx, p);
        p = NULL;
    }
    return p;
}

void tls_free(tls_t *tls)
{
    sslConnectionClose(&tls->conn);
    sslContextClose(&tls->c);
    xmpp_free(tls->ctx, tls);
}

int tls_set_credentials(tls_t *tls, const char *cafilename)
{
    Result rc = 0;

    rc = sslConnectionSetHostName(&tls->conn, cafilename, strlen(cafilename));
    if (R_FAILED(rc)) {
        tls->error = R_DESCRIPTION(rc);
        return 0;
    }

    if (hosversionBefore(5, 0, 0)) {
        rc = sslConnectionSetVerifyOption(&tls->conn, SslVerifyOption_HostName);
        if (R_FAILED(rc)) {
            tls->error = R_DESCRIPTION(rc);
            return 0;
        }
    }

    return 1;
}

int tls_start(tls_t *tls)
{
    Result rc = 0;

    while (rc = sslConnectionDoHandshake(&tls->conn, NULL, NULL, NULL, 0),
           R_VALUE(rc) == 0x1987b) {
        svcSleepThread(100000000);
    }

    if (R_FAILED(rc)) {
        tls->error = R_DESCRIPTION(rc);
        return 0;
    }

    return 1;
}

int tls_stop(tls_t *tls)
{
    (void)tls;

    return 1;
}

int tls_error(tls_t *tls)
{
    return tls->error;
}

int tls_pending(tls_t *tls)
{
    s32 pending = 0;
    Result rc = 0;

    rc = sslConnectionPending(&tls->conn, &pending);

    if (R_FAILED(rc)) {
        tls->error = R_DESCRIPTION(rc);
        return -1;
    }

    return pending;
}

int tls_read(tls_t *tls, void *const buff, const size_t len)
{
    u32 read_bytes = 0;
    Result rc = sslConnectionRead(&tls->conn, buff, len, &read_bytes);

    if (R_FAILED(rc)) {
        tls->error = R_DESCRIPTION(rc);
        return -1;
    }

    return read_bytes;
}

int tls_write(tls_t *tls, const void *const buff, const size_t len)
{
    u32 written_bytes = 0;
    Result rc = sslConnectionWrite(&tls->conn, buff, len, &written_bytes);

    if (R_FAILED(rc)) {
        tls->error = R_DESCRIPTION(rc);
        return -1;
    }

    return written_bytes;
}

int tls_clear_pending_write(tls_t *tls)
{
    (void)tls;

    return 0;
}

int tls_is_recoverable(int error)
{
    return error == 0xcc || (error == 0 || error == 0x73);
}