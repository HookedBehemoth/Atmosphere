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
#include "npns_system_event_handler.hpp"

namespace ams::npns {

    namespace {

        constexpr const psc::PmModuleId PmDependencies[] = {
            static_cast<psc::PmModuleId>(3), /* I tried... */
            psc::PmModuleId_Fs,
        };

    }

    SystemEventHandlerThread::SystemEventHandlerThread()
        : ThreadTemplate(AMS_GET_SYSTEM_THREAD_NAME(npns, SystemEventHandler), AMS_GET_SYSTEM_THREAD_PRIORITY(npns, SystemEventHandler)),
          pm_module(),
          pm_mutex(nullptr),
          wake(true) {
    }

    Result SystemEventHandlerThread::Initialize() {
        R_TRY(this->pm_module.Initialize(psc::PmModuleId_Npns, PmDependencies, sizeof(PmDependencies) / sizeof(psc::PmModuleId), os::EventClearMode_AutoClear));

        auto pm_guard = SCOPE_GUARD {
            this->pm_mutex = nullptr;
            this->pm_module.Finalize();
        };

        this->pm_mutex = this->pm_module.GetEventPointer();

        R_TRY(ThreadTemplate::Initialize());

        pm_guard.Cancel();

        return ResultSuccess();
    }

    void SystemEventHandlerThread::Finalize() {
        this->SignalExit();
        ThreadTemplate::Finalize();

        this->pm_mutex = nullptr;
        this->pm_module.Finalize();
    }

    void SystemEventHandlerThread::ThreadFunction() {
        this->pm_mutex->Wait();

        while (!this->should_exit) {
            psc::PmState state;
            psc::PmFlagSet flags;
            if (this->pm_module.GetRequest(&state, &flags).IsSuccess() && state < 6) {
                switch (state) {
                    case psc::PmState_Awake:
                        this->wake = true;
                        // daemon->m_StateMachineThread.NotifyAwake();
                        // daemon->m_ClientThread.NotifyAwake();
                        break;
                    case psc::PmState_ReadyAwaken:
                        if (this->wake) {
                            // daemon->m_StateMachineThread.NotifyReadyAwaken();
                            // daemon->m_ClientThread.NotifyReadyAwaken();
                        }
                        this->wake = true;
                    case psc::PmState_ReadyShutdown:
                        // daemon->m_StateMachineThread.NotifyReadyShutdown();
                        // daemon->m_ClientThread.NotifyReadyShutdown();
                        break;
                    default:
                        this->wake = false;
                }

                this->pm_module.Acknowledge(state, ResultSuccess());
            }
            this->pm_mutex->Wait();
        }
    }

    void SystemEventHandlerThread::SignalExit() {
        this->pm_mutex->Signal();
        this->should_exit = true;
    }

}
