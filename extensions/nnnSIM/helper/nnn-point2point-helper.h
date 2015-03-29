/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-point2point-helper.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-point2point-helper.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-point2point-helper.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_POINT2POINT_HELPER_H_
#define NNN_POINT2POINT_HELPER_H_

#include <string>

#include <ns3-dev/ns3/object-factory.h>
#include <ns3-dev/ns3/net-device-container.h>
#include <ns3-dev/ns3/node-container.h>

#include <ns3-dev/ns3/trace-helper.h>

namespace ns3
{
  class Queue;
  class NetDevice;
  class Node;

  namespace nnn
  {

    class FlexPointToPointHelper : public ns3::PcapHelperForDevice, public ns3::AsciiTraceHelperForDevice
    {
    public:
      FlexPointToPointHelper ();

      virtual
      ~FlexPointToPointHelper ();

      void
      SetQueue (std::string type,
                     std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                     std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                     std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                     std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue ());

      void
      SetDeviceAttribute (std::string name, const AttributeValue &value);

      void
      SetChannelAttribute (std::string name, const AttributeValue &value);

      NetDeviceContainer
      Install (NodeContainer c);

      NetDeviceContainer
      Install (Ptr<Node> a, Ptr<Node> b);

      NetDeviceContainer
      Install (Ptr<Node> a, std::string bName);

      NetDeviceContainer
      Install (std::string aName, Ptr<Node> b);

      NetDeviceContainer
      Install (std::string aNode, std::string bNode);

    private:
      virtual void
      EnablePcapInternal (std::string prefix, Ptr<NetDevice> nd, bool promiscuous, bool explicitFilename);

      virtual void
      EnableAsciiInternal (
	  Ptr<OutputStreamWrapper> stream,
	  std::string prefix,
	  Ptr<NetDevice> nd,
	  bool explicitFilename);

      ObjectFactory m_queueFactory;         //!< Queue Factory
      ObjectFactory m_channelFactory;       //!< Channel Factory
      ObjectFactory m_remoteChannelFactory; //!< Remote Channel Factory
      ObjectFactory m_deviceFactory;        //!< Device Factory
    };
  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_POINT2POINT_HELPER_H_ */
