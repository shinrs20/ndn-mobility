/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-do.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-do.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-do.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/unused.h>

#include "nnn-do.h"

NS_LOG_COMPONENT_DEFINE ("nnn.DO");

namespace ns3 {
  namespace nnn {

    DO::DO () : NNNPacket (DO_NNN, Seconds (0))
    , m_payload  (Create<Packet> ())
    , m_PDUdatatype (NDN_NNN)
    {
    }

    DO::DO (Ptr<NNNAddress> name, Ptr<Packet> payload)
    : NNNPacket (DO_NNN, Seconds (0))
    , m_name     (name)
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

    DO::DO (const NNNAddress &name, Ptr<Packet> payload)
    : NNNPacket (DO_NNN, Seconds(0))
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

    DO::DO (const DO &do_p)
    : NNNPacket (DO_NNN, do_p.GetLifetime ())
    , m_name     (Create<NNNAddress> (do_p.GetName()))
    , m_payload  (do_p.GetPayload ()->Copy ())
    , m_PDUdatatype (do_p.GetPDUPayloadType ())
    {
      NS_LOG_FUNCTION("DO correct copy constructor");

      SetWire (do_p.GetWire ());
    }

    const NNNAddress&
    DO::GetName () const
    {
      if (m_name == 0) throw DOException ();
      return *m_name;
    }


    Ptr<const NNNAddress>
    DO::GetNamePtr () const
    {
      return m_name;
    }

    void
    DO::SetName (Ptr<NNNAddress> name)
    {
      m_name = name;
      SetWire(0);
    }

    void
    DO::SetName (const NNNAddress &name)
    {
      m_name = Create<NNNAddress> (name);
      SetWire(0);
    }

    uint16_t
    DO::GetPDUPayloadType() const
    {
      return m_PDUdatatype;
    }

    void
    DO::SetPDUPayloadType (uint16_t pdu_type)
    {
      m_PDUdatatype = pdu_type;
    }

    void
    DO::SetPayload (Ptr<Packet> payload)
    {
      m_payload = payload;
      SetWire(0);
    }

    Ptr<const Packet>
    DO::GetPayload () const
    {
      return m_payload;
    }

    void
    DO::Print (std::ostream &os) const
    {
      os << "<DO>" << std::endl;
      os << "  <TTL>" << GetLifetime () << "</TTL>" << std::endl;
      os << "  <Version>" << GetVersion () << "</Version>" << std::endl;
      os << "  <Name>" << GetName () << "</Name>" << std::endl;
      os << "  <PDU Type>" << GetPDUPayloadType() << "</PDU Type>" << std::endl;
      if (m_payload != 0)
	{
	  os << "  <Payload>Yes</Payload>" << std::endl;
	}
      else
	{
	  os << "  <Payload>No</Payload>" << std::endl;
	}
      os << "</DO>" << std::endl;
    }

  } // namespace nnn
} // namespace ns3
