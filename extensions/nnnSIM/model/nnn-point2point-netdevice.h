/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-point2point-netdevice.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-point2point-netdevice.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-point2point-netdevice.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_POINTTOPOINT_NETDEVICE_H_
#define NNN_POINTTOPOINT_NETDEVICE_H_

#include <cstring>
#include <ns3-dev/ns3/address.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/net-device.h>
#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/traced-callback.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/data-rate.h>
#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/mac48-address.h>
#include <ns3-dev/ns3/point-to-point-net-device.h>

#include <boost/bimap.hpp>

namespace ns3
{
  class Queue;
  class ErrorModel;

  namespace nnn
  {
    class FlexPointToPointChannel;

    class FlexPointToPointNetDevice : public ns3::NetDevice
    {
    public:

      typedef boost::bimap<uint16_t, uint16_t> ppp2ether_bimap;
      typedef ppp2ether_bimap::value_type mapping;

      static TypeId
      GetTypeId (void);

      FlexPointToPointNetDevice ();

      virtual
      ~FlexPointToPointNetDevice ();

      void
      SetDataRate (DataRate bps);

      void
      SetInterframeGap (Time t);

      bool
      Attach (Ptr<FlexPointToPointChannel> ch);

      void
      SetQueue (Ptr<Queue> queue);

      Ptr<Queue>
      GetQueue (void) const;

      void
      SetReceiveErrorModel (Ptr<ErrorModel> em);

      void
      Receive (Ptr<Packet> p);

      // The remaining methods are documented in ns3::NetDevice*

      virtual void
      SetIfIndex (const uint32_t index);

      virtual uint32_t
      GetIfIndex (void) const;

      virtual
      Ptr<Channel> GetChannel (void) const;

      virtual void
      SetAddress (Address address);

      virtual Address
      GetAddress (void) const;

      virtual bool
      SetMtu (const uint16_t mtu);

      virtual uint16_t
      GetMtu (void) const;

      virtual bool
      IsLinkUp (void) const;

      typedef void (* LinkChangeTracedCallback) (void);

      virtual void
      AddLinkChangeCallback (Callback<void> callback);

      virtual bool
      IsBroadcast (void) const;

      virtual Address
      GetBroadcast (void) const;

      virtual bool
      IsMulticast (void) const;

      virtual Address
      GetMulticast (Ipv4Address multicastGroup) const;

      virtual bool
      IsPointToPoint (void) const;

      virtual bool
      IsBridge (void) const;

      virtual bool
      Send (Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber);

      virtual bool
      SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);

      virtual Ptr<Node> GetNode (void) const;

      virtual void
      SetNode (Ptr<Node> node);

      virtual bool
      NeedsArp (void) const;

      virtual void
      SetReceiveCallback (NetDevice::ReceiveCallback cb);

      virtual Address
      GetMulticast (Ipv6Address addr) const;

      virtual void
      SetPromiscReceiveCallback (PromiscReceiveCallback cb);

      virtual bool
      SupportsSendFrom (void) const;

      void
      AddProtocol (uint16_t ppp_protocol, uint16_t ether_protocol);

    protected:

      void
      DoMpiReceive (Ptr<Packet> p);

    private:

      FlexPointToPointNetDevice&
      operator = (const FlexPointToPointNetDevice &o);

      FlexPointToPointNetDevice
      (const FlexPointToPointNetDevice &o);

      virtual
      void DoDispose (void);

    private:

      Address
      GetRemote (void) const;

      void
      AddHeader (Ptr<Packet> p, uint16_t protocolNumber);

      bool
      ProcessHeader (Ptr<Packet> p, uint16_t& param);

      bool
      TransmitStart (Ptr<Packet> p);

      void
      TransmitComplete (void);

      void
      NotifyLinkUp (void);

      enum TxMachineState
      {
	READY,   /**< The transmitter is ready to begin transmission of a packet */
	BUSY     /**< The transmitter is busy transmitting a packet */
      };

      TxMachineState m_txMachineState;

      DataRate       m_bps;

      Time           m_tInterframeGap;

      Ptr<FlexPointToPointChannel> m_channel;

      Ptr<Queue> m_queue;

      Ptr<ErrorModel> m_receiveErrorModel;

      TracedCallback<Ptr<const Packet> > m_macTxTrace;

      TracedCallback<Ptr<const Packet> > m_macTxDropTrace;

      TracedCallback<Ptr<const Packet> > m_macPromiscRxTrace;

      TracedCallback<Ptr<const Packet> > m_macRxTrace;

      TracedCallback<Ptr<const Packet> > m_macRxDropTrace;

      TracedCallback<Ptr<const Packet> > m_phyTxBeginTrace;

      TracedCallback<Ptr<const Packet> > m_phyTxEndTrace;

      TracedCallback<Ptr<const Packet> > m_phyTxDropTrace;

      TracedCallback<Ptr<const Packet> > m_phyRxBeginTrace;

      TracedCallback<Ptr<const Packet> > m_phyRxEndTrace;

      TracedCallback<Ptr<const Packet> > m_phyRxDropTrace;

      TracedCallback<Ptr<const Packet> > m_snifferTrace;

      TracedCallback<Ptr<const Packet> > m_promiscSnifferTrace;

      Ptr<Node> m_node;         //!< Node owning this NetDevice
      Mac48Address m_address;   //!< Mac48Address of this NetDevice
      NetDevice::ReceiveCallback m_rxCallback;   //!< Receive callback
      NetDevice::PromiscReceiveCallback m_promiscCallback;  //!< Receive callback
      //   (promisc data)
      uint32_t m_ifIndex; //!< Index of the interface
      bool m_linkUp;      //!< Identify if the link is up or not
      TracedCallback<> m_linkChangeCallbacks;  //!< Callback for the link change event

      static const uint16_t DEFAULT_MTU = 1500; //!< Default MTU

      uint32_t m_mtu;

      Ptr<Packet> m_currentPkt; //!< Current packet processed

      ppp2ether_bimap m_supported; //!< List of currently supported protocols of type boost::bimap PPP - Ether

      uint16_t PppToEther (uint16_t protocol);

      uint16_t EtherToPpp (uint16_t protocol);
    };
  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_POINTTOPOINT_NETDEVICE_H_ */
