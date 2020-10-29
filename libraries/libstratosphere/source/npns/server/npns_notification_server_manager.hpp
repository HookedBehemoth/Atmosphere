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

#include "npns_notification_service.hpp"

namespace ams::npns::server {

    class NotificationServerManager {
        public:
            static constexpr inline size_t NumServers                 = 2;
            static constexpr inline size_t UserNotificationSessions   = 0x1e;
            static constexpr inline size_t SystemNotificationSessions = 0x1e;
            static constexpr inline size_t MaxSessions                = UserNotificationSessions + SystemNotificationSessions;

            static constexpr inline sm::ServiceName UserNotificationServiceName   = sm::ServiceName::Encode("npns:u");
            static constexpr inline sm::ServiceName SystemNotificationServiceName = sm::ServiceName::Encode("npns:s");

            using ServerOptions = sf::hipc::DefaultServerManagerOptions;
            using ServerManager = sf::hipc::ServerManager<NumServers, ServerOptions, MaxSessions>;
        private:
            std::optional<NpnsUser> user_service_holder;
            std::optional<NpnsSystem> system_service_holder;
            std::optional<ServerManager> server_manager_holder;
            os::EventType idle_event;
        public:
            NotificationServerManager() : user_service_holder(), system_service_holder(), server_manager_holder(), idle_event{} { /* ... */ }

            Result Initialize();
            void   Finalize();

            void StartServer();
            void StopServer();

            void RunServer();
    };

}
