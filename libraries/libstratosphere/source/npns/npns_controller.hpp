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
#include "npns_client.hpp"

namespace ams::npns {

    struct JabberIdentifier {
        char jid[0x40];
        char pass[0x40];
    };

    class TokenTimeoutHelper {
      private:
        os::Tick last_tick;
        TimeSpan expiration;
        bool has;

      public:
        TokenTimeoutHelper() : last_tick(0), expiration(0), has(false) { /* ... */ }
        ~TokenTimeoutHelper() = default;

        bool IsValid() {
            return os::ConvertToTick(this->expiration - TimeSpan::FromMinutes(1)) + this->last_tick > os::GetSystemTick() && this->has;
        }

        void Invalidate() {
            this->has = false;
        }

        void Validate(TimeSpan &expiration) {
            this->has = true;
            this->last_tick = os::GetSystemTick();
            this->expiration = expiration;
        }
    };

    class AuthHelper {
        private:
        /* http::ConnectionBroker *connection_broker; */
        TokenTimeoutHelper *timeout_helper;
        public:
        AuthHelper(/* http::ConnectionBroker *connection_broker, */ TokenTimeoutHelper *timeout_helper) : /* connection_broker(connection_broker), */ timeout_helper(timeout_helper) { /* ... */ }
        ~AuthHelper() = default;
    };

    struct JabberHelper {
        u32 unk;
        bool has_jid;
        JabberIdentifier jid;
        char env[4];
    };

    class Controller {
        os::Event event_0x0;
        ClientThread *client_thread;
        NifmRequest request_client;
        /* http::ConnectionBroker *connection_broker; */
        TokenTimeoutHelper timeout_helper;
        JabberHelper jabber;

      public:
        Controller(ClientThread *client_thread);
        ~Controller() = default;

        Result Initialize();
        void Finalize();

        Result Connect();
        void Restart();
        bool IsAuthenticated();
        bool IsUnauthenticated();
        Result GetResult();
        Result CreateJid();
        Result DestroyJid();
        Result SetJid(JabberIdentifier &jid);
        Result ClearJid();
        void SetHasJid();
        Result CreateTokenWithApplicationId();
        bool HasJid();
        Result DestroyTokenWithApplicationId();
        Result WaitForNetworkAvailability();
        bool IsNetworkAvailable();
        Result FUN_710001688c();
        Result FUN_7100016968();
        void Submit();
        void Cancel();
        Result GetJid(JabberIdentifier &out);
    };

}
