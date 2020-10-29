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

namespace ams::npns {

    class SystemEventHandlerThread : public ThreadTemplate<SystemEventHandlerThread, 0x4000ul> {
        psc::PmModule pm_module;
        os::SystemEvent *pm_mutex;
        bool wake;

      public:
        SystemEventHandlerThread();
        virtual ~SystemEventHandlerThread() = default;

        Result Initialize();
        virtual void Finalize() override;

        virtual void ThreadFunction() override;
        virtual void SignalExit() override;
    };

}
