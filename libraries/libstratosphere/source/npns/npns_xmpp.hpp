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
#pragma once
#include <stratosphere.hpp>
//
#include "libstrophe/strophe.h"

namespace ams::npns {

    class Listener : public util::IntrusiveListBaseNode<Listener> {
        NON_COPYABLE(Listener);
        NON_MOVEABLE(Listener);

      public:
        os::Event event;
        u64 title_id;
        Result result;
    };

    class Xmpp {
      public:
        class UnknownCtx;
        class Stanza;

      private:
        os::Mutex mutex_0x10;
        xmpp_ctx_t *ctx;
        xmpp_conn_t *conn;
        xmpp_log_t *log;
        const char *name;
        const char *version;
        u32 field_0x58;
        bool has_proxy;

      private:
        using ListenerEntryList = util::IntrusiveListBaseTraits<Listener>::ListType;

      private:
        ListenerEntryList field_0x60;

        os::Mutex mutex_0x70;

      public:
        Xmpp(const char *name, const char *version);
        void Initialize();
        void Finalize();
        void Restart();

        Result ConnectClient(const char *jid, const char *pass);

        static void ConnectionHandlerEntry(xmpp_conn_t *conn, xmpp_conn_event_t status, int error, xmpp_stream_error_t *stream_error, void *userdata);

        void Disconnect();
        Result SendAndWait(Stanza &stanza);
        void SendRaw(const char *message);

        Result Subscribe(const char *node);
        Result Unsubscribe(const char *node);
        Result QueryExists(const char *node, u32 max_items);

        bool IsAuthenticated();
        bool IsUnauthenticated();
        Result GetResult();
        bool HasProxy();
        static Result GetResult(Stanza &error_stanza);
        bool IsRunning();

        static int VersionHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata);
        static int MessageHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata);
        static int ResultHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata);
        static int ErrorHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata);
        static int AckHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata);
        static int StreamManagementHandlerEntry(xmpp_conn_t *conn, xmpp_stanza_t *stanza, void *userdata);

        void AddHandler(xmpp_handler handler, const char *ns, const char *name, const char *type, void *userdata);

        virtual void RunOnce(u64 timeout);
        virtual void SetHandlers(xmpp_conn_event_t status, int error);
        virtual void GetVersion(Stanza &out);
        virtual void HandleMessage(Stanza &out);
        virtual void vfun_0x20(UnknownCtx &unk);
        virtual void HandleResult(Stanza &out);
        virtual void HandleStreamManagement();
        virtual void ShowPresence();
        virtual void vfun_0x40();

        class Stanza {
            xmpp_stanza_t *impl;
            Result last_rc;

          public:
            Stanza();
            Stanza(xmpp_ctx_t *ctx, const char *name);
            Stanza(xmpp_ctx_t *ctx, const char *name, const char *ns);
            Stanza(xmpp_stanza_t *stanza);
            Stanza(Stanza &rhs);
            virtual ~Stanza();

            void Release();

            void SetName(const char *name);
            void SetNamespace(const char *ns);
            void SetId(const char *id);
            void SetAttribute(const char *key, const char *value);
            void SetAttribute(const char *key, u64 value);

            void AddChild(Stanza &child);
            void CreateChild(const char *text);

            bool GetChildren(Stanza &out);
            bool GetNext();
            bool GetChildByName(const char *name, Stanza &out);

            const char *GetName();
            const char *GetNamespace();
            const char *GetId();
            const char *GetAttribute(const char *name);
            const char *GetType();

            bool Ready();
            bool HasXmppError();

            bool GetText(UnknownCtx &out);

            Result Send(xmpp_conn_t *conn);

            static void Test();
        };

        class StanzaText : public Stanza {
          public:
            StanzaText(xmpp_ctx_t *ctx, const char *name, const char *text);
        };

        class UnknownCtx {
            xmpp_ctx_t *ctx;
            char *text;

          public:
            UnknownCtx();
            ~UnknownCtx();

            void SetText(xmpp_ctx_t *param_2, char *param_3);
            const char *GetText();
            size_t GetLength();
        };
    };

}