/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *           Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnnsim-oen.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-oen.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-oen.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "nnnsim-oen.h"

namespace ns3
{
  namespace nnn
  {
    namespace wire
    {
      namespace nnnSIM
      {
	NS_OBJECT_ENSURE_REGISTERED (OEN);

	NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.OEN");

	OEN::OEN ()
	: CommonHeader<nnn::OEN> ()
	{
	}

	OEN::OEN(Ptr<nnn::OEN> oen_p)
	: CommonHeader<nnn::OEN> (oen_p)
	  {
	  }

	TypeId
	OEN::GetTypeId(void)
	{
	  static TypeId tid = TypeId("ns3::nnn::OEN::nnnSIM")
		      .SetGroupName("Nnn")
		      .SetParent<Header> ()
		      .AddConstructor<OEN> ()
		      ;
	  return tid;
	}

	TypeId
	OEN::GetInstanceTypeId (void) const
	{
	  return GetTypeId ();
	}

	Ptr<Packet>
	OEN::ToWire(Ptr<const nnn::OEN> oen_p)
	{
	  Ptr<const Packet> p = oen_p->GetWire ();
	  if (!p)
	    {
	      // Mechanism packets have no payload, make an empty packet
	      Ptr<Packet> packet = Create<Packet> ();
	      OEN wireEncoding (ConstCast<nnn::OEN> (oen_p));
	      packet-> AddHeader (wireEncoding);
	      oen_p->SetWire (packet);

	      p = packet;
	    }
	  return p->Copy();
	}

	Ptr<nnn::OEN>
	OEN::FromWire (Ptr<Packet> packet)
	{
	  Ptr<nnn::OEN> oen_p = Create<nnn::OEN> ();
	  Ptr<Packet> wire = packet->Copy();

	  OEN wireEncoding (oen_p);
	  packet->RemoveHeader (wireEncoding);

	  // Mechanism packets have no payload, make an empty packet
	  oen_p->SetWire (wire);

	  return oen_p;
	}

	uint32_t
	OEN::GetSerializedSize(void) const
	{
	  size_t poatype_size = 0;
	  size_t poa_num = 0;
	  size_t poa_num2 = 0;
	  size_t poatype_size2 = 0;

	  poa_num = m_ptr->GetNumPoa ();
	  // Get the serialized size from the Address class
	  for (int i = 0; i < poa_num; i++)
	    {
	      poatype_size += m_ptr->GetOnePoa (i).GetSerializedSize ();
	    }

	  poa_num2 = m_ptr->GetPersonalNumPoa ();
	  // Get the serialized size from the Address class
	  for (int i = 0; i < poa_num2; i++)
	    {
	      poatype_size += m_ptr->GetPersonalOnePoa (i).GetSerializedSize ();
	    }

	  size_t size = CommonGetSerializedSize() +             /* Common header */
	      2 +                                               /* Number of PoAs */
	      poatype_size +                                    /* Total size of PoAs */
	      8 +                                               /* Lease time */
	      NnnSim::SerializedSizeName(m_ptr->GetName ()) +   /* Name size */
	      2 +                                               /* Number of PoAs */
	      poatype_size2 +                                   /* Total size of PoAs */
	      NnnSim::SerializedSizeName(m_ptr->GetSrcName ())
	  ;
	  return size;
	}

	void
	OEN::Serialize(Buffer::Iterator start) const
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

	  uint32_t totalpoas = m_ptr->GetNumPoa();

	  NS_LOG_INFO ("Serialize -> PoA Num = " << totalpoas);

	  // Serialize Number of PoAs
	  start.WriteU16(totalpoas);

	  // Serialize PoAs
	  for (int i = 0; i < totalpoas; i++)
	    {
	      Address tmpaddr = m_ptr->GetOnePoa (i);
	      uint32_t serialSize = tmpaddr.GetSerializedSize ();
	      uint8_t addrSize = tmpaddr.GetLength ();
	      uint8_t buffer[serialSize];

	      // Use the CopyTo function to get the bit representation
	      m_ptr->GetOnePoa (i).CopyAllTo (buffer, addrSize);

	      // Since the bit representation is in 8 bit chunks, serialize it
	      // accordingly
	      for (int j = 0; j < serialSize; j++)
		start.WriteU8 (buffer[j]);
	    }

	  uint64_t lease = static_cast<uint64_t> (m_ptr->GetLeasetime ().ToInteger (Time::S));

	  NS_ASSERT_MSG (0 <= lease &&
			 lease < 0x7fffffffffffffffLL,
			 "Incorrect Lease time (should not be smaller than 0 and larger than UINT64_MAX");

	  // Round lease time to lease and serialize
	  start.WriteU64 (lease);
	  NS_LOG_INFO ("Serialize -> Lease time = " << lease);

	  // Serialize NNN address
	  NnnSim::SerializeName(start, m_ptr->GetName());

	  uint32_t totalpersonalpoas = m_ptr->GetPersonalNumPoa ();

	  NS_LOG_INFO ("Serialize -> Personal PoA Num = " << totalpersonalpoas);

	  // Serialize Number of PoAs
	  start.WriteU16(totalpersonalpoas);

	  // Serialize personal PoAs
	  for (int i = 0; i < totalpersonalpoas; i++)
	    {
	      Address tmpaddr = m_ptr->GetPersonalOnePoa (i);
	      uint32_t serialSize = tmpaddr.GetSerializedSize ();
	      uint8_t addrSize = tmpaddr.GetLength ();
	      uint8_t buffer[serialSize];

	      // Use the CopyTo function to get the bit representation
	      m_ptr->GetPersonalOnePoa(i).CopyAllTo (buffer, addrSize);

	      // Since the bit representation is in 8 bit chunks, serialize it
	      // accordingly
	      for (int j = 0; j < serialSize; j++)
		start.WriteU8(buffer[j]);
	    }

	  // Serialize NNN address
	  NnnSim::SerializeName(start, m_ptr->GetSrcName());

	  NS_LOG_INFO("Finished serialization wrote " << GetSerializedSize ());
	}

	uint32_t
	OEN::Deserialize (Buffer::Iterator start)
	{
	  Buffer::Iterator i = start;

	  // Deserialize the header
	  uint32_t skip = CommonDeserialize (i);

	  NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
	  NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
	  NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
	  NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

	  // Check packet ID
	  if (m_ptr->GetPacketId() != nnn::OEN_NNN)
	    throw new OENException ();

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

	  uint64_t lease = i.ReadU64 ();

	  NS_LOG_INFO ("Deserialize ->  = Lease time " << lease);

	  // Deserialize and set the lease time
	  m_ptr->SetLeasetime (Seconds (lease));

	  // Deserialize the old name
	  m_ptr->SetName(NnnSim::DeserializeName(i));

	  uint16_t personal_totalpoas = i.ReadU16 ();

	  NS_LOG_INFO ("Deserialize -> Personal PoA Num = " << personal_totalpoas);

	  for (int k = 0; k < personal_totalpoas; k++)
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

	      m_ptr->AddPersonalPoa (tmp);
	    }

	  // Deserialize the src name
	  m_ptr->SetSrcName (NnnSim::DeserializeName(i));

	  NS_ASSERT (GetSerializedSize ()== (i.GetDistanceFrom (start)));

	  return i.GetDistanceFrom (start);
	}

      } /* namespace nnnSIM */
    } /* namespace wire */
  } /* namespace nnn */
} /* namespace ns3 */
