/*
 * Copyright (c) 2018-2019 Atmosphère-NX
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

namespace ams::npns {

    #define AMS_NPNS_USER_NOTIFICATION_SERVICE_INTERFACE_INFO(C, H)\
        AMS_SF_METHOD_INFO(C, H, 1, void, Boobs, ())

    #define AMS_NPNS_SYSTEM_NOTIFICATION_SERVICE_INTERFACE_INFO(C, H)\
        AMS_SF_METHOD_INFO(C, H, 1, void, Boobs, ())

    AMS_SF_DEFINE_INTERFACE(INpnsUser, AMS_NPNS_USER_NOTIFICATION_SERVICE_INTERFACE_INFO)
    AMS_SF_DEFINE_INTERFACE(INpnsSystem, AMS_NPNS_SYSTEM_NOTIFICATION_SERVICE_INTERFACE_INFO)

    class NpnsUser final {
        public:
            void Boobs();
    };
    static_assert(IsINpnsUser<NpnsUser>);

    class NpnsSystem final {
        public:
            void Boobs();
    };
    static_assert(IsINpnsSystem<NpnsSystem>);

}