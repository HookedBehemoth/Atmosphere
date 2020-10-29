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

namespace ams::npns {

    constexpr const SocketInitConfig g_SocketConfig = {
        .bsdsockets_version = 1,

        .tcp_tx_buf_size     = 0x8000,
        .tcp_rx_buf_size     = 0x10000,
        .tcp_tx_buf_max_size = 0x5b000,
        .tcp_rx_buf_max_size = 0x5b000,

        .udp_tx_buf_size = 0x2400,
        .udp_rx_buf_size = 0xA500,

        .sb_efficiency = 3,

        .num_bsd_sessions = 2,
        .bsd_service_type = BsdServiceType_User,
    };

}
