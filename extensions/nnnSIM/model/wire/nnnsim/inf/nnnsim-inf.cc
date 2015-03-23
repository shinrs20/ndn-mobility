/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-inf.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-inf.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-inf.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnnsim-inf.h"

NNN_NAMESPACE_BEGIN

namespace wire {
  namespace nnnSIM {

    NS_OBJECT_ENSURE_REGISTERED (INF);

    NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.INF");

    INF::INF ()
    : CommonHeader<nnn::INF> ()
    {
    }

    INF::INF (Ptr<nnn::INF> inf_p)
    : CommonHeader<nnn::INF>(inf_p)
    {
    }

    TypeId
    INF::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::INF::nnnSIM")
	  .SetGroupName ("Nnn")
	  .SetParent<Header> ()
	  .AddConstructor<INF> ()
	  ;
      return tid;
    }

    TypeId
    INF::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    Ptr<Packet>
    INF::ToWire (Ptr<const nnn::INF> inf_p)
    {
      Ptr<const Packet> p = inf_p->GetWire ();
      if (!p)
	{
	  // Mechanism packets have no payload, make an empty packet
	  Ptr<Packet> packet = Create<Packet> ();
	  INF wireEncoding (ConstCast<nnn::INF> (inf_p));
	  packet->AddHeader (wireEncoding);
	  inf_p->SetWire (packet);

	  p = packet;
	}
      return p->Copy ();
    }

    Ptr<nnn::INF>
    INF::FromWire (Ptr<Packet> packet)
    {
      Ptr<nnn::INF> inf_p = Create<nnn::INF> ();
      Ptr<Packet> wire = packet->Copy ();

      INF wireEncoding (inf_p);
      packet->RemoveHeader (wireEncoding);

      // Mechanism packets have no payload, make an empty packet
      inf_p->SetWire (wire);

      return inf_p;
    }

    uint32_t
    INF::GetSerializedSize (void) const
    {
      size_t size =
	  CommonGetSerializedSize() +                         /* Common header */
	  8 +                                                 /* re Lease time */
	  NnnSim::SerializedSizeName (m_ptr->GetOldName ()) + /* Name size */
	  NnnSim::SerializedSizeName (m_ptr->GetNewName ());  /* Name size */
      return size;
    }

    void
    INF::Serialize (Buffer::Iterator start) const
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

      uint64_t lease = static_cast<uint64_t> (m_ptr->GetRemainLease ().ToInteger (Time::S));

      NS_ASSERT_MSG (0 <= lease &&
		     lease < 0x7fffffffffffffffLL,
		     "Incorrect Lease time (should not be smaller than 0 and larger than UINT64_MAX");

      // Round lease time to seconds and serialize
      start.WriteU64 (lease);

      // Serialize NNN address
      NnnSim::SerializeName(start, m_ptr->GetOldName());

      // Serialize NNN address
      NnnSim::SerializeName(start, m_ptr->GetNewName());
      NS_LOG_INFO("Finished serialization");
    }

    uint32_t
    INF::Deserialize (Buffer::Iterator start)
    {
      Buffer::Iterator i = start;

      // Deserialize the header
      uint32_t skip = CommonDeserialize (i);

      NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
      NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
      NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
      NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

      // Check packet ID
      if (m_ptr->GetPacketId() != nnn::INF_NNN)
	throw new INFException ();

      // Move the iterator forward
      i.Next(skip);

      uint64_t lease = i.ReadU64 ();

      NS_LOG_INFO ("Deserialize -> = Lease time " << lease);

      // Read the PoA Type
      m_ptr->SetRemainLease (Seconds (lease));

      // Deserialize the old name
      m_ptr->SetOldName(NnnSim::DeserializeName(i));

      // Deserialize the new name
      m_ptr->SetNewName(NnnSim::DeserializeName(i));

      NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

      return i.GetDistanceFrom (start);
    }
  }
}

NNN_NAMESPACE_END
