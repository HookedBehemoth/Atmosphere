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
#include "npns_client.hpp"

namespace ams::npns {

    Client::Client()
        : Xmpp("npns", "0.2.0"),
          last_tick(),
          field_0x9a(0),
          field_0x9e(0) {
        this->field_0xa0  = 0;
        this->field_0x9c  = 0x850;
        this->field_0xb8  = 0;
        this->field_0x8e8 = 0x32;
        this->field_0x98  = 1;
    }

    Result Client::TrySubscribe(const char *node) {
        R_UNLESS(this->IsAuthenticated(), npns::ResultNotAuthenticated());

        R_TRY_CATCH(this->Subscribe(node)) {
            R_CONVERT(npns::ResultXmppNotFound, npns::ResultNotFound());
        } R_END_TRY_CATCH;

        return ResultSuccess();
    }

    Result Client::TryUnsubscribe(const char *node) {
        R_UNLESS(this->IsAuthenticated(), npns::ResultNotAuthenticated());

        R_TRY_CATCH(this->Unsubscribe(node)) {
            R_CONVERT(npns::ResultXmppUnexpectedInput, npns::ResultInvalidInput());
            R_CONVERT(npns::ResultXmppNotFound, npns::ResultNotFound());
        } R_END_TRY_CATCH;

        return ResultSuccess();
    }

    Result Client::TryQueryExists(bool *exists, const char *node) {
        R_UNLESS(this->IsAuthenticated(), npns::ResultNotAuthenticated());

        R_TRY_CATCH(this->QueryExists(node, 1)) {
            R_CATCH(npns::ResultXmppNotFound) {
                *exists = false;
                return ResultSuccess();
            }
        } R_END_TRY_CATCH;

        *exists = true;
        return ResultSuccess();
    }

    void Client::vfun_0x20(UnknownCtx &unk) {
        /* TODO */
    }

    void Client::RunOnce(u64 timeout) {
        if (this->HasProxy()) {
            os::Tick current_tick = os::GetSystemTick();
            if (os::ConvertToTimeSpan(current_tick - this->last_tick) > TimeSpan::FromSeconds(50)) {
                this->SendRaw(" ");
                this->last_tick = current_tick;
            }
        }

        Xmpp::RunOnce(timeout);
    }

    ClientThread::ClientThread()
        : ThreadTemplate(AMS_GET_SYSTEM_THREAD_NAME(npns, Client), AMS_GET_SYSTEM_THREAD_PRIORITY(npns, Client)),
          Client(),
          client_event(os::EventClearMode_ManualClear),
          event_0x9ae8(os::EventClearMode_AutoClear),
          conn_mutex(false) { /* ... */
    }

    Result ClientThread::Initialize() {
        Client::Initialize();
        return ThreadTemplate::Initialize();
    }

    void ClientThread::Finalize() {
        ThreadTemplate::Finalize();
        Client::Finalize();
    }

    Result ClientThread::Connect(const char *jid, const char *pass) {
        std::scoped_lock lk(this->conn_mutex);

        this->event_0x9ae8.Clear();
        R_TRY(this->ConnectClient(jid, pass));
        this->client_event.Signal();
        
        return ResultSuccess();
    }

    void ClientThread::Restart() {
        {
            std::scoped_lock lk(this->conn_mutex);

            this->Disconnect();
            this->client_event.Clear();
        }

        for (int i = 0; i < 0x20; i++) {
            if (!this->IsRunning())
                break;
            
            svc::SleepThread(100'000'000);
        }

        std::scoped_lock lk(this->conn_mutex);
        Xmpp::Restart();
    }

    os::Event *ClientThread::GetEvent_0x9ae8() {
        return &this->event_0x9ae8;
    }

    void ClientThread::NotifyReadyAwaken() {
        /* ... */
    }

    void ClientThread::NotifyAwake() {
        /* ... */
    }

    void ClientThread::NotifyReadyShutdown() {
        /* ... */
    }

    void ClientThread::ThreadFunction() {
        while (!this->should_exit) {
            this->client_event.Wait();
            do {
                Client::RunOnce(100);
                svc::SleepThread(10'000'000);
            } while ((Xmpp::IsRunning() && !this->should_exit));
        }
    }

    void ClientThread::SignalExit() {
        this->client_event.Signal();
        ThreadTemplate::SignalExit();
    }

    void ClientThread::ShowPresence() {
        Xmpp::ShowPresence();
        this->event_0x9ae8.Signal();
    }

    void ClientThread::vfun_0x40() {
        this->event_0x9ae8.Signal();

    }

}