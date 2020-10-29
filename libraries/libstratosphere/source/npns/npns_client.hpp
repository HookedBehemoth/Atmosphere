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
#include "npns_thread_template.hpp"
#include "npns_xmpp.hpp"

namespace ams::npns {

    class Client : public Xmpp {
        os::Tick last_tick;
        u16 field_0x98;
        u16 field_0x9a;
        u16 field_0x9c;
        u16 field_0x9e;
        u64 field_0xa0;
        u64 field_0xa8;
        u64 field_0xb0;
        u64 field_0xb8;
        u8 pad_0xc0[0x828];
        u32 field_0x8e8;

      public:
        Client();

        Result TrySubscribe(const char *node);
        Result TryUnsubscribe(const char *node);
        Result TryQueryExists(bool *exists, const char *node);

        virtual void vfun_0x20(UnknownCtx &unk) override;
        virtual void RunOnce(u64 timeout) override;
    };

    class ClientThread : public ThreadTemplate<ClientThread, 0x8000ul>, public Client {
        os::Event client_event;
        os::Event event_0x9ae8;
        os::Mutex conn_mutex;

      public:
        ClientThread();
        virtual ~ClientThread() = default;

        Result Initialize();
        virtual void Finalize() override;

        Result Connect(const char *jid, const char *pass);
        void Restart();
        os::Event *GetEvent_0x9ae8();

        void NotifyReadyAwaken();
        void NotifyAwake();
        void NotifyReadyShutdown();

        virtual void ThreadFunction() override;
        virtual void SignalExit() override;

        virtual void ShowPresence() override;
        virtual void vfun_0x40() override;
    };

}
