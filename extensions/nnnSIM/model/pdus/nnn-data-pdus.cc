/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-data-pdus.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-data-pdus.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-data-pdus.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-data-pdus.h"

namespace ns3
{
  namespace nnn
  {
    DATAPDU::DATAPDU ()
    : m_PDUdatatype (NDN_NNN)
    {
    }

    DATAPDU::~DATAPDU ()
    {
    }

    uint16_t
    DATAPDU::GetPDUPayloadType() const
    {
      return m_PDUdatatype;
    }

    void
    DATAPDU::SetPDUPayloadType (uint16_t pdu_type)
    {
      m_PDUdatatype = pdu_type;
    }

    void
    DATAPDU::SetPayload (Ptr<Packet> payload)
    {
      m_payload = payload;
      SetWire(0);
    }

    Ptr<const Packet>
    DATAPDU::GetPayload () const
    {
      return m_payload;
    }

    void
    DATAPDU::Print(std::ostream &os) const
    {
      os << "  <PDU Type>" << GetPDUPayloadType() << "</PDU Type>" << std::endl;
      if (m_payload != 0)
	os << "  <Payload>Yes</Payload>" << std::endl;
      else
	os << "  <Payload>No</Payload>" << std::endl;
    }

  } /* namespace nnn */
} /* namespace ns3 */
