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
#pragma once
#include <stratosphere.hpp>

#include "../npns_client.hpp"
#include "../npns_controller.hpp"
#include "../npns_router.hpp"
#include "../npns_state_machine.hpp"
#include "../npns_system_event_handler.hpp"
#include "npns_notification_server_manager.hpp"

namespace ams::npns {

    class ServerDaemon {
      private:
        ClientThread &m_ClientThread;
        StateMachineThread &m_StateMachineThread;
        Router &m_Router;
        Controller &m_Controller;
        server::NotificationServerManager &m_IpcServerManager;
        SystemEventHandlerThread &m_SystemEventHandlerThread;

      public:
        constexpr ServerDaemon(ClientThread &client_thread, StateMachineThread &state_machine_thread, Router &router, Controller &controller, server::NotificationServerManager &ipc_server_manager, SystemEventHandlerThread &system_event_handler_thread)
          : m_ClientThread(client_thread),
            m_StateMachineThread(state_machine_thread),
            m_Router(router),
            m_Controller(controller),
            m_IpcServerManager(ipc_server_manager),
            m_SystemEventHandlerThread(system_event_handler_thread) { /* ... */ }

        void Initialize();
        void Finalize();

        void Run();
    };

    extern ServerDaemon g_Daemon;

}
