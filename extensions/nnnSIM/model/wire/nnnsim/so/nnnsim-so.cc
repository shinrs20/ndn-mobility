/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-so.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-so.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-so.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnnsim-so.h"

NNN_NAMESPACE_BEGIN

namespace wire {
  namespace nnnSIM {

    NS_OBJECT_ENSURE_REGISTERED (SO);

    NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.SO");

    SO::SO ()
    : CommonHeader<nnn::SO> ()
    {
    }

    SO::SO (Ptr<nnn::SO> so_p)
    : CommonHeader<nnn::SO> (so_p)
    {
    }

    Ptr<Packet>
    SO::ToWire (Ptr<const nnn::SO> so_p)
    {
      Ptr<const Packet> p = so_p->GetWire ();
      if (!p)
	{
	  Ptr<Packet> packet = Create<Packet> (*so_p->GetPayload ());
	  SO wireEncoding (ConstCast<nnn::SO> (so_p));
	  packet->AddHeader (wireEncoding);
	  so_p->SetWire (packet);

	  p = packet;
	}

      return p->Copy ();
    }

    Ptr<nnn::SO>
    SO::FromWire (Ptr<Packet> packet)
    {
      Ptr<nnn::SO> so_p = Create<nnn::SO> ();
      Ptr<Packet> wire = packet->Copy ();

      SO wireEncoding (so_p);
      packet->RemoveHeader (wireEncoding);

      so_p->SetPayload (packet);
      so_p->SetWire (wire);

      return so_p;
    }

    TypeId
    SO::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::SO::nnnSIM")
	  .SetGroupName ("Nnn")
	  .SetParent<Header> ()
	  .AddConstructor<SO> ()
	  ;
      return tid;
    }

    TypeId
    SO::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    uint32_t
    SO::GetSerializedSize () const
    {
      size_t size =
	  CommonGetSerializedSize () +                    /* Common header */
	  2 +                                             /* PDU Data type */
	  NnnSim::SerializedSizeName (m_ptr->GetName ())  /* Name size */
	  //m_ptr->GetPayload ()->GetSerializedSize()     /* Payload data */
	  ;
      return size;
    }

    void
    SO::Serialize (Buffer::Iterator start) const
    {
      // Serialize the header
      CommonSerialize(start);

      // Remember that CommonSerialize doesn't write the Packet length
      // Move the iterator forward
      start.Next(CommonGetSerializedSize() -2);

      NS_LOG_INFO ("Serialize -> PktID = " << m_ptr->GetPacketId());
      NS_LOG_INFO ("Serialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
      NS_LOG_INFO ("Serialize -> Version = " << m_ptr->GetVersion ());
      NS_LOG_INFO ("Serialize -> Pkt Len = " << GetSerializedSize());

      // Serialize the packet size
      start.WriteU16(GetSerializedSize());

      // Serialize the PDU Data type
      start.WriteU16(m_ptr->GetPDUPayloadType());

      // Serialize
      NnnSim::SerializeName(start, m_ptr->GetName());

      NS_LOG_INFO("Finished serialization");
    }

    uint32_t
    SO::Deserialize (Buffer::Iterator start)
    {
      Buffer::Iterator i = start;

      // Deserialize the header
      uint32_t skip = CommonDeserialize (i);

      NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
      NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
      NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
      NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

      // Check packet ID
      if (m_ptr->GetPacketId() != nnn::SO_NNN)
	throw new SOException ();

      // Move the iterator forward
      i.Next(skip);

      // Deserialize the PDU Data type
      m_ptr->SetPDUPayloadType(i.ReadU16());

      // Deserialize the name
      m_ptr->SetName(NnnSim::DeserializeName(i));

      NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

      return i.GetDistanceFrom (start);
    }
  }
}

NNN_NAMESPACE_END
