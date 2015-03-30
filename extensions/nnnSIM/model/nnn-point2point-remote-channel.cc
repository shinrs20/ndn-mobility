/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-point2point-remote-channel.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-point2point-remote-channel.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-point2point-remote-channel.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "nnn-point2point-remote-channel.h"
#include "nnn-point2point-netdevice.h"

#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/mpi-interface.h>

NS_LOG_COMPONENT_DEFINE ("FlexPointToPointRemoteChannel");

namespace ns3
{
  namespace nnn
  {
    NS_OBJECT_ENSURE_REGISTERED (FlexPointToPointRemoteChannel);

    TypeId
    FlexPointToPointRemoteChannel::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::FlexPointToPointRemoteChannel")
	.SetParent<FlexPointToPointChannel> ()
	.AddConstructor<FlexPointToPointRemoteChannel> ()
	;
      return tid;
    }

    FlexPointToPointRemoteChannel::FlexPointToPointRemoteChannel ()
    {
    }

    FlexPointToPointRemoteChannel::~FlexPointToPointRemoteChannel ()
    {
    }

    bool
    FlexPointToPointRemoteChannel::TransmitStart (
	Ptr<Packet> p,
	Ptr<FlexPointToPointNetDevice> src,
	Time txTime)
    {
      NS_LOG_FUNCTION (this << p << src);
      NS_LOG_LOGIC ("UID is " << p->GetUid () << ")");

      IsInitialized ();

      uint32_t wire = src == GetSource (0) ? 0 : 1;
      Ptr<FlexPointToPointNetDevice> dst = GetDestination (wire);

#ifdef NS3_MPI
      // Calculate the rxTime (absolute)
      Time rxTime = Simulator::Now () + txTime + GetDelay ();
      MpiInterface::SendPacket (p, rxTime, dst->GetNode ()->GetId (), dst->GetIfIndex ());
#else
      NS_FATAL_ERROR ("Can't use distributed simulator without MPI compiled in");
#endif
      return true;
    }

  } // namespace nnn
} // namespace ns3
