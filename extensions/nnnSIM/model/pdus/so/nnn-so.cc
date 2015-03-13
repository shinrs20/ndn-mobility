/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-so.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-so.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-so.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/unused.h>

#include "nnn-so.h"

NS_LOG_COMPONENT_DEFINE ("nnn.SO");

namespace ns3 {
  namespace nnn {

    SO::SO ()
    : NNNPDU (SO_NNN, Seconds (0))
    , m_name     (Create<NNNAddress> ())
    , m_payload  (Create<Packet> ())
    , m_PDUdatatype (NDN_NNN)
    {
    }

    SO::SO (Ptr<NNNAddress> name, Ptr<Packet> payload)
    : NNNPDU (SO_NNN, Seconds (0))
    , m_name (name)
    , m_PDUdatatype (NDN_NNN)
    {
      if (m_payload == 0)
	{
	  m_payload = Create<Packet> ();
	} else
	  {
	    m_payload = payload;
	  }
    }

    SO::SO (const NNNAddress &name, Ptr<Packet> payload)
    : NNNPDU (SO_NNN, Seconds (0))
    , m_name     (Create<NNNAddress> (name))
    , m_PDUdatatype (NDN_NNN)
    {
      if (m_payload == 0)
	{
	  m_payload = Create<Packet> ();
	} else
	  {
	    m_payload = payload;
	  }
    }

    SO::SO (const SO &so_p)
    : NNNPDU (SO_NNN, so_p.GetLifetime ())
    , m_name     (Create<NNNAddress> (so_p.GetName()))
    , m_payload  (so_p.GetPayload ()->Copy ())
    , m_PDUdatatype (so_p.GetPDUPayloadType ())
    {
      NS_LOG_FUNCTION("SO correct copy constructor");

      SetWire (so_p.GetWire ());
    }

    const NNNAddress&
    SO::GetName () const
    {
      if (m_name == 0) throw SOException ();
      return *m_name;
    }

    Ptr<const NNNAddress>
    SO::GetNamePtr () const
    {
      return m_name;
    }

    void
    SO::SetName (Ptr<NNNAddress> name)
    {
      m_name = name;
      m_wire = 0;
    }

    void
    SO::SetName (const NNNAddress &name)
    {
      m_name = Create<NNNAddress> (name);
      m_wire = 0;
    }

    Ptr<const Packet>
    SO::GetPayload () const
    {
      return m_payload;
    }

    void
    SO::SetPayload (Ptr<Packet> payload)
    {
      m_payload = payload;
      m_wire = 0;
    }

    uint16_t
    SO::GetPDUPayloadType() const
    {
      return m_PDUdatatype;
    }

    void
    SO::SetPDUPayloadType (uint16_t pdu_type)
    {
      m_PDUdatatype = pdu_type;
    }

    void
    SO::Print (std::ostream &os) const
    {
      os << "<SO>" << std::endl;
      os << "  <TTL>" << GetLifetime () << "</TTL>" << std::endl;
      os << "  <Version>" << GetVersion () << "</Version>" << std::endl;
      os << "  <Name>" << GetName () << "</Name>" << std::endl;
      os << "  <PDU Type>" << GetPDUPayloadType() << "</PDU Type>" << std::endl;
      if (m_payload != 0)
	{
	  os << "  <Payload>Yes</Payload>" << std::endl;
	} else
	  {
	    os << "  <Payload>No</Payload>" << std::endl;
	  }
      os << "</SO>" << std::endl;
    }
  } // namespace nnn
} // namespace ns3
