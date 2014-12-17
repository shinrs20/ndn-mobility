/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-ren.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-ren.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-ren.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnnsim-ren.h"

NNN_NAMESPACE_BEGIN

namespace wire {
  namespace nnnSIM {

    NS_OBJECT_ENSURE_REGISTERED (REN);

    NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.REN");

    REN::REN ()
    : CommonHeader<nnn::REN>()
    {
    }

    REN::REN (Ptr<nnn::REN> ren_p)
    : CommonHeader<nnn::REN>(ren_p)
    {
    }

    TypeId
    REN::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::REN::nnnSIM")
	  .SetGroupName ("Nnn")
	  .SetParent<Header> ()
	  .AddConstructor<REN> ()
	  ;
      return tid;
    }

    TypeId
    REN::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    Ptr<Packet>
    REN::ToWire (Ptr<const nnn::REN> ren_p)
    {
      Ptr<const Packet> p = ren_p->GetWire ();
      if (!p)
	{
	  // Mechanism packets have no payload, make an empty packet
	  Ptr<Packet> packet = Create<Packet> ();
	  REN wireEncoding (ConstCast<nnn::REN> (ren_p));
	  packet->AddHeader (wireEncoding);
	  ren_p->SetWire (packet);

	  p = packet;
	}
      return p->Copy ();
    }

    Ptr<nnn::REN>
    REN::FromWire (Ptr<Packet> packet)
    {
      Ptr<nnn::REN> ren_p = Create<nnn::REN> ();
      Ptr<Packet> wire = packet->Copy ();

      REN wireEncoding (ren_p);
      packet->RemoveHeader (wireEncoding);

      // Mechanism packets have no payload, make an empty packet
      ren_p->SetWire (wire);

      return ren_p;
    }

    uint32_t
    REN::GetSerializedSize (void) const
    {
      uint16_t poatype = m_ptr->GetPoaType ();
      size_t poatype_size = 0;
      size_t poa_num = 0;

      if (poatype == 0)
	{
	  poa_num = m_ptr->GetNumPoa ();
	  poatype_size = 6; // Hardcoded size of a Mac48Address
	}

      size_t size =
	  CommonGetSerializedSize() +                       /* Common header */
	  2 +                                               /* PoA Type */
	  2 +                                               /* Number of PoAs */
	  poa_num * poatype_size +                          /* Total size of PoAs */
	  2 +                                               /* Lease time */
	  NnnSim::SerializedSizeName (m_ptr->GetName ()); /* Name size */
      return size;
    }

    void
    REN::Serialize (Buffer::Iterator start) const
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

      uint16_t poatype = m_ptr->GetPoaType ();
      NS_LOG_INFO ("Serialize -> PoA Type = " << poatype);
      size_t bufsize = 0;

      if (poatype == 0)
	bufsize = 6; // Hardcoded Mac48Address size

      // Create a buffer to be able to serialize PoAs
      uint8_t buffer[bufsize];

      uint32_t totalpoas = m_ptr->GetNumPoa();
      NS_LOG_INFO ("Serialize -> PoA Num = " << totalpoas);

      // Serialize the PoA Type
      start.WriteU16(poatype);

      // Serialize the Number of PoAs
      start.WriteU16(totalpoas);

      // Go through the PoA list
      for (int i = 0; i < totalpoas; i++)
	{
	  // Use the CopyTo function to get the bit representation
	  m_ptr->GetOnePoa(i).CopyTo(buffer);

	  // Since the bit representation is in 8 bit chunks, serialize it
	  // accordingly
	  for (int j = 0; j < bufsize; j++)
	    start.WriteU8(buffer[j]);
	}

      NS_ASSERT_MSG (0 <= m_ptr->GetRemainLease ().ToInteger (Time::S) &&
		     m_ptr->GetRemainLease ().ToInteger (Time::S) < 65535,
		     "Incorrect Lease time (should not be smaller than 0 and larger than 65535");

      // Round lease time to seconds and serialize
      start.WriteU16 (static_cast<uint16_t> (m_ptr->GetRemainLease ().ToInteger (Time::S)));

      // Serialize NNN address
      NnnSim::SerializeName(start, m_ptr->GetName());

      NS_LOG_INFO("Finished serialization");
    }

    uint32_t
    REN::Deserialize (Buffer::Iterator start)
    {
      Buffer::Iterator i = start;

      // Deserialize the header
      uint32_t skip = CommonDeserialize (i);

      NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
      NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
      NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
      NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

      // Check packet ID
      if (m_ptr->GetPacketId() != nnn::REN_NNN)
	throw new RENException ();

      // Move the iterator forward
      i.Next(skip);

      uint16_t poatype = i.ReadU16 ();
      size_t bufsize = 0;

      NS_LOG_INFO ("Deserialize -> PoA Type = " << poatype);

      if (poatype == 0)
	bufsize = 6; // Hardcoded Mac48Address size

      uint16_t totalpoas = i.ReadU16 ();

      NS_LOG_INFO ("Deserialize -> PoA Num = " << totalpoas);

      // Create a buffer to be able to deserialize PoAs
      uint8_t buffer[bufsize];

      for (int k = 0; k < totalpoas; k++)
	{
	  for (int j = 0; j < bufsize; j++)
	    {
	      buffer[j] = i.ReadU8 ();
	    }

	  Address tmp = Address ();
	  tmp.CopyFrom(buffer, bufsize);

	  m_ptr->AddPoa(tmp);
	}

      // Deserialize the lease time
      m_ptr->SetRemainLease (Seconds (i.ReadU16 ()));

      // Deserialize the old name
      m_ptr->SetName(NnnSim::DeserializeName(i));

      NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

      return i.GetDistanceFrom (start);
    }
  }
}

NNN_NAMESPACE_END
