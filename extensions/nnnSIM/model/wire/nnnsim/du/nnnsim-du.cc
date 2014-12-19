/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *           Zhu Li <philispzhuli1990@ruri.waseda.jp>
 *
 *  nnnsim-den.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-den.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-den.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "nnnsim-du.h"

NNN_NAMESPACE_BEGIN

namespace wire
{
  namespace nnnSIM
  {
    NS_OBJECT_ENSURE_REGISTERED (DU);

    NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.DU");

    DU::DU ()
    : CommonHeader<nnn::DU> ()
    {
    }

    DU::DU(Ptr<nnn::DU> du_p)
    : CommonHeader<nnn::DU> (du_p)
    {
    }

    TypeId
    DU::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::DU::nnnSIM")
	  .SetGroupName ("Nnn")
	  .SetParent<Header> ()
	  .AddConstructor<DU> ()
	  ;
      return tid;
    }

    TypeId
    DU::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    Ptr<Packet>
    DU::ToWire (Ptr<const nnn::DU> du_p)
    {
      Ptr<const Packet> p = du_p->GetWire ();
      if (!p)
	{
	  Ptr<Packet> packet = Create<Packet> (*du_p->GetPayload ());
	  DU wireEncoding (ConstCast<nnn::DU> (du_p));
	  packet->AddHeader (wireEncoding);
	  du_p->SetWire (packet);

	  p = packet;
	}
      return p->Copy ();
    }

    Ptr<nnn::DU>
    DU::FromWire (Ptr<Packet> packet)
    {
      Ptr<nnn::DU> du_p = Create<nnn::DU> ();
      Ptr<Packet> wire = packet->Copy ();

      DU wireEncoding (du_p);
      packet->RemoveHeader (wireEncoding);

      du_p->SetPayload (packet);
      du_p->SetWire (wire);

      return du_p;
    }

    uint32_t
    DU::GetSerializedSize (void) const
    {
      size_t size = CommonGetSerializedSize() +                 /* Common header */
	  2 +                                                   /* PDU Data type */
	  NnnSim::SerializedSizeName (m_ptr->GetSrcName ()) +   /* Src Name size */
	  NnnSim::SerializedSizeName (m_ptr->GetDstName())      /* Dst Name size */
      //m_ptr->GetPayload ()->GetSerializedSize()        /* Payload data size */
      ;
      return size;
    }

    void
    DU::Serialize (Buffer::Iterator start) const
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

      // Serialize the source name
      NnnSim::SerializeName(start, m_ptr->GetSrcName());

      // Serialize the destination name
      NnnSim::SerializeName(start, m_ptr->GetDstName());

      NS_LOG_INFO("Finished serialization");
    }

    uint32_t
    DU::Deserialize (Buffer::Iterator start)
    {
      Buffer::Iterator i = start;

      // Deserialize the header
      uint32_t skip = CommonDeserialize (i);

      NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
      NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
      NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
      NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

      // Check packet ID
      if (m_ptr->GetPacketId() != nnn::DU_NNN)
	throw new DUException ();

      // Move the iterator forward
      i.Next(skip);

      // Deserialize the PDU Data type
      m_ptr->SetPDUPayloadType(i.ReadU16());

      // Deserialize the source name
      m_ptr->SetSrcName(NnnSim::DeserializeName(i));

      // Deserialize the destination name
      m_ptr->SetDstName(NnnSim::DeserializeName(i));

      NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

      return i.GetDistanceFrom (start);
    }

  } /* namespace nnnSIM */
} /* namespace wire */

NNN_NAMESPACE_END
