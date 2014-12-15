/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-packet.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-packet.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-packet.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-packet.h"

namespace ns3
{
  namespace nnn
  {
    NNNPacket::NNNPacket(uint32_t pkt_id, Time ttl)
    : m_packetid    (pkt_id)
    , m_ttl         (ttl)
    , m_version     (A_NNN)
    , m_wire        (0)
    {
    }

    NNNPacket::NNNPacket (uint32_t pkt_id, Time ttl, uint16_t ver)
    : m_packetid    (pkt_id)
    , m_ttl         (ttl)
    , m_version     (ver)
    , m_wire        (0)
    {
    }

    NNNPacket::~NNNPacket ()
    {
    }

    inline uint32_t
    NNNPacket::GetPacketId()
    {
      return m_packetid;
    }

    inline uint16_t
    NNNPacket::GetVersion()
    {
      return m_version;
    }

    inline void
    NNNPacket::SetVersion(uint16_t version)
    {
      m_version = version;
    }

    inline Time
    NNNPacket::GetLifetime () const
    {
      return m_ttl;
    }

    inline void
    NNNPacket::SetLifetime (Time ttl)
    {
      m_ttl = ttl;
      m_wire = 0;
    }

    inline Ptr<const Packet>
    NNNPacket::GetWire () const
    {
      return m_wire;
    }

    inline void
    NNNPacket::SetWire (Ptr<const Packet> packet) const
    {
      m_wire = packet;
    }

  } /* namespace nnn */
} /* namespace ns3 */
