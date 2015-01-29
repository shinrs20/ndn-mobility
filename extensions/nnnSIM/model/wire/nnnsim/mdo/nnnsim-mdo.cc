/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-mdo.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-mdo.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-mdo.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnnsim-mdo.h"

namespace ns3
{
  namespace nnn
  {
    namespace wire
    {
      namespace nnnSIM
      {
	NS_OBJECT_ENSURE_REGISTERED (MDO);

	NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.MDO");

	MDO::MDO ()
	: CommonHeader<nnn::MDO> ()
	  {
	  }

	MDO::MDO (Ptr<nnn::MDO> mdo_p)
	: CommonHeader<nnn::MDO> (mdo_p)
	  {
	  }

	TypeId
	MDO::GetTypeId (void)
	{
	  static TypeId tid = TypeId ("ns3::nnn::MDO::nnnSIM")
		  .SetGroupName("Nnn")
		  .SetParent<Header> ()
		  .AddConstructor<MDO>()
		  ;
	  return tid;
	}

	TypeId
	MDO::GetInstanceTypeId (void) const
	{
	  return GetTypeId ();
	}

	Ptr<Packet>
	MDO::ToWire (Ptr<const nnn::MDO> mdo_p)
	{
	  Ptr<const Packet> p = mdo_p->GetWire ();
	  if (!p)
	    {
	      Ptr<Packet> packet = Create<Packet> (*mdo_p->GetPayload ());
	      MDO wireEncoding (ConstCast<nnn::MDO> (mdo_p));
	      packet->AddHeader (wireEncoding);
	      mdo_p->SetWire (packet);

	      p = packet;
	    }
	  return p->Copy ();
	}

	Ptr<nnn::MDO>
	MDO::FromWire (Ptr<Packet> packet)
	{
	  Ptr<nnn::MDO> mdo_p = Create<nnn::MDO> ();
	  Ptr<Packet> wire = packet->Copy ();

	  MDO wireEncoding (mdo_p);
	  packet->RemoveHeader (wireEncoding);

	  mdo_p->SetPayload (packet);
	  mdo_p->SetWire (wire);

	  return mdo_p;
	}

	uint32_t
	MDO::GetSerializedSize (void) const
	{
	  uint16_t distinctAddr = m_ptr->GetNumDistinctDestinations();
	  std::vector<Ptr<NNNAddress> > sectors = m_ptr->GetDistinctDestinations();
	  std::vector<Ptr<NNNAddress> > tmp;

	  size_t size = CommonGetSerializedSize() +            /* Common header */
	      2 +                                              /* PDU Data type */
	      2 +                                              /* Total number of addrs */
	      2                                                /* Number of sectors */
             ;

	  for (int i = 0; i < distinctAddr; i++)
	    {
	      tmp = m_ptr->GetDestinations(sectors[i]);

	      size += 2 +                                      /* Addresses for this sector */
		  NnnSim::SerializedSizeName(*sectors[i])      /* Size of the serialized sector */;

	      for (int j = 0; j < tmp.size(); j++)
		{
		  size += NnnSim::SerializedSizeName(*tmp[j])  /* Size of the serialized final addresses */
		  ;
		}
	    }

	  return size;
	}

	void
	MDO::Serialize (Buffer::Iterator start) const
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

	  // Serialize total number of addresses
	  start.WriteU16(m_ptr->GetNumTotalDestinations ());

	  uint16_t distinctAddr = m_ptr->GetNumDistinctDestinations();
	  std::vector<Ptr<NNNAddress> > sectors = m_ptr->GetDistinctDestinations();
	  std::vector<Ptr<NNNAddress> > tmp;

	  // Serialize total number of sectors
	  start.WriteU16(distinctAddr);

	  for (int i = 0; i < distinctAddr; i++)
	    {
	      tmp = m_ptr->GetDestinations(sectors[i]);

	      uint16_t sectorNum = tmp.size();

	      // Serialize number of addresses in this sector
	      start.WriteU16(sectorNum);

	      NnnSim::SerializeName(start, *sectors[i]);

	      for (int j = 0; j < sectorNum; j++)
		{
		  NnnSim::SerializeName(start, *tmp[j]);
		}
	    }

	  NS_LOG_INFO("Finished serialization");
	}

	uint32_t
	MDO::Deserialize (Buffer::Iterator start)
	{
	  Buffer::Iterator i = start;

	  // Deserialize the header
	  uint32_t skip = CommonDeserialize (i);

	  NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
	  NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
	  NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
	  NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

	  // Check packet ID
	  if (m_ptr->GetPacketId() != nnn::MDO_NNN)
	    throw new DOException ();

	  // Move the iterator forward
	  i.Next(skip);

	  // Deserialize the PDU Data type
	  m_ptr->SetPDUPayloadType(i.ReadU16());

	  // Deserialize total number of addresses
	  uint16_t totaladdr = i.ReadU16();

	  // Deserialize total number of sectors
	  uint16_t distinct = i.ReadU16();

	  Ptr<NNNAddress> sector;
	  Ptr<NNNAddress> tmp;

	  for (int k = 0; k < distinct; k++)
	    {
	      uint16_t sectorNum = i.ReadU16();

	      sector = NnnSim::DeserializeName(i);

	      for (int l = 0; l < sectorNum; l++)
		{
		  tmp = NnnSim::DeserializeName(i);

		  m_ptr->AddDestination(Create<NNNAddress> (*sector + *tmp));
		}
	    }
	  NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

	  return i.GetDistanceFrom (start);
	}

      } /* namespace nnnSIM */
    } /* namespace wire */
  } /* namespace nnn */
} /* namespace ns3 */
