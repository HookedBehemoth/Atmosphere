/*
 * Copyright (c) 2018-2020 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "npns_xmpp.hpp"

#include "libstrophe/common.h"

#include <mutex>

namespace ams::npns {

    namespace {

        ALWAYS_INLINE static Result HandleXmppResult(int res) {
            if (res == 0)
                return ResultSuccess();

            switch (res) {
                case XMPP_EMEM:
                    return npns::ResultXmppOutOfMemory();
                case XMPP_EINVOP:
                    return npns::ResultXmppInvalidInput();
                    AMS_UNREACHABLE_DEFAULT_CASE();
            }
        }

    }

    Xmpp::Xmpp(const char *name, const char *version)
        : mutex_0x10(true), mutex_0x70(true) {
        this->ctx        = nullptr;
        this->conn       = nullptr;
        this->log        = nullptr;
        this->name       = name;
        this->version    = version;
        this->field_0x58 = 0;
    }

    void Xmpp::Initialize() {
        std::scoped_lock lk(this->mutex_0x10);
        xmpp_initialize();
        this->has_proxy = false;
        this->log       = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
        this->ctx       = xmpp_ctx_new(nullptr, this->log);
        this->conn      = xmpp_conn_new(this->ctx);
    }

    void Xmpp::Finalize() {
        std::scoped_lock lk(this->mutex_0x10);

        xmpp_conn_release(this->conn);
        this->conn = nullptr;
        xmpp_ctx_free(this->ctx);
        this->ctx = nullptr;
        xmpp_shutdown();
    }

    void Xmpp::Restart() {
        std::scoped_lock lk(this->mutex_0x10);

        this->Finalize();
        this->Initialize();
    }

    Result Xmpp::ConnectClient(const char *jid, const char *pass) {
        std::scoped_lock lk(this->mutex_0x10);

        xmpp_conn_set_jid(this->conn, jid);
        xmpp_conn_set_pass(this->conn, pass);
        xmpp_conn_set_flags(conn, XMPP_CONN_FLAG_LEGACY_SSL);

        this->conn->connect_timeout = 5000;

        /* TODO: Currently broken */
        NifmNetworkProfileData profile;
        if (R_SUCCEEDED(nifmGetCurrentNetworkProfile(&profile))) {
            NifmProxySetting &proxy_setting = profile.ip_setting_data.proxy_setting;
            if (proxy_setting.enabled) {
                xmpp_conn_set_proxy(this->conn, proxy_setting.server, proxy_setting.port);
                if (proxy_setting.auto_auth_enabled) {
                    xmpp_conn_set_proxy_auth(this->conn, proxy_setting.user, proxy_setting.password);
                }
            } else {
                xmpp_conn_set_proxy(this->conn, 0, 0);
            }
            this->has_proxy = proxy_setting.enabled;
        }

        char *const domain = xmpp_jid_domain(this->ctx, jid);

        int res = xmpp_connect_client(this->conn, domain, 443, &Xmpp::ConnectionHandlerEntry, this);

        xmpp_free(this->ctx, domain);

        R_UNLESS(res >= 0, npns::Result_100());

        this->ctx->loop_status = XMPP_LOOP_RUNNING;
        return ResultSuccess();
    }

    void Xmpp::ConnectionHandlerEntry(xmpp_conn_t *conn, xmpp_conn_event_t status, int error, xmpp_stream_error_t *stream_error, void *userdata) {
        AMS_UNUSED(conn, stream_error);

        return reinterpret_cast<Xmpp *>(userdata)->SetHandlers(status, error);
    }

    void Xmpp::Disconnect() {
        std::scoped_lock lk(this->mutex_0x10);

        return xmpp_disconnect(this->conn);
    }

    void Xmpp::RunOnce(u64 timeout) {
        std::scoped_lock lk(this->mutex_0x10);

        return xmpp_run_once(this->ctx, timeout);
    }

    Result Xmpp::SendAndWait(Stanza &stanza) {
        /* TODO */
        /* nn::npns::AsyncManager<nn::npns::Xmpp::Stanza> */
        return ResultSuccess();
    }

    void Xmpp::SendRaw(const char *message) {
        std::scoped_lock lk(this->mutex_0x10);

        return xmpp_send_raw_string(this->conn, message);
    }

    Result Xmpp::Subscribe(const char *node) {
        R_UNLESS(this->IsAuthenticated(), npns::ResultNotAuthenticated());

        Stanza root(this->ctx, "iq", "jabber:client");
        root.SetAttribute("set", "iq");

        Stanza sub(this->ctx, "subscribe");
        sub.SetAttribute("node", node);
        sub.SetAttribute("jid", this->conn->jid);

        {
            Stanza pubsub(this->ctx, "pubsub", "http://jabber.org/protocol/pubsub");
            pubsub.AddChild(sub);

            char domain[0x80];
            std::snprintf(domain, sizeof(domain), "pubsub.%s", this->conn->base_domain);
            root.SetAttribute("to", domain);
            root.AddChild(pubsub);
        }

        return this->SendAndWait(root);
    }

    Result Xmpp::Unsubscribe(const char *node) {
        R_UNLESS(this->IsAuthenticated(), npns::ResultNotAuthenticated());

        Stanza root(this->ctx, "iq", "jabber:client");
        root.SetAttribute("set", "iq");

        Stanza sub(this->ctx, "unsubscribe");
        sub.SetAttribute("node", node);
        sub.SetAttribute("jid", this->conn->jid);

        {
            Stanza pubsub(this->ctx, "pubsub", "http://jabber.org/protocol/pubsub");
            pubsub.AddChild(sub);

            char domain[0x80];
            std::snprintf(domain, sizeof(domain), "pubsub.%s", this->conn->base_domain);
            root.SetAttribute("to", domain);
            root.AddChild(pubsub);
        }

        return this->SendAndWait(root);
    }

    Result Xmpp::QueryExists(const char *node, u32 max_items) {
        R_UNLESS(this->IsAuthenticated(), npns::ResultNotAuthenticated());

        Stanza root(this->ctx, "iq", "jabber:client");
        root.SetAttribute("get", "iq");

        Stanza itms(this->ctx, "items");
        itms.SetAttribute("node", "node");
        itms.SetAttribute("max_items", max_items);

        {
            Stanza pubsub(this->ctx, "pubsub", "http://jabber.org/protocol/pubsub");
            pubsub.AddChild(itms);

            char domain[0x80];
            std::snprintf(domain, sizeof(domain), "pubsub.%s", this->conn->base_domain);
            root.SetAttribute("to", domain);
            root.AddChild(pubsub);
        }

        return this->SendAndWait(root);
    }

    bool Xmpp::IsAuthenticated() {
        return this->conn->state == XMPP_STATE_CONNECTED && this->conn->authenticated;
    }

    bool Xmpp::IsUnauthenticated() {
        return this->conn->state >= XMPP_STATE_CONNECTING && !this->conn->authenticated;
    }

    Result Xmpp::GetResult() {
        R_UNLESS(this->conn->tls_failed == 0, npns::Result_100());
        if (this->conn->error) {
            if (this->conn->error == 0x6e)
                return npns::Result_101();
            else
                return npns::Result_1024();
        }
        R_UNLESS(this->conn->authenticated == 1, npns::Result_410());

        return ResultSuccess();
    }

    bool Xmpp::HasProxy() {
        return this->has_proxy;
    }

    Result Xmpp::GetResult(Stanza &error_stanza) {
        typedef struct {
            const char *desc;
            Result rc;
        } NpnsErrorDescPair;
        constexpr const std::array<NpnsErrorDescPair, 22> XmppErrorArray = {
            {
                {"bad-format", npns::ResultXmppBadFormat()},
                {"bad-request", npns::ResultXmppBadRequest()},
                {"conflict", npns::ResultXmppConflict()},
                {"feature-not-implemented", npns::ResultXmppFeatureNotImplemented()},
                {"forbidden", npns::ResultXmppForbidden()},
                {"gone", npns::ResultXmppGone()},
                {"internal-server-error", npns::ResultXmppInternalServerError()},
                {"item-not-found", npns::ResultXmppNotFound()},
                {"jid-malformed", npns::ResultXmppJidMalformed()},
                {"not-acceptable", npns::ResultXmppNotAcceptable()},
                {"not-allowed", npns::ResultXmppNotAllowed()},
                {"not-authorized", npns::ResultXmppNotAuthorized()},
                {"payment-required", npns::ResultXmppPaymentRequired()},
                {"recipient-unavailable", npns::ResultXmppRecipientUnavailable()},
                {"redirect", npns::ResultXmppRedirect()},
                {"registration-required", npns::ResultXmppRegistrationRequired()},
                {"remote-server-not-found", npns::ResultXmppRemoveServerNotFound()},
                {"remote-server-timeout", npns::ResultXmppRemoveServerTimeout()},
                {"resource-constraint", npns::ResultXmppResourceConstraint()},
                {"service-unavailable", npns::ResultXmppServiceUnavailable()},
                {"subscription-required", npns::ResultXmppSubscriptionRequired()},
                {"unexpected-request", npns::ResultXmppUnexpectedInput()},
            },
        };

        const char *name = error_stanza.GetName();

        for (auto &entry : XmppErrorArray) {
            if (std::strcmp(entry.desc, name))
                continue;

            return entry.rc;
        }

        return npns::ResultXmppUnknownError();
    }

    bool Xmpp::IsRunning() {
        return this->ctx->loop_status == XMPP_LOOP_RUNNING;
    }

    int Xmpp::VersionHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata) {
        AMS_UNUSED(conn);

        Stanza wrapper(stanza);
        reinterpret_cast<Xmpp *>(userdata)->GetVersion(wrapper);
        return 1;
    }

    int Xmpp::MessageHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata) {
        AMS_UNUSED(conn);

        Stanza wrapper(stanza);
        reinterpret_cast<Xmpp *>(userdata)->HandleMessage(wrapper);
        return 1;
    }

    int Xmpp::ResultHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata) {
        AMS_UNUSED(conn);

        Stanza wrapper(stanza);
        reinterpret_cast<Xmpp *>(userdata)->HandleResult(wrapper);
        return 1;
    }

    int Xmpp::ErrorHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata) {
        AMS_UNUSED(conn);

        Stanza wrapper(stanza);
        reinterpret_cast<Xmpp *>(userdata)->HandleResult(wrapper);
        return 1;
    }

    int Xmpp::AckHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata) {
        AMS_UNUSED(conn, stanza);

        reinterpret_cast<Xmpp *>(userdata)->field_0x58++;
        return 1;
    }

    int Xmpp::StreamManagementHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata) {
        AMS_UNUSED(conn, stanza);

        reinterpret_cast<Xmpp *>(userdata)->HandleStreamManagement();
        return 1;
    }

    void Xmpp::SetHandlers(xmpp_conn_event_t status, int error) {
        AMS_UNUSED(error);

        switch (status) {
            case XMPP_CONN_CONNECT:
            case XMPP_CONN_RAW_CONNECT:
                this->AddHandler(VersionHandlerEntry, "jabber:iq:version", "iq", nullptr, this);
                this->AddHandler(MessageHandlerEntry, nullptr, "message", nullptr, this);
                this->AddHandler(ResultHandlerEntry, "jabber:client", "iq", "result", this);
                this->AddHandler(ErrorHandlerEntry, "jabber:client", "iq", "error", this);
                this->AddHandler(AckHandlerEntry, nullptr, "message", nullptr, this);
                this->AddHandler(AckHandlerEntry, nullptr, "iq", nullptr, this);
                this->AddHandler(StreamManagementHandlerEntry, "urn:xmpp:sm:3", "r", nullptr, this);
                this->ShowPresence();
                break;
            case XMPP_CONN_DISCONNECT:
            case XMPP_CONN_FAIL:
                xmpp_stop(this->ctx); // TODO: wrapper?
                this->vfun_0x40();
        }
    }

    void Xmpp::AddHandler(xmpp_handler handler, const char *ns, const char *name, const char *type, void *userdata) {
        std::scoped_lock lk(this->mutex_0x10);
        xmpp_handler_add(this->conn, handler, ns, name, type, userdata);
    }

    void Xmpp::ShowPresence() {
        {
            Stanza presence(this->ctx, "presence");
            StanzaText show(this->ctx, "show", "chat");

            presence.AddChild(show);

            std::scoped_lock lk(this->mutex_0x10);
            presence.Send(this->conn);
        }

        this->field_0x58 = 0;

        this->SendRaw("<enable xmlns=\'urn:xmpp:sm:3\'/>");
    }

    void Xmpp::vfun_0x40() {}

    void Xmpp::GetVersion(Stanza &out) {
        Stanza iq(this->ctx, "iq");
        iq.SetAttribute("result", "iq");
        iq.SetId(out.GetId());
        iq.SetAttribute("to", out.GetAttribute("from"));

        Stanza query(this->ctx, "query");
        Stanza children;
        if (out.GetChildren(children)) {
            const char *ns = children.GetNamespace();
            if (ns)
                query.SetNamespace(ns);
        }

        {
            StanzaText name(this->ctx, "name", this->name);
            query.AddChild(name);
            StanzaText version(this->ctx, "version", this->version);
            query.AddChild(version);
        }

        iq.AddChild(query);

        std::scoped_lock lk(this->mutex_0x10);
        iq.Send(this->conn);
    }

    void Xmpp::HandleMessage(Stanza &out) {
        if (out.HasXmppError())
            return;

        {
            Stanza child;
            if (out.GetChildByName("body", child)) {
                UnknownCtx unk;
                if (child.GetText(unk))
                    this->vfun_0x20(unk);
                return;
            }
        }

        Stanza event;
        Stanza items;
        Stanza children;
        if (!out.GetChildByName("event", event) ||
            !event.GetChildByName("items", items) ||
            !items.GetChildren(children))
            return;

        do {
            Stanza child;
            if (children.GetChildByName("entry", child)) {
                UnknownCtx unk;
                if (child.GetText(unk))
                    this->vfun_0x20(unk);
            }
        } while (children.GetNext());
    }

    void Xmpp::HandleResult(Stanza &out) {
        const char *id = out.GetId();
        if (id == nullptr)
            return;

        u64 title_id = std::strtoul(id, nullptr, 10);
        if (title_id == 0)
            return;

        Result rc = ResultSuccess();

        if (out.HasXmppError()) {
            Stanza error;
            Stanza error_text;
            rc = npns::Result_1023();
            if (out.GetChildByName("error", error) &&
                error.GetChildren(error_text))
                rc = Xmpp::GetResult(error_text);
        }

        std::scoped_lock lk(this->mutex_0x70);
        for (auto &entry : this->field_0x60) {
            if (entry.title_id != title_id)
                continue;

            entry.result = rc;
            entry.event.Signal();
            break;
        }
    }

    void Xmpp::HandleStreamManagement() {
        char buffer[0x40];
        std::snprintf(buffer, sizeof(buffer), "<a xmlns=\'urn:xmpp:sm:3\' h=\'%d\'/>", this->field_0x58);
        this->SendRaw(buffer);
    }

    void Xmpp::vfun_0x20(UnknownCtx &unk) {}

    Xmpp::Stanza::Stanza() {
        this->impl    = nullptr;
        this->last_rc = ResultSuccess();
    }

    Xmpp::Stanza::Stanza(xmpp_ctx_t *ctx, const char *name) : Stanza() {
        this->impl = xmpp_stanza_new(ctx);

        if (!this->impl) {
            this->last_rc = npns::ResultXmppOutOfMemory();
            return;
        }

        this->SetName(name);
    }

    Xmpp::Stanza::Stanza(xmpp_ctx_t *ctx, const char *name, const char *ns) : Stanza() {
        this->impl = xmpp_stanza_new(ctx);

        if (!this->impl) {
            this->last_rc = npns::ResultXmppOutOfMemory();
            return;
        }

        this->SetName(name);
        this->SetNamespace(ns);
    }

    Xmpp::Stanza::Stanza(xmpp_stanza_t *stanza) {
        this->impl    = xmpp_stanza_clone(stanza);
        this->last_rc = ResultSuccess();
    }

    Xmpp::Stanza::Stanza(Stanza &rhs) {
        this->impl    = xmpp_stanza_clone(rhs.impl);
        this->last_rc = rhs.last_rc;
    }

    Xmpp::Stanza::~Stanza() {
        this->Release();
    }

    void Xmpp::Stanza::Release() {
        if (this->impl) {
            xmpp_stanza_release(this->impl);
            this->impl = nullptr;
        }
    }

    void Xmpp::Stanza::SetName(const char *name) {
        if (R_FAILED(this->last_rc))
            return;

        this->last_rc = HandleXmppResult(xmpp_stanza_set_name(this->impl, name));
    }

    void Xmpp::Stanza::SetNamespace(const char *ns) {
        if (R_FAILED(this->last_rc))
            return;

        this->last_rc = HandleXmppResult(xmpp_stanza_set_ns(this->impl, ns));
    }

    void Xmpp::Stanza::SetId(const char *id) {
        if (R_FAILED(this->last_rc))
            return;

        this->last_rc = HandleXmppResult(xmpp_stanza_set_id(this->impl, id));
    }

    void Xmpp::Stanza::SetAttribute(const char *key, const char *value) {
        if (R_FAILED(this->last_rc))
            return;

        this->last_rc = HandleXmppResult(xmpp_stanza_set_attribute(this->impl, key, value));
    }

    void Xmpp::Stanza::SetAttribute(const char *key, u64 value) {
        if (R_FAILED(this->last_rc))
            return;

        char buffer[20];
        std::snprintf(buffer, sizeof(buffer), "%ld", value);
        this->last_rc = HandleXmppResult(xmpp_stanza_set_attribute(this->impl, key, buffer));
    }

    void Xmpp::Stanza::AddChild(Stanza &child) {
        Result rhs_rc = child.last_rc;
        Result lhs_rc = this->last_rc;

        if (R_FAILED(rhs_rc) && R_SUCCEEDED(this->last_rc)) {
            this->last_rc = rhs_rc;
        } else if (R_SUCCEEDED(lhs_rc)) {
            this->last_rc = HandleXmppResult(xmpp_stanza_add_child(this->impl, child.impl));
        }
    }

    void Xmpp::Stanza::CreateChild(const char *text) {
        if (R_FAILED(this->last_rc))
            return;

        Stanza child(this->impl->ctx, text);
        if (R_SUCCEEDED(child.last_rc))
            this->AddChild(child);

        if (R_SUCCEEDED(this->last_rc))
            this->last_rc = child.last_rc;
    }

    bool Xmpp::Stanza::GetChildren(Stanza &out) {
        xmpp_stanza_t *children = xmpp_stanza_get_children(this->impl);

        if (children == nullptr)
            return false;

        out.Release();

        out.impl = xmpp_stanza_clone(children);
        return true;
    }

    bool Xmpp::Stanza::GetNext() {
        xmpp_stanza_t *next = xmpp_stanza_get_next(this->impl);

        if (next == nullptr)
            return false;

        this->Release();
        this->impl = xmpp_stanza_clone(next);
        return true;
    }

    bool Xmpp::Stanza::GetChildByName(const char *name, Stanza &out) {
        xmpp_stanza_t *child = xmpp_stanza_get_child_by_name(this->impl, name);

        if (child == nullptr)
            return false;

        out.Release();

        out.impl = xmpp_stanza_clone(child);
        return true;
    }

    const char *Xmpp::Stanza::GetName() {
        return xmpp_stanza_get_name(this->impl);
    }

    const char *Xmpp::Stanza::GetNamespace() {
        return xmpp_stanza_get_ns(this->impl);
    }

    const char *Xmpp::Stanza::GetId() {
        return this->Ready() ? xmpp_stanza_get_id(this->impl) : nullptr;
    }

    const char *Xmpp::Stanza::GetAttribute(const char *name) {
        return this->Ready() ? xmpp_stanza_get_attribute(this->impl, name) : nullptr;
    }

    const char *Xmpp::Stanza::GetType() {
        return this->Ready() ? xmpp_stanza_get_type(this->impl) : nullptr;
    }

    bool Xmpp::Stanza::Ready() {
        return this->impl && R_SUCCEEDED(this->last_rc);
    }

    bool Xmpp::Stanza::HasXmppError() {
        return this->GetType() && std::strcmp(GetType(), "error") == 0;
    }

    bool Xmpp::Stanza::GetText(UnknownCtx &out) {
        if (!this->Ready())
            return false;

        char *text = xmpp_stanza_get_text(this->impl);
        if (!text)
            return false;

        out.SetText(this->impl->ctx, text);
        return true;
    }

    Result Xmpp::Stanza::Send(xmpp_conn_t *conn) {
        R_TRY(this->last_rc);

        xmpp_send(conn, this->impl);

        return ResultSuccess();
    }

    void Xmpp::Stanza::Test() {
        xmpp_ctx_t *ctx = xmpp_ctx_new(nullptr, nullptr);
        Stanza stanza_org(ctx, "name", "ns");
        Stanza stanza_copy(stanza_org);
        Stanza stanza_new(ctx, "name", "ns");
        // AMS_ASSERT(stanza_org == stanza_copy);
        // AMS_ASSERT(stanza_org == stanza_new);
        // AMS_ASSERT(stanza_copy == stanza_new);
        xmpp_ctx_free(ctx);
    }

    Xmpp::StanzaText::StanzaText(xmpp_ctx_t *ctx, const char *name, const char *text)
        : Stanza(ctx, name) {
        this->CreateChild(text);
    }

    Xmpp::UnknownCtx::UnknownCtx() {
        this->ctx  = nullptr;
        this->text = nullptr;
    }

    Xmpp::UnknownCtx::~UnknownCtx() {
        if (this->text) {
            xmpp_free(this->ctx, this->text);
            this->text = nullptr;
        }
    }

    void Xmpp::UnknownCtx::SetText(xmpp_ctx_t *ctx, char *text) {
        this->~UnknownCtx();
        this->ctx  = ctx;
        this->text = text;
    }

    const char *Xmpp::UnknownCtx::GetText() {
        return this->text;
    }

    size_t Xmpp::UnknownCtx::GetLength() {
        return this->text ? std::strlen(this->text) : 0;
    }

}
