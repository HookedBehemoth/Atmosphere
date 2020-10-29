/*
 * Copyright (c) 2019-2020 Atmosphère-NX
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
#include <stratosphere.hpp>

#include "npns_notification_daemon.hpp"

namespace ams::npns {

    namespace {

        server::NotificationServerManager g_IpcServerManager;
        ClientThread g_ClientThread;
        Controller g_Controller(&g_ClientThread);
        StateMachineThread g_StateMachineThread(&g_Controller);
        SystemEventHandlerThread g_SystemEventHandlerThread;
        Router g_Router;

    };

    void ServerDaemon::Initialize() {
        Xmpp::Stanza::Test();

        R_ABORT_UNLESS(this->m_ClientThread.Initialize());
        R_ABORT_UNLESS(this->m_StateMachineThread.Initialize());
        R_ABORT_UNLESS(this->m_SystemEventHandlerThread.Initialize());
        R_ABORT_UNLESS(this->m_IpcServerManager.Initialize());
    }

    void ServerDaemon::Finalize() {
        this->m_IpcServerManager.Finalize();
        this->m_SystemEventHandlerThread.Finalize();
        this->m_StateMachineThread.Finalize();
        this->m_ClientThread.Finalize();
    }

    ServerDaemon g_Daemon = {
        g_ClientThread,
        g_StateMachineThread,
        g_Router,
        g_Controller,
        g_IpcServerManager,
        g_SystemEventHandlerThread,
    };

}
