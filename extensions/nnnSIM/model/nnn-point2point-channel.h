/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-point2point-channel.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-point2point-channel.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-point2point-channel.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_POINT_TO_POINT_CHANNEL_H
#define NNN_POINT_TO_POINT_CHANNEL_H

#include <list>
#include <ns3-dev/ns3/channel.h>
#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/data-rate.h>
#include <ns3-dev/ns3/traced-callback.h>

namespace ns3
{
  class Packet;

  namespace nnn
  {
    class FlexPointToPointNetDevice;

    class FlexPointToPointChannel : public Channel
    {
    public:
      static TypeId GetTypeId (void);

      FlexPointToPointChannel ();

      void
      Attach (Ptr<FlexPointToPointNetDevice> device);

      virtual bool
      TransmitStart (Ptr<Packet> p, Ptr<FlexPointToPointNetDevice> src, Time txTime);

      virtual uint32_t
      GetNDevices (void) const;

      Ptr<FlexPointToPointNetDevice>
      GetPointToPointDevice (uint32_t i) const;

      virtual Ptr<NetDevice>
      GetDevice (uint32_t i) const;

    protected:
      Time
      GetDelay (void) const;

      bool
      IsInitialized (void) const;

      Ptr<FlexPointToPointNetDevice>
      GetSource (uint32_t i) const;

      Ptr<FlexPointToPointNetDevice>
      GetDestination (uint32_t i) const;

    private:
      // Each point to point link has exactly two net devices
      static const int N_DEVICES = 2;

      Time          m_delay;
      int32_t       m_nDevices;

      TracedCallback<uint32_t,          // channel ID
      Ptr<const Packet>, // Packet being transmitted
      Ptr<NetDevice>,    // Transmitting NetDevice
      Ptr<NetDevice>,    // Receiving NetDevice
      Time,              // Amount of time to transmit the pkt
      Time               // Last bit receive time (relative to now)
      > m_txrxPointToPoint;

      enum WireState
      {
	INITIALIZING,
	IDLE,
	TRANSMITTING,
	PROPAGATING
      };

      class Link
      {
      public:
	Link() : m_state (INITIALIZING), m_src (0), m_dst (0) {}
	WireState                  m_state;
	Ptr<FlexPointToPointNetDevice> m_src;
	Ptr<FlexPointToPointNetDevice> m_dst;
      };

      Link    m_link[N_DEVICES];
    };
  } // namespace nnn
} // namespace ns3

#endif /* NNN_POINT_TO_POINT_CHANNEL_H */
