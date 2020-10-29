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
#include "npns_controller.hpp"

namespace ams::npns {

    enum State {
        State_Initial,
        State_Idle,
        State_AwaitingOnline,
        State_Online,
        State_NoValidJid,
        State_RequestingJid,
        State_ReadyToConnect,
        State_Connecting,
        State_Connected,
        State_BackoffWaiting,
        State_Suspend,
        State_ShuttingDown,
        State_Exit,

        State_Count,
    };

    class StateMachine {
        u32 field_0x8, field_0xc;
        Controller *controller;
        os::Event *event;
        void *field_0x20; // Timespan?
        u32 field_0x28, field_0x2c;
        bool field_0x30;
        bool field_0x31;
        bool background_processing;

      public:
        StateMachine(Controller *controller, os::Event *event);
        virtual ~StateMachine() = default;

        Result Initialize();
        virtual void Finalize();

        virtual u32 Get0();
        virtual u32 Get10();
        virtual const char *GetStateDescription(int state);
        virtual bool IsReady(int state);
        virtual Result FUN_7100017c04();
        virtual Result HandleState1(int state);
        virtual Result HandleState2(int state1, int state2);
        void FUN_71000189d4();
        virtual void FUN_7100018c90();
        virtual void FUN_71000189f4();
        virtual Result FUN_7100018a34(Result result);


    };

    class StateMachineThread : public ThreadTemplate<StateMachineThread, 0x8000>, public StateMachine {
        os::Mutex mutex_0x9208;
        os::Mutex mutex_0x9228;
        os::Event event_0x9248;
        os::Event event_0x9270;
        os::MessageQueue queue_0x9298;
        uintptr_t buf_0x92e0[8];
        os::Mutex mutex_0x9320;
        os::Event event_0x9340;
        u32 field_0x9368;
        Result result_0x936c;
        u32 field_0x9370;
        bool bool_0x9374;
        bool bool_0x9375;
        void *field_0x9378;
        void *prev_0x9380;
        void *next_0x9388;
        os::Mutex mutex_0x9390;

      public:
        StateMachineThread(Controller *controller);
        virtual ~StateMachineThread() = default;

        Result Initialize();
        virtual void Finalize() override;

        virtual void ThreadFunction() override;
        virtual void SignalExit() override;
    };

}
