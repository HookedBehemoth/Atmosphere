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
#include "npns_state_machine.hpp"

namespace ams::npns {

    namespace {

        constexpr const char *const StateDescriptions[]{
            [State_Initial]        = "State_Initial",
            [State_Idle]           = "State_Idle",
            [State_AwaitingOnline] = "State_AwaitingOnline",
            [State_Online]         = "State_Online",
            [State_NoValidJid]     = "State_NoValidJid",
            [State_RequestingJid]  = "State_RequestingJid",
            [State_ReadyToConnect] = "State_ReadyToConnect",
            [State_Connecting]     = "State_Connecting",
            [State_Connected]      = "State_Connected",
            [State_BackoffWaiting] = "State_BackoffWaiting",
            [State_Suspend]        = "State_Suspend",
            [State_ShuttingDown]   = "State_ShuttingDown",
            [State_Exit]           = "State_Exit",
        };
    }

    StateMachine::StateMachine(Controller *controller, os::Event *event)
        : controller(controller),
          event(event),
          field_0x20(nullptr),
          field_0x2c(0),
          field_0x30(false),
          field_0x31(false) {
        bool temp = true;

        this->background_processing = settings::fwdbg::GetSettingsItemValue(&temp, 1, "npns", "background_processing") == 1 && temp;
    }

    Result StateMachine::Initialize() {
        this->field_0x8 = this->Get0();
        this->field_0xc = this->Get10();

        return this->controller->Initialize();
    }

    void StateMachine::Finalize() {
        return this->controller->Finalize();
    }

    u32 StateMachine::Get0() {
        return 0;
    }

    u32 StateMachine::Get10() {
        return 10;
    }

    const char *StateMachine::GetStateDescription(int state) {
        if (state >= State_Count)
            return "unknown";

        return StateDescriptions[state];
    }

    bool StateMachine::IsReady(int state) {
        /* GCC doesn't manage to optimize this properly.
        return state == State_Idle ||
                state == State_Online ||
                state == State_Connected ||
                state == State_Suspend ||
                state == State_Exit;*/
        if ((state - 1) < 0xc) {
            return 0xa85 >> ((state - 1) & 0x1f) & 1;
        }
        return false;
    }

    Result StateMachine::FUN_7100017c04() {
        /* TODO */
        return ResultSuccess();
    }

    Result StateMachine::HandleState1(int state) {
        switch (state) {
            case State_Idle:
                this->controller->Submit();
                return ResultSuccess();
            case State_RequestingJid:
                return this->controller->CreateJid();
            case State_Connecting:
                return this->controller->Connect();
            default:
                return 0;
        }
    }

    Result StateMachine::HandleState2(int state1, int state2) {
        switch (state1) {
            case 1:
                if (state2 != 2)
                    this->controller->Cancel();
                break;
            case 4 ... 7:
                if (state2 != 8)
                    this->controller->Restart();
                break;
            case 8:
                this->controller->Restart();

                if (!this->controller->IsAuthenticated())
                    //return this->controller->MultiWait(this->event, 2'000'000'000);
                    return ResultSuccess();

                break;
            case 9:
                if (!this->field_0x30)
                    //return this->controller->MultiWait(this->event, this->field_0x20);
                    return ResultSuccess();

                break;
        }
        return ResultSuccess();
    }

    void StateMachine::FUN_71000189d4() {
        /* TODO */
    }

    void StateMachine::FUN_7100018c90() {
        /* ... */
    }

    void StateMachine::FUN_71000189f4() {
        /* ??? */
        this->field_0x30 = false;
        this->field_0x2c = 0;
        u32 iVar1        = this->field_0x8;
        if (iVar1 != 0xc && iVar1 != 8 && this->background_processing != false) {
            this->field_0x30 = false;
            this->field_0x31 = false;
            if (this->field_0x8 >> 0x20 != 8) {
                this->field_0xc = 8;
            }
        }
    }

    Result StateMachine::FUN_7100018a34(Result result) {
        /* TODO */
        return ResultSuccess();
    }

    StateMachineThread::StateMachineThread(Controller *controller)
        : ThreadTemplate(AMS_GET_SYSTEM_THREAD_NAME(npns, StateMachine), AMS_GET_SYSTEM_THREAD_PRIORITY(npns, StateMachine)),
          StateMachine(controller, &this->event_0x9248),
          mutex_0x9208(true),
          mutex_0x9228(true),
          event_0x9248(os::EventClearMode_AutoClear),
          event_0x9270(os::EventClearMode_AutoClear),
          queue_0x9298(this->buf_0x92e0, 8),
          mutex_0x9320(true),
          event_0x9340(os::EventClearMode_ManualClear),
          result_0x936c(npns::Result_1023()),
          field_0x9370(0),
          bool_0x9374(true),
          bool_0x9375(false),
          field_0x9378(nullptr),
          prev_0x9380(&this->prev_0x9380),
          next_0x9388(&this->prev_0x9380),
          mutex_0x9390(true) { /* ... */
    }

    Result StateMachineThread::Initialize() {
        StateMachine::Initialize();

        R_TRY(ThreadTemplate::Initialize());

        this->field_0x9368 = 0;
        this->bool_0x9375  = true;
        this->event_0x9340.Signal();

        return ResultSuccess();
    }

    void StateMachineThread::Finalize() {
        StateMachine::Finalize();

        ThreadTemplate::Finalize();
    }

    void StateMachineThread::ThreadFunction() {
        /* TODO */
    }

    void StateMachineThread::SignalExit() {
        this->should_exit = true;
        this->event_0x9248.Signal();
    }

}
