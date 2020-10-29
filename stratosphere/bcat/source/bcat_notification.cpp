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
#include "bcat_notifications.hpp"

namespace ams::bcat::server {

    namespace {

        ncm::ProgramId g_ProcessAppId = ncm::SystemProgramId::Bcat;

        os::ThreadType g_NotificationReceiveThread;
        alignas(os::MemoryPageSize) u8 g_NotificationReceiveThreadStack[0x5000];
        os::EventType g_NotificationReceiveExitEvent;

        NotificationCallback notification_callbacks[4];

        void NotifReceiver(void *) {
            npnsGetReceiveEvent()
        }

    }

    void NotificationInitialize() {
        R_ABORT_UNLESS(npns::ListenTo(g_ProcessAppId));
        R_ABORT_UNLESS(os::ClearEvent(&g_NotificationReceiveExitEvent));
        R_ABORT_UNLESS(os::CreateThread(&g_NotificationReceiveThread, NotifReceiver, nullptr, g_NotificationReceiveThreadStack, sizeof(g_NotificationReceiveThreadStack), AMS_GET_SYSTEM_THREAD_PRIORITY(bcat, NotificationReceiver)));
        R_ABORT_UNLESS(os::SetThreadStackPointer(&g_NotificationReceiveThread, AMS_GET_SYSTEM_THREAD_NAME(bcat, NotificationReceiver)));
    }

    void NotificationFinalize() {
        os::SignalEvent(&g_NotificationReceiveExitEvent);
        os::WaitThread(&g_NotificationReceiveThread);
        os::DestroyThread(&g_NotificationReceiveThread);
    }

    void NotificationAddListener(NotificationCallback callback) {
        for (NotificationCallback &cb : notification_callbacks) {
            if (!cb) {
                cb = callback;
                return;
            }
        }

        AMS_ABORT();
    }

}