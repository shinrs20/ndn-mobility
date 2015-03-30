/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-point2point-helper.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-point2point-helper.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-point2point-helper.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-point2point-helper.h"
#include "../model/nnn-point2point-netdevice.h"
#include "../model/nnn-point2point-channel.h"
#include "../model/nnn-point2point-remote-channel.h"

#include <ns3-dev/ns3/abort.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/simulator.h>

#include <ns3-dev/ns3/queue.h>
#include <ns3-dev/ns3/config.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/names.h>
#include <ns3-dev/ns3/mpi-interface.h>
#include <ns3-dev/ns3/mpi-receiver.h>

namespace ns3
{
  namespace nnn
  {
    NS_LOG_COMPONENT_DEFINE ("FlexPointToPointHelper");

    FlexPointToPointHelper::FlexPointToPointHelper ()
    {
      m_queueFactory.SetTypeId ("ns3::DropTailQueue");
      m_deviceFactory.SetTypeId ("ns3::nnn::FlexPointToPointNetDevice");
      m_channelFactory.SetTypeId ("ns3::nnn::FlexPointToPointChannel");
      m_remoteChannelFactory.SetTypeId ("ns3::nnn::FlexPointToPointRemoteChannel");
    }

    FlexPointToPointHelper::~FlexPointToPointHelper ()
    {
    }

    void
    FlexPointToPointHelper::SetQueue (std::string type,
                                  std::string n1, const AttributeValue &v1,
                                  std::string n2, const AttributeValue &v2,
                                  std::string n3, const AttributeValue &v3,
                                  std::string n4, const AttributeValue &v4)
    {
      m_queueFactory.SetTypeId (type);
      m_queueFactory.Set (n1, v1);
      m_queueFactory.Set (n2, v2);
      m_queueFactory.Set (n3, v3);
      m_queueFactory.Set (n4, v4);
    }

    void
    FlexPointToPointHelper::SetDeviceAttribute (std::string n1, const AttributeValue &v1)
    {
      m_deviceFactory.Set (n1, v1);
    }

    void
    FlexPointToPointHelper::SetChannelAttribute (std::string n1, const AttributeValue &v1)
    {
      m_channelFactory.Set (n1, v1);
      m_remoteChannelFactory.Set (n1, v1);
    }

    void
    FlexPointToPointHelper::EnablePcapInternal (std::string prefix, Ptr<NetDevice> nd, bool promiscuous, bool explicitFilename)
    {
      //
      // All of the Pcap enable functions vector through here including the ones
      // that are wandering through all of devices on perhaps all of the nodes in
      // the system.  We can only deal with devices of type PointToPointNetDevice.
      //
      Ptr<FlexPointToPointNetDevice> device = nd->GetObject<FlexPointToPointNetDevice> ();
      if (device == 0)
	{
	  NS_LOG_INFO ("FlexPointToPointHelper::EnablePcapInternal(): Device " << device << " not of type ns3::PointToPointNetDevice");
	  return;
	}

      PcapHelper pcapHelper;

      std::string filename;
      if (explicitFilename)
	{
	  filename = prefix;
	}
      else
	{
	  filename = pcapHelper.GetFilenameFromDevice (prefix, device);
	}

      Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out,
                                                         PcapHelper::DLT_PPP);
      pcapHelper.HookDefaultSink<FlexPointToPointNetDevice> (device, "PromiscSniffer", file);
    }

    void
    FlexPointToPointHelper::EnableAsciiInternal (
	Ptr<OutputStreamWrapper> stream,
	std::string prefix,
	Ptr<NetDevice> nd,
	bool explicitFilename)
    {
      //
      // All of the ascii enable functions vector through here including the ones
      // that are wandering through all of devices on perhaps all of the nodes in
      // the system.  We can only deal with devices of type PointToPointNetDevice.
      //
      Ptr<FlexPointToPointNetDevice> device = nd->GetObject<FlexPointToPointNetDevice> ();
      if (device == 0)
	{
	  NS_LOG_INFO ("FlexPointToPointHelper::EnableAsciiInternal(): Device " << device <<
	               " not of type ns3::PointToPointNetDevice");
	  return;
	}

      //
      // Our default trace sinks are going to use packet printing, so we have to
      // make sure that is turned on.
      //
      Packet::EnablePrinting ();

      //
      // If we are not provided an OutputStreamWrapper, we are expected to create
      // one using the usual trace filename conventions and do a Hook*WithoutContext
      // since there will be one file per context and therefore the context would
      // be redundant.
      //
      if (stream == 0)
	{
	  //
	  // Set up an output stream object to deal with private ofstream copy
	  // constructor and lifetime issues.  Let the helper decide the actual
	  // name of the file given the prefix.
	  //
	  AsciiTraceHelper asciiTraceHelper;

	  std::string filename;
	  if (explicitFilename)
	    {
	      filename = prefix;
	    }
	  else
	    {
	      filename = asciiTraceHelper.GetFilenameFromDevice (prefix, device);
	    }

	  Ptr<OutputStreamWrapper> theStream = asciiTraceHelper.CreateFileStream (filename);

	  //
	  // The MacRx trace source provides our "r" event.
	  //
	  asciiTraceHelper.HookDefaultReceiveSinkWithoutContext<FlexPointToPointNetDevice> (device, "MacRx", theStream);

	  //
	  // The "+", '-', and 'd' events are driven by trace sources actually in the
	  // transmit queue.
	  //
	  Ptr<Queue> queue = device->GetQueue ();
	  asciiTraceHelper.HookDefaultEnqueueSinkWithoutContext<Queue> (queue, "Enqueue", theStream);
	  asciiTraceHelper.HookDefaultDropSinkWithoutContext<Queue> (queue, "Drop", theStream);
	  asciiTraceHelper.HookDefaultDequeueSinkWithoutContext<Queue> (queue, "Dequeue", theStream);

	  // PhyRxDrop trace source for "d" event
	  asciiTraceHelper.HookDefaultDropSinkWithoutContext<FlexPointToPointNetDevice> (device, "PhyRxDrop", theStream);

	  return;
	}

      //
      // If we are provided an OutputStreamWrapper, we are expected to use it, and
      // to providd a context.  We are free to come up with our own context if we
      // want, and use the AsciiTraceHelper Hook*WithContext functions, but for
      // compatibility and simplicity, we just use Config::Connect and let it deal
      // with the context.
      //
      // Note that we are going to use the default trace sinks provided by the
      // ascii trace helper.  There is actually no AsciiTraceHelper in sight here,
      // but the default trace sinks are actually publicly available static
      // functions that are always there waiting for just such a case.
      //
      uint32_t nodeid = nd->GetNode ()->GetId ();
      uint32_t deviceid = nd->GetIfIndex ();
      std::ostringstream oss;

      oss << "/NodeList/" << nd->GetNode ()->GetId () << "/DeviceList/" << deviceid << "/$ns3::nnn::FlexPointToPointNetDevice/MacRx";
      Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultReceiveSinkWithContext, stream));

      oss.str ("");
      oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::nnn::FlexPointToPointNetDevice/TxQueue/Enqueue";
      Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultEnqueueSinkWithContext, stream));

      oss.str ("");
      oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::nnn::FlexPointToPointNetDevice/TxQueue/Dequeue";
      Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultDequeueSinkWithContext, stream));

      oss.str ("");
      oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::nnn::FlexPointToPointNetDevice/TxQueue/Drop";
      Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultDropSinkWithContext, stream));

      oss.str ("");
      oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::nnn::FlexPointToPointNetDevice/PhyRxDrop";
      Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultDropSinkWithContext, stream));
    }

    NetDeviceContainer
    FlexPointToPointHelper::Install (NodeContainer c)
    {
      NS_ASSERT (c.GetN () == 2);
      return Install (c.Get (0), c.Get (1));
    }

    NetDeviceContainer
    FlexPointToPointHelper::Install (Ptr<Node> a, Ptr<Node> b)
    {
      NetDeviceContainer container;

      Ptr<FlexPointToPointNetDevice> devA = m_deviceFactory.Create<FlexPointToPointNetDevice> ();
      devA->SetAddress (Mac48Address::Allocate ());
      a->AddDevice (devA);
      Ptr<Queue> queueA = m_queueFactory.Create<Queue> ();
      devA->SetQueue (queueA);
      Ptr<FlexPointToPointNetDevice> devB = m_deviceFactory.Create<FlexPointToPointNetDevice> ();
      devB->SetAddress (Mac48Address::Allocate ());
      b->AddDevice (devB);
      Ptr<Queue> queueB = m_queueFactory.Create<Queue> ();
      devB->SetQueue (queueB);
      // If MPI is enabled, we need to see if both nodes have the same system id
      // (rank), and the rank is the same as this instance.  If both are true,
      //use a normal p2p channel, otherwise use a remote channel
      bool useNormalChannel = true;
      Ptr<FlexPointToPointChannel> channel = 0;

      if (MpiInterface::IsEnabled ())
	{
	  uint32_t n1SystemId = a->GetSystemId ();
	  uint32_t n2SystemId = b->GetSystemId ();
	  uint32_t currSystemId = MpiInterface::GetSystemId ();
	  if (n1SystemId != currSystemId || n2SystemId != currSystemId)
	    {
	      useNormalChannel = false;
	    }
	}
      if (useNormalChannel)
	{
	  channel = m_channelFactory.Create<FlexPointToPointChannel> ();
	}
      else
	{
	  channel = m_remoteChannelFactory.Create<FlexPointToPointRemoteChannel> ();
	  Ptr<MpiReceiver> mpiRecA = CreateObject<MpiReceiver> ();
	  Ptr<MpiReceiver> mpiRecB = CreateObject<MpiReceiver> ();
	  mpiRecA->SetReceiveCallback (MakeCallback (&FlexPointToPointNetDevice::Receive, devA));
	  mpiRecB->SetReceiveCallback (MakeCallback (&FlexPointToPointNetDevice::Receive, devB));
	  devA->AggregateObject (mpiRecA);
	  devB->AggregateObject (mpiRecB);
	}

      devA->Attach (channel);
      devB->Attach (channel);
      container.Add (devA);
      container.Add (devB);

      return container;
    }

    NetDeviceContainer
    FlexPointToPointHelper::Install (Ptr<Node> a, std::string bName)
    {
      Ptr<Node> b = Names::Find<Node> (bName);
      return Install (a, b);
    }

    NetDeviceContainer
    FlexPointToPointHelper::Install (std::string aName, Ptr<Node> b)
    {
      Ptr<Node> a = Names::Find<Node> (aName);
      return Install (a, b);
    }

    NetDeviceContainer
    FlexPointToPointHelper::Install (std::string aName, std::string bName)
    {
      Ptr<Node> a = Names::Find<Node> (aName);
      Ptr<Node> b = Names::Find<Node> (bName);
      return Install (a, b);
    }
  } /* namespace nnn */
} /* namespace ns3 */
