/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-nullp.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-nullp.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-nullp.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnnsim-nullp.h"

NNN_NAMESPACE_BEGIN

namespace wire {
  namespace nnnSIM {

    NS_OBJECT_ENSURE_REGISTERED (NULLp);

    NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.NULLp");

    NULLp::NULLp ()
    : CommonHeader<nnn::NULLp> ()
    {
    }

    NULLp::NULLp (Ptr<nnn::NULLp> null_p)
    : CommonHeader<nnn::NULLp> (null_p)
    {
    }

    TypeId
    NULLp::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::NULLp::nnnSIM")
	  .SetGroupName ("Nnn")
	  .SetParent<Header> ()
	  .AddConstructor<NULLp> ()
	  ;
      return tid;
    }

    TypeId
    NULLp::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    Ptr<Packet>
    NULLp::ToWire (Ptr<const nnn::NULLp> null_p)
    {
      Ptr<const Packet> p = null_p->GetWire ();
      if (!p)
	{
	  Ptr<Packet> packet = Create<Packet> (*null_p->GetPayload ());
	  NULLp wireEncoding (ConstCast<nnn::NULLp> (null_p));
	  packet->AddHeader (wireEncoding);
	  null_p->SetWire (packet);

	  p = packet;
	}

      return p->Copy ();
    }

    Ptr<nnn::NULLp>
    NULLp::FromWire (Ptr<Packet> packet)
    {
      Ptr<nnn::NULLp> null_p = Create<nnn::NULLp> ();
      Ptr<Packet> wire = packet->Copy ();

      NULLp wireEncoding (null_p);
      packet->RemoveHeader (wireEncoding);

      null_p->SetPayload (packet);
      null_p->SetWire (wire);

      return null_p;
    }

    uint32_t
    NULLp::GetSerializedSize (void) const
    {
      size_t size =
	  CommonGetSerializedSize () +                     /* Common header */
	  2                                                /* PDU Data type */
	  //m_null_p->GetPayload ()->GetSerializedSize();  /* Payload data */
	  ;
      return size;
    }

    void
    NULLp::Serialize (Buffer::Iterator start) const
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

      NS_LOG_INFO("Finished serialization");
    }

    uint32_t
    NULLp::Deserialize (Buffer::Iterator start)
    {
      Buffer::Iterator i = start;

      // Deserialize the header
      uint32_t skip = CommonDeserialize (i);

      NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
      NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
      NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
      NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

      // Check packet ID
      if (m_ptr->GetPacketId() != nnn::NULL_NNN)
	throw new NULLpException ();

      // Move the iterator forward
      i.Next(skip);

      // Deserialize the PDU Data type
      m_ptr->SetPDUPayloadType(i.ReadU16());

      NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));
      return i.GetDistanceFrom (start);
    }
  }
}

NNN_NAMESPACE_END
