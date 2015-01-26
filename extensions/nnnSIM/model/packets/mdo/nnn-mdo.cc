/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-mdo.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-mdo.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-mdo.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nnn-mdo.h"

namespace ns3
{
  namespace nnn
  {

    MDO::MDO ()
     : NNNPacket (MDO_NNN, Seconds (0))
     , m_payload (Create<Packet> ())
     , m_PDUdatatype (NDN_NNN)
    {
    }

    Ptr<const Packet>
    MDO::GetPayload () const
    {
      return m_payload;
    }

    void
    MDO::SetPayload (Ptr<Packet> payload)
    {
      m_payload = payload;
      m_wire = 0;
    }

    uint16_t
    MDO::GetPDUPayloadType() const
    {
      return m_PDUdatatype;
    }

    void
    MDO::SetPDUPayloadType (uint16_t pdu_type)
    {
      m_PDUdatatype = pdu_type;
    }

    void
    MDO::Print (std::ostream &os) const
    {
      os << "<MDO>" << std::endl;
      os << "  <TTL>" << GetLifetime () << "</TTL>" << std::endl;
      os << "  <Version>" << GetVersion () << "</Version>" << std::endl;
      os << "  <PDU Type>" << GetPDUPayloadType() << "</PDU Type>" << std::endl;
      if (m_payload != 0)
	{
	  os << "  <Payload>Yes</Payload>" << std::endl;
	} else
	  {
	    os << "  <Payload>No</Payload>" << std::endl;
	  }
      os << "</MDO>" << std::endl;
    }
  } /* namespace nnn */
} /* namespace ns3 */
