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
#include "nnnsim-den.h"

NNN_NAMESPACE_BEGIN

namespace wire{
  namespace nnnSIM{

    NS_OBJECT_ENSURE_REGISTERED (DEN);

    NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.DEN");

    DEN::DEN ()
    : CommonHeader<nnn::DEN>()
    {
    }

    DEN::DEN(Ptr<nnn::DEN> den_p)
    : CommonHeader<nnn::DEN>(den_p)
    {
    }

    TypeId
    DEN::GetTypeId(void)
    {
      static TypeId tid = TypeId("ns3::nnn::DEN::nnnSIM")
	  .SetGroupName("Nnn")
	  .SetParent<Header> ()
	  .AddConstructor<DEN> ()
	  ;
      return tid;
    }

    TypeId
    DEN::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    Ptr<Packet>
    DEN::ToWire(Ptr<const nnn::DEN> den_p)
    {
      Ptr<const Packet> p = den_p->GetWire ();
      if (!p)
	{
	  // Mechanism packets have no payload, make an empty packet
	  Ptr<Packet> packet = Create<Packet> ();
	  DEN wireEncoding (ConstCast<nnn::DEN> (den_p));
	  packet-> AddHeader (wireEncoding);
	  den_p->SetWire (packet);

	  p = packet;
	}
      return p->Copy();
    }

    Ptr<nnn::DEN>
    DEN::FromWire (Ptr<Packet> packet)
    {
      Ptr<nnn::DEN> den_p = Create<nnn::DEN> ();
      Ptr<Packet> wire = packet->Copy();

      DEN wireEncoding (den_p);
      packet->RemoveHeader (wireEncoding);

      // Mechanism packets have no payload, make an empty packet
      den_p->SetWire (wire);

      return den_p;
    }

    uint32_t
    DEN::GetSerializedSize(void) const
    {
      size_t poatype_size = 0;
      size_t poa_num = 0;

      poa_num = m_ptr->GetNumPoa ();
      // Get the serialized size from the Address class
      for (int i = 0; i < poa_num; i++)
	{
	  poatype_size += m_ptr->GetOnePoa (i).GetSerializedSize();
	}

      size_t size = CommonGetSerializedSize() +         /* Common header */
	  2 +                                           /* Number of PoAs */
	  poatype_size +                                /* Total size of PoAs */
	  NnnSim::SerializedSizeName(m_ptr->GetName()); /* Name size */
      return size;
    }

    void
    DEN::Serialize(Buffer::Iterator start) const
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

      // Create a buffer to be able to serialize PoAs
      uint32_t totalpoas = m_ptr->GetNumPoa();

      NS_LOG_INFO ("Serialize -> PoA Num = " << totalpoas);

      // Serialize Number of PoAs
      start.WriteU16(totalpoas);

      // Serialize PoAs
      for (int i = 0; i < totalpoas; i++)
	{
	  Address tmpaddr = m_ptr->GetOnePoa(i);
	  uint32_t serialSize = tmpaddr.GetSerializedSize ();
	  uint8_t addrSize = tmpaddr.GetLength ();
	  uint8_t buffer[serialSize];

	  NS_LOG_INFO ("Serialize Addr: " << i);
	  NS_LOG_INFO ("Address: " << tmpaddr);
	  NS_LOG_INFO ("Serialized Length: " << serialSize);
	  NS_LOG_INFO ("Address length: " << addrSize);

	  // Use the CopyTo function to get the bit representation
	  m_ptr->GetOnePoa(i).CopyAllTo(buffer, addrSize);

	  // Since the bit representation is in 8 bit chunks, serialize it
	  // accordingly
	  for (int j = 0; j < serialSize; j++)
	    start.WriteU8(buffer[j]);
	}

      // Serialize NNN address
      NnnSim::SerializeName(start, m_ptr->GetName());

      NS_LOG_INFO("Finished serialization");
    }

    uint32_t
    DEN::Deserialize (Buffer::Iterator start)
    {
      Buffer::Iterator i = start;

      // Deserialize the header
      uint32_t skip = CommonDeserialize (i);

      NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
      NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
      NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
      NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

      // Check packet ID
      if (m_ptr->GetPacketId() != nnn::DEN_NNN)
	throw new DENException ();

      // Move the iterator forward
      i.Next(skip);

      uint16_t totalpoas = i.ReadU16 ();

      NS_LOG_INFO ("Deserialize -> PoA Num = " << totalpoas);

      for (int k = 0; k < totalpoas; k++)
	{
	  uint8_t type = i.ReadU8 ();
	  uint8_t length = i.ReadU8 ();

	  // Create a buffer to be able to deserialize PoAs
	  uint8_t buffer[length];

	  for (int j = 0; j < length; j++)
	    {
	      buffer[j] = i.ReadU8 ();
	    }

	  Address tmp = Address (type, buffer, length);

	  m_ptr->AddPoa(tmp);
	}

      // Deserialize the old name
      m_ptr->SetName(NnnSim::DeserializeName(i));

      NS_ASSERT (GetSerializedSize ()== (i.GetDistanceFrom (start)));

      return i.GetDistanceFrom (start);
    }
  }
}

NNN_NAMESPACE_END
