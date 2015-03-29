/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-ppp-header.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-ppp-header.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-ppp-header.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-ppp-header.h"

#include <iostream>
#include <ns3-dev/ns3/abort.h>
#include <ns3-dev/ns3/assert.h>
#include <ns3-dev/ns3/log.h>

namespace ns3
{
  namespace nnn
  {
    NS_LOG_COMPONENT_DEFINE ("FlexPppHeader");

    NS_OBJECT_ENSURE_REGISTERED (FlexPppHeader);

    FlexPppHeader::FlexPppHeader ()
    {
      m_protocol = 0;
      // Fill the map with the basic protocols, IP, IPv6, NDN and NNN
      m_supportedProtocols[0x0021] = "IP (0x0021)";
      m_supportedProtocols[0x0057] = "IPv6 (0x0057)";
      m_supportedProtocols[0x0077] = "NDN (0x0077)";
      m_supportedProtocols[0x0086] = "NNN (0x0086)";
    }

    FlexPppHeader::~FlexPppHeader ()
    {
    }

    TypeId
    FlexPppHeader::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::FlexPppHeader")
            .SetParent<Header> ()
            .AddConstructor<FlexPppHeader> ()
            ;
      return tid;
    }

    TypeId
    FlexPppHeader::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    void
    FlexPppHeader::Print (std::ostream &os) const
    {
      std::string proto;
      std::map<uint16_t, std::string>::iterator it;

      if (m_supportedProtocols.find(m_protocol) != m_supportedProtocols.end ())
	{
	  proto = it->second;
	}
      else
	{
	  NS_ASSERT_MSG (false, "PPP Protocol number not defined!");
	}

      os << "Flex Point-to-Point Protocol: " << proto;
    }

    uint32_t
    FlexPppHeader::GetSerializedSize (void) const
    {
      return 2;
    }

    void
    FlexPppHeader::Serialize (Buffer::Iterator start) const
    {
      start.WriteHtonU16 (m_protocol);
    }

    uint32_t
    FlexPppHeader::Deserialize (Buffer::Iterator start)
    {
      m_protocol = start.ReadNtohU16 ();
      return GetSerializedSize ();
    }

    void
    FlexPppHeader::SetProtocol (uint16_t protocol)
    {
      if (m_supportedProtocols.find(protocol) != m_supportedProtocols.end())
	{
	  m_protocol = protocol;
	}
      else
	{
	  NS_ASSERT_MSG (false, "PPP Protocol number not defined! Add Protocol first!");
	}
    }

    uint16_t
    FlexPppHeader::GetProtocol (void)
    {
      return m_protocol;
    }

    void
    FlexPppHeader::AddProtocol (uint16_t protocol, std::string description)
    {
      if (m_supportedProtocols.find(protocol) == m_supportedProtocols.end ())
	{
	  m_supportedProtocols[protocol] = description;
	}
      else
	{
	  NS_ASSERT_MSG (false, "Protocol " << protocol  << " has already been defined as " << m_supportedProtocols[protocol] << "!");
	}
    }
  } /* namespace nnn */
} /* namespace ns3 */
