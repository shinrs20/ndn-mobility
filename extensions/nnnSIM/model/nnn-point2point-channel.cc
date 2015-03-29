/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-point2point-channel.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-point2point-channel.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-point2point-channel.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-point2point-channel.h"
#include "nnn-point2point-netdevice.h"

#include <ns3-dev/ns3/trace-source-accessor.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/log.h>

NS_LOG_COMPONENT_DEFINE ("FlexPointToPointChannel");

namespace ns3
{
  namespace nnn
  {

    NS_OBJECT_ENSURE_REGISTERED (FlexPointToPointChannel);

    TypeId
    FlexPointToPointChannel::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::FlexPointToPointChannel")
	.SetParent<Channel> ()
	.AddConstructor<FlexPointToPointChannel> ()
	.AddAttribute ("Delay", "Transmission delay through the channel",
	               TimeValue (Seconds (0)),
	               MakeTimeAccessor (&FlexPointToPointChannel::m_delay),
	               MakeTimeChecker ())
	.AddTraceSource ("TxRxPointToPoint",
	                 "Trace source indicating transmission of packet from the PointToPointChannel, used by the Animation interface.",
	                 MakeTraceSourceAccessor (&FlexPointToPointChannel::m_txrxPointToPoint))
	;
      return tid;
    }

    //
    // By default, you get a channel that
    // has an "infitely" fast transmission speed and zero delay.
    FlexPointToPointChannel::FlexPointToPointChannel()
    :
	Channel (),
	m_delay (Seconds (0.)),
	m_nDevices (0)
    {
      NS_LOG_FUNCTION_NOARGS ();
    }

    void
    FlexPointToPointChannel::Attach (Ptr<FlexPointToPointNetDevice> device)
    {
      NS_LOG_FUNCTION (this << device);
      NS_ASSERT_MSG (m_nDevices < N_DEVICES, "Only two devices permitted");
      NS_ASSERT (device != 0);

      m_link[m_nDevices++].m_src = device;
      //
      // If we have both devices connected to the channel, then finish introducing
      // the two halves and set the links to IDLE.
      //
      if (m_nDevices == N_DEVICES)
	{
	  m_link[0].m_dst = m_link[1].m_src;
	  m_link[1].m_dst = m_link[0].m_src;
	  m_link[0].m_state = IDLE;
	  m_link[1].m_state = IDLE;
	}
    }

    bool
    FlexPointToPointChannel::TransmitStart (
	Ptr<Packet> p,
	Ptr<FlexPointToPointNetDevice> src,
	Time txTime)
    {
      NS_LOG_FUNCTION (this << p << src);
      NS_LOG_LOGIC ("UID is " << p->GetUid () << ")");

      NS_ASSERT (m_link[0].m_state != INITIALIZING);
      NS_ASSERT (m_link[1].m_state != INITIALIZING);

      uint32_t wire = src == m_link[0].m_src ? 0 : 1;

      Simulator::ScheduleWithContext (m_link[wire].m_dst->GetNode ()->GetId (),
                                      txTime + m_delay, &FlexPointToPointNetDevice::Receive,
                                      m_link[wire].m_dst, p);

      // Call the tx anim callback on the net device
      m_txrxPointToPoint (GetId (), p, src, m_link[wire].m_dst, txTime, txTime + m_delay);
      return true;
    }

    uint32_t
    FlexPointToPointChannel::GetNDevices (void) const
    {
      NS_LOG_FUNCTION_NOARGS ();
      return m_nDevices;
    }

    Ptr<FlexPointToPointNetDevice>
    FlexPointToPointChannel::GetPointToPointDevice (uint32_t i) const
    {
      NS_LOG_FUNCTION_NOARGS ();
      NS_ASSERT (i < 2);
      return m_link[i].m_src;
    }

    Ptr<NetDevice>
    FlexPointToPointChannel::GetDevice (uint32_t i) const
    {
      NS_LOG_FUNCTION_NOARGS ();
      return GetPointToPointDevice (i);
    }

    Time
    FlexPointToPointChannel::GetDelay (void) const
    {
      return m_delay;
    }

    Ptr<FlexPointToPointNetDevice>
    FlexPointToPointChannel::GetSource (uint32_t i) const
    {
      return m_link[i].m_src;
    }

    Ptr<FlexPointToPointNetDevice>
    FlexPointToPointChannel::GetDestination (uint32_t i) const
    {
      return m_link[i].m_dst;
    }

    bool
    FlexPointToPointChannel::IsInitialized (void) const
    {
      NS_ASSERT (m_link[0].m_state != INITIALIZING);
      NS_ASSERT (m_link[1].m_state != INITIALIZING);
      return true;
    }
  } // namespace nnn
} // namespace ns3
