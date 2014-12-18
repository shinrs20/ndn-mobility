/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-do.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-do.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-do.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnnsim-do.h"

NNN_NAMESPACE_BEGIN

namespace wire {
  namespace nnnSIM {

    NS_OBJECT_ENSURE_REGISTERED (DO);

    NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.DO");

    DO::DO ()
    : CommonHeader<nnn::DO> ()
    {
    }

    DO::DO (Ptr<nnn::DO> do_p)
    : CommonHeader<nnn::DO> (do_p)
    {
    }

    TypeId
    DO::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::DO::nnnSIM")
	  .SetGroupName ("Nnn")
	  .SetParent<Header> ()
	  .AddConstructor<DO> ()
	  ;
      return tid;
    }

    TypeId
    DO::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    Ptr<Packet>
    DO::ToWire (Ptr<const nnn::DO> do_p)
    {
      Ptr<const Packet> p = do_p->GetWire ();
      if (!p)
	{
	  Ptr<Packet> packet = Create<Packet> (*do_p->GetPayload ());
	  DO wireEncoding (ConstCast<nnn::DO> (do_p));
	  packet->AddHeader (wireEncoding);
	  do_p->SetWire (packet);

	  p = packet;
	}
      return p->Copy ();
    }

    Ptr<nnn::DO>
    DO::FromWire (Ptr<Packet> packet)
    {
      Ptr<nnn::DO> do_p = Create<nnn::DO> ();
      Ptr<Packet> wire = packet->Copy ();

      DO wireEncoding (do_p);
      packet->RemoveHeader (wireEncoding);

      do_p->SetPayload (packet);
      do_p->SetWire (wire);

      return do_p;
    }

    uint32_t
    DO::GetSerializedSize (void) const
    {
      size_t size = CommonGetSerializedSize() +            /* Common header */
	  2 +                                              /* PDU Data type */
	  NnnSim::SerializedSizeName (m_ptr->GetName ())   /* Name size */
	  //m_ptr->GetPayload ()->GetSerializedSize()        /* Payload data size */
	  ;
      return size;
    }

    void
    DO::Serialize (Buffer::Iterator start) const
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
    DO::Deserialize (Buffer::Iterator start)
    {
      Buffer::Iterator i = start;

      // Deserialize the header
      uint32_t skip = CommonDeserialize (i);

      NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
      NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
      NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
      NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

      // Check packet ID
      if (m_ptr->GetPacketId() != nnn::DO_NNN)
	throw new DOException ();

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
