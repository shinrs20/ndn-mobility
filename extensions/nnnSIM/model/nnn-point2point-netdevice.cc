/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-point2point-netdevice.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-point2point-netdevice.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-point2point-netdevice.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-point2point-netdevice.h"

#include "nnn-point2point-channel.h"

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/queue.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/llc-snap-header.h>
#include <ns3-dev/ns3/error-model.h>
#include <ns3-dev/ns3/trace-source-accessor.h>
#include <ns3-dev/ns3/uinteger.h>
#include <ns3-dev/ns3/pointer.h>

#include <ns3-dev/ns3/ptr.h>

#include "nnn-ppp-header.h"

NS_LOG_COMPONENT_DEFINE ("FlexPointToPointNetDevice");

namespace ns3
{
  namespace nnn
  {
    NS_OBJECT_ENSURE_REGISTERED (FlexPointToPointNetDevice);

    TypeId
    FlexPointToPointNetDevice::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::FlexPointToPointNetDevice")
            .SetParent<NetDevice> ()
            .AddConstructor<FlexPointToPointNetDevice> ()
            .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                           UintegerValue (DEFAULT_MTU),
                           MakeUintegerAccessor (&FlexPointToPointNetDevice::SetMtu, &FlexPointToPointNetDevice::GetMtu),
                           MakeUintegerChecker<uint16_t> ())
            .AddAttribute ("Address", "The MAC address of this device.",
                           Mac48AddressValue (Mac48Address ("ff:ff:ff:ff:ff:ff")),
                           MakeMac48AddressAccessor (&FlexPointToPointNetDevice::m_address),
                           MakeMac48AddressChecker ())
            .AddAttribute ("DataRate", "The default data rate for point to point links",
                           DataRateValue (DataRate ("32768b/s")),
                           MakeDataRateAccessor (&FlexPointToPointNetDevice::m_bps),
                           MakeDataRateChecker ())
            .AddAttribute ("ReceiveErrorModel", "The receiver error model used to simulate packet loss",
                           PointerValue (),
                           MakePointerAccessor (&FlexPointToPointNetDevice::m_receiveErrorModel),
                           MakePointerChecker<ErrorModel> ())
            .AddAttribute ("InterframeGap", "The time to wait between packet (frame) transmissions",
                           TimeValue (Seconds (0.0)),
                           MakeTimeAccessor (&FlexPointToPointNetDevice::m_tInterframeGap),
                           MakeTimeChecker ())
            .AddAttribute ("TxQueue", "A queue to use as the transmit queue in the device.",
                           PointerValue (),
                           MakePointerAccessor (&FlexPointToPointNetDevice::m_queue),
                           MakePointerChecker<Queue> ())
            .AddTraceSource ("MacTx", "Trace source indicating a packet has arrived for transmission by this device",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_macTxTrace))
            .AddTraceSource ("MacTxDrop", "Trace source indicating a packet has been dropped by the device before transmission",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_macTxDropTrace))
            .AddTraceSource ("MacPromiscRx", "A packet has been received by this device, has been passed up from the physical layer "
                             "and is being forwarded up the local protocol stack.  This is a promiscuous trace,",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_macPromiscRxTrace))
            .AddTraceSource ("MacRx", "A packet has been received by this device, has been passed up from the physical layer "
                             "and is being forwarded up the local protocol stack.  This is a non-promiscuous trace,",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_macRxTrace))
#if 0
            // Not currently implemented for this device
            .AddTraceSource ("MacRxDrop","Trace source indicating a packet was dropped before being forwarded up the stack",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_macRxDropTrace))
#endif
            .AddTraceSource ("PhyTxBegin","Trace source indicating a packet has begun transmitting over the channel",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_phyTxBeginTrace))
            .AddTraceSource ("PhyTxEnd","Trace source indicating a packet has been completely transmitted over the channel",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_phyTxEndTrace))
            .AddTraceSource ("PhyTxDrop", "Trace source indicating a packet has been dropped by the device during transmission",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_phyTxDropTrace))
#if 0
            // Not currently implemented for this device
            .AddTraceSource ("PhyRxBegin", "Trace source indicating a packet has begun being received by the device",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_phyRxBeginTrace))
#endif
            .AddTraceSource ("PhyRxEnd","Trace source indicating a packet has been completely received by the device",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_phyRxEndTrace))
            .AddTraceSource ("PhyRxDrop", "Trace source indicating a packet has been dropped by the device during reception",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_phyRxDropTrace))
            .AddTraceSource ("Sniffer", "Trace source simulating a non-promiscuous packet sniffer attached to the device",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_snifferTrace))
            .AddTraceSource ("PromiscSniffer", "Trace source simulating a promiscuous packet sniffer attached to the device",
                             MakeTraceSourceAccessor (&FlexPointToPointNetDevice::m_promiscSnifferTrace))
      ;
      return tid;
    }


    FlexPointToPointNetDevice::FlexPointToPointNetDevice ()
    : m_txMachineState (READY)
    , m_channel (0)
    , m_linkUp (false)
    , m_currentPkt (0)
    {
      NS_LOG_FUNCTION (this);

      // Add the known PPP to Ethernet protocols we wish to use
      m_supported.insert ( mapping (0x0021, 0x0800)); // IPv4
      m_supported.insert ( mapping (0x0057, 0x86DD)); // IPv6
      m_supported.insert ( mapping (0x0077, 0x7777)); // NDN
      m_supported.insert ( mapping (0x0086, 0x7786)); // NNN
    }

    FlexPointToPointNetDevice::~FlexPointToPointNetDevice ()
    {
      NS_LOG_FUNCTION (this);
    }

    void
    FlexPointToPointNetDevice::AddHeader (Ptr<Packet> p, uint16_t protocolNumber)
    {
      NS_LOG_FUNCTION (this << p << protocolNumber);
      FlexPppHeader ppp;
      ppp.SetProtocol (EtherToPpp (protocolNumber));
      p->AddHeader (ppp);
    }

    bool
    FlexPointToPointNetDevice::ProcessHeader (Ptr<Packet> p, uint16_t& param)
    {
      NS_LOG_FUNCTION (this << p << param);
      FlexPppHeader ppp;
      p->RemoveHeader (ppp);
      param = PppToEther (ppp.GetProtocol ());
      return true;
    }

    void
    FlexPointToPointNetDevice::DoDispose ()
    {
      NS_LOG_FUNCTION (this);
      m_node = 0;
      m_channel = 0;
      m_receiveErrorModel = 0;
      m_currentPkt = 0;
      NetDevice::DoDispose ();
    }

    void
    FlexPointToPointNetDevice::SetDataRate (DataRate bps)
    {
      NS_LOG_FUNCTION (this);
      m_bps = bps;
    }

    void
    FlexPointToPointNetDevice::SetInterframeGap (Time t)
    {
      NS_LOG_FUNCTION (this << t.GetSeconds ());
      m_tInterframeGap = t;
    }

    bool
    FlexPointToPointNetDevice::TransmitStart (Ptr<Packet> p)
    {
      NS_LOG_FUNCTION (this << p);
      NS_LOG_LOGIC ("UID is " << p->GetUid () << ")");

      //
      // This function is called to start the process of transmitting a packet.
      // We need to tell the channel that we've started wiggling the wire and
      // schedule an event that will be executed when the transmission is complete.
      //
      NS_ASSERT_MSG (m_txMachineState == READY, "Must be READY to transmit");
      m_txMachineState = BUSY;
      m_currentPkt = p;
      m_phyTxBeginTrace (m_currentPkt);

      Time txTime = Seconds (m_bps.CalculateTxTime (p->GetSize ()));
      Time txCompleteTime = txTime + m_tInterframeGap;

      NS_LOG_LOGIC ("Schedule TransmitCompleteEvent in " << txCompleteTime.GetSeconds () << "sec");
      Simulator::Schedule (txCompleteTime, &FlexPointToPointNetDevice::TransmitComplete, this);

      bool result = m_channel->TransmitStart (p, this, txTime);
      if (result == false)
	{
	  m_phyTxDropTrace (p);
	}
      return result;
    }

    void
    FlexPointToPointNetDevice::TransmitComplete (void)
    {
      NS_LOG_FUNCTION (this);

      //
      // This function is called to when we're all done transmitting a packet.
      // We try and pull another packet off of the transmit queue.  If the queue
      // is empty, we are done, otherwise we need to start transmitting the
      // next packet.
      //
      NS_ASSERT_MSG (m_txMachineState == BUSY, "Must be BUSY if transmitting");
      m_txMachineState = READY;

      NS_ASSERT_MSG (m_currentPkt != 0, "PointToPointNetDevice::TransmitComplete(): m_currentPkt zero");

      m_phyTxEndTrace (m_currentPkt);
      m_currentPkt = 0;

      Ptr<Packet> p = m_queue->Dequeue ();
      if (p == 0)
	{
	  //
	  // No packet was on the queue, so we just exit.
	  //
	  return;
	}

      //
      // Got another packet off of the queue, so start the transmit process agin.
      //
      m_snifferTrace (p);
      m_promiscSnifferTrace (p);
      TransmitStart (p);
    }

    bool
    FlexPointToPointNetDevice::Attach (Ptr<FlexPointToPointChannel> ch)
    {
      NS_LOG_FUNCTION (this << &ch);

      m_channel = ch;

      m_channel->Attach (this);

      //
      // This device is up whenever it is attached to a channel.  A better plan
      // would be to have the link come up when both devices are attached, but this
      // is not done for now.
      //
      NotifyLinkUp ();
      return true;
    }

    void
    FlexPointToPointNetDevice::SetQueue (Ptr<Queue> q)
    {
      NS_LOG_FUNCTION (this << q);
      m_queue = q;
    }

    void
    FlexPointToPointNetDevice::SetReceiveErrorModel (Ptr<ErrorModel> em)
    {
      NS_LOG_FUNCTION (this << em);
      m_receiveErrorModel = em;
    }

    void
    FlexPointToPointNetDevice::Receive (Ptr<Packet> packet)
    {
      NS_LOG_FUNCTION (this << packet);
      uint16_t protocol = 0;

      if (m_receiveErrorModel && m_receiveErrorModel->IsCorrupt (packet) )
	{
	  //
	  // If we have an error model and it indicates that it is time to lose a
	  // corrupted packet, don't forward this packet up, let it go.
	  //
	  m_phyRxDropTrace (packet);
	}
      else
	{
	  //
	  // Hit the trace hooks.  All of these hooks are in the same place in this
	  // device because it is so simple, but this is not usually the case in
	  // more complicated devices.
	  //
	  m_snifferTrace (packet);
	  m_promiscSnifferTrace (packet);
	  m_phyRxEndTrace (packet);

	  //
	  // Trace sinks will expect complete packets, not packets without some of the
	  // headers.
	  //
	  Ptr<Packet> originalPacket = packet->Copy ();

	  //
	  // Strip off the point-to-point protocol header and forward this packet
	  // up the protocol stack.  Since this is a simple point-to-point link,
	  // there is no difference in what the promisc callback sees and what the
	  // normal receive callback sees.
	  //
	  ProcessHeader (packet, protocol);

	  if (!m_promiscCallback.IsNull ())
	    {
	      m_macPromiscRxTrace (originalPacket);
	      m_promiscCallback (this, packet, protocol, GetRemote (), GetAddress (), NetDevice::PACKET_HOST);
	    }

	  m_macRxTrace (originalPacket);
	  m_rxCallback (this, packet, protocol, GetRemote ());
	}
    }

    Ptr<Queue>
    FlexPointToPointNetDevice::GetQueue (void) const
    {
      NS_LOG_FUNCTION (this);
      return m_queue;
    }

    void
    FlexPointToPointNetDevice::NotifyLinkUp (void)
    {
      NS_LOG_FUNCTION (this);
      m_linkUp = true;
      m_linkChangeCallbacks ();
    }

    void
    FlexPointToPointNetDevice::SetIfIndex (const uint32_t index)
    {
      NS_LOG_FUNCTION (this);
      m_ifIndex = index;
    }

    uint32_t
    FlexPointToPointNetDevice::GetIfIndex (void) const
    {
      return m_ifIndex;
    }

    Ptr<Channel>
    FlexPointToPointNetDevice::GetChannel (void) const
    {
      return m_channel;
    }

    //
    // This is a point-to-point device, so we really don't need any kind of address
    // information.  However, the base class NetDevice wants us to define the
    // methods to get and set the address.  Rather than be rude and assert, we let
    // clients get and set the address, but simply ignore them.

    void
    FlexPointToPointNetDevice::SetAddress (Address address)
    {
      NS_LOG_FUNCTION (this << address);
      m_address = Mac48Address::ConvertFrom (address);
    }

    Address
    FlexPointToPointNetDevice::GetAddress (void) const
    {
      return m_address;
    }

    bool
    FlexPointToPointNetDevice::IsLinkUp (void) const
    {
      NS_LOG_FUNCTION (this);
      return m_linkUp;
    }

    void
    FlexPointToPointNetDevice::AddLinkChangeCallback (Callback<void> callback)
    {
      NS_LOG_FUNCTION (this);
      m_linkChangeCallbacks.ConnectWithoutContext (callback);
    }

    //
    // This is a point-to-point device, so every transmission is a broadcast to
    // all of the devices on the network.
    //
    bool
    FlexPointToPointNetDevice::IsBroadcast (void) const
    {
      NS_LOG_FUNCTION (this);
      return true;
    }

    //
    // We don't really need any addressing information since this is a
    // point-to-point device.  The base class NetDevice wants us to return a
    // broadcast address, so we make up something reasonable.
    //
    Address
    FlexPointToPointNetDevice::GetBroadcast (void) const
    {
      NS_LOG_FUNCTION (this);
      return Mac48Address ("ff:ff:ff:ff:ff:ff");
    }

    bool
    FlexPointToPointNetDevice::IsMulticast (void) const
    {
      NS_LOG_FUNCTION (this);
      return true;
    }

    Address
    FlexPointToPointNetDevice::GetMulticast (Ipv4Address multicastGroup) const
    {
      NS_LOG_FUNCTION (this);
      return Mac48Address ("01:00:5e:00:00:00");
    }

    Address
    FlexPointToPointNetDevice::GetMulticast (Ipv6Address addr) const
    {
      NS_LOG_FUNCTION (this << addr);
      return Mac48Address ("33:33:00:00:00:00");
    }

    bool
    FlexPointToPointNetDevice::IsPointToPoint (void) const
    {
      NS_LOG_FUNCTION (this);
      return true;
    }

    bool
    FlexPointToPointNetDevice::IsBridge (void) const
    {
      NS_LOG_FUNCTION (this);
      return false;
    }

    bool
    FlexPointToPointNetDevice::Send (
	Ptr<Packet> packet,
	const Address &dest,
	uint16_t protocolNumber)
    {
      NS_LOG_FUNCTION (this << packet << dest << protocolNumber);
      NS_LOG_LOGIC ("p=" << packet << ", dest=" << &dest);
      NS_LOG_LOGIC ("UID is " << packet->GetUid ());

      //
      // If IsLinkUp() is false it means there is no channel to send any packet
      // over so we just hit the drop trace on the packet and return an error.
      //
      if (IsLinkUp () == false)
	{
	  m_macTxDropTrace (packet);
	  return false;
	}

      //
      // Stick a point to point protocol header on the packet in preparation for
      // shoving it out the door.
      //
      AddHeader (packet, protocolNumber);

      m_macTxTrace (packet);

      //
      // We should enqueue and dequeue the packet to hit the tracing hooks.
      //
      if (m_queue->Enqueue (packet))
	{
	  //
	  // If the channel is ready for transition we send the packet right now
	  //
	  if (m_txMachineState == READY)
	    {
	      packet = m_queue->Dequeue ();
	      m_snifferTrace (packet);
	      m_promiscSnifferTrace (packet);
	      return TransmitStart (packet);
	    }
	  return true;
	}

      // Enqueue may fail (overflow)
      m_macTxDropTrace (packet);
      return false;
    }

    bool
    FlexPointToPointNetDevice::SendFrom (Ptr<Packet> packet,
                                     const Address &source,
                                     const Address &dest,
                                     uint16_t protocolNumber)
    {
      NS_LOG_FUNCTION (this << packet << source << dest << protocolNumber);
      return false;
    }

    void
    FlexPointToPointNetDevice::AddProtocol (uint16_t ppp_protocol, uint16_t ether_protocol)
    {
      m_supported.insert ( mapping (ppp_protocol, ether_protocol));
    }

    Ptr<Node>
    FlexPointToPointNetDevice::GetNode (void) const
    {
      return m_node;
    }

    void
    FlexPointToPointNetDevice::SetNode (Ptr<Node> node)
    {
      NS_LOG_FUNCTION (this);
      m_node = node;
    }

    bool
    FlexPointToPointNetDevice::NeedsArp (void) const
    {
      NS_LOG_FUNCTION (this);
      return false;
    }

    void
    FlexPointToPointNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
    {
      m_rxCallback = cb;
    }

    void
    FlexPointToPointNetDevice::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
    {
      m_promiscCallback = cb;
    }

    bool
    FlexPointToPointNetDevice::SupportsSendFrom (void) const
    {
      NS_LOG_FUNCTION (this);
      return false;
    }

    void
    FlexPointToPointNetDevice::DoMpiReceive (Ptr<Packet> p)
    {
      NS_LOG_FUNCTION (this << p);
      Receive (p);
    }

    Address
    FlexPointToPointNetDevice::GetRemote (void) const
    {
      NS_LOG_FUNCTION (this);
      NS_ASSERT (m_channel->GetNDevices () == 2);
      for (uint32_t i = 0; i < m_channel->GetNDevices (); ++i)
	{
	  Ptr<NetDevice> tmp = m_channel->GetDevice (i);
	  if (tmp != this)
	    {
	      return tmp->GetAddress ();
	    }
	}
      NS_ASSERT (false);
      // quiet compiler.
      return Address ();
    }

    bool
    FlexPointToPointNetDevice::SetMtu (uint16_t mtu)
    {
      NS_LOG_FUNCTION (this << mtu);
      m_mtu = mtu;
      return true;
    }

    uint16_t
    FlexPointToPointNetDevice::GetMtu (void) const
    {
      NS_LOG_FUNCTION (this);
      return m_mtu;
    }

    uint16_t
    FlexPointToPointNetDevice::PppToEther (uint16_t proto)
    {
      NS_LOG_FUNCTION_NOARGS();

      ppp2ether_bimap::left_const_iterator it;

      it = m_supported.left.find (proto);

      if (it != m_supported.left.end ())
	{
	  return it->second;
	}
      else
	{
	  NS_ASSERT_MSG (false, "PPP Protocol number not defined!");
	  return 0;
	}
    }

    uint16_t
    FlexPointToPointNetDevice::EtherToPpp (uint16_t proto)
    {
      NS_LOG_FUNCTION_NOARGS();

      ppp2ether_bimap::right_const_iterator it;

      it = m_supported.right.find (proto);

      if (it != m_supported.right.end ())
	{
	  return it->second;
	}
      else
	{
	  NS_ASSERT_MSG (false, "PPP Protocol number not defined!");
	  return 0;
	}
    }
  } /* namespace nnn */
} /* namespace ns3 */
