/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-packet.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-packet.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-packet.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/simple-ref-count.h>

#ifndef NNN_PACKET_H_
#define NNN_PACKET_H_

namespace ns3
{
  namespace nnn
  {
    /**
     * @brief enum for NNN PacketId for NNN Packets
     */
    enum NNN_PDU_TYPE { NULL_NNN = 0, SO_NNN, DO_NNN, DU_NNN, EN_NNN, AEN_NNN, REN_NNN, DEN_NNN, INF_NNN};

    /**
     * @brief enum for payload type the NNN data transmission packets are carrying
     */
    enum NNN_PDU_TRANS { NDN_NNN = 1 , NNN_NNN };

    /**
     * @brief enum for NNN version being used
     */
    enum NNN_VER { A_NNN = 0 };

    /**
     * @brief enum for NNN POAs supported
     */
    enum POA_TYPES { POA_MAC48 = 0, POA_IP };

    class NNNPacket
    {
    public:

      NNNPacket (uint32_t pkt_id, Time ttl);

      NNNPacket (uint32_t pkt_id, Time ttl, uint16_t ver);

      virtual
      ~NNNPacket ();

      /**
       * \brief Return Id of the packet
       *
       */
      inline uint32_t
      GetPacketId() const;

      /**
       * \brief Get the version of the packet
       */
      inline uint16_t
      GetVersion () const;

      /**
       * \brief Set the version of the packet
       * @param version Version number
       */
      inline void
      SetVersion (uint16_t version);

      /**
       * \brief Get time out value for NNN packets
       * Indicates the (approximate) time remaining before the packet times out.
       * The timeout is relative to the arrival time of the interest at the current node.
       * Based heavily on the NDN implementation for Interest Life time
       * \see http://www.ndn.org/releases/latest/doc/technical/InterestMessage.html for more information.
       */
      inline Time
      GetLifetime () const;

      /**
       * \brief Set time out for NNN packets
       * Indicates the (approximate) time remaining before the packet times out.
       * The timeout is relative to the arrival time of the interest at the current node.
       * Based heavily on the NDN implementation for Interest Life time
       * \see http://www.ndn.org/releases/latest/doc/technical/InterestMessage.html for more information.
       * @param[in] time interest lifetime
       */
      inline void
      SetLifetime (Time ttl);

      /**
       * @brief Get wire formatted packet
       *
       * If wire formatted packet has not been set before, 0 will be returned
       */
      inline Ptr<const Packet>
      GetWire () const;

      /**
       * @brief Set (cache) wire formatted packet
       */
      inline void
      SetWire (Ptr<const Packet> packet) const;

    protected:
      uint32_t m_packetid;              ///< @brief Packet Identifier
      Time m_ttl;                           ///< @brief Packet life time (TTL)
      uint16_t m_version;                   ///< @brief NNN Packet version
      mutable Ptr<const Packet> m_wire;
    };

    inline uint32_t
    NNNPacket::GetPacketId() const
    {
      return m_packetid;
    }

    inline uint16_t
    NNNPacket::GetVersion() const
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



#endif /* NNN_PACKET_H_ */
