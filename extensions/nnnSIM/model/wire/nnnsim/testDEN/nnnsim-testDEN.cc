/*
 * nnnsim-testDEN.cc
 *
 *  Created on: Dec 16, 2014
 *      Author: jelfn
 */

#include <ns3-dev/ns3/log.h>

#include "nnnsim-testDEN.h"


namespace ns3
{
  namespace nnn
  {
    namespace wire
    {
      namespace nnnSIM
      {
	NS_OBJECT_ENSURE_REGISTERED (TestDEN);

	NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.TestDEN");

	TestDEN::TestDEN ()
	: CommonHeader<nnn::DEN>()
	  {
	  }

	TestDEN::TestDEN (Ptr<nnn::DEN> t_p)
	: CommonHeader<nnn::DEN>(t_p)
	  {
	  }

	TestDEN::~TestDEN ()
	{
	}

	TypeId
	TestDEN::GetTypeId(void)
	{
	  static TypeId tid = TypeId("ns3::nnn::DEN::nnnSIM")
		  .SetGroupName("Nnn")
		  .SetParent<Header> ()
		  .AddConstructor<TestDEN> ()
		  ;
	  return tid;
	}

	TypeId
	TestDEN::GetInstanceTypeId (void) const
	{
	  return GetTypeId ();
	}


	uint32_t
	TestDEN::GetSerializedSize(void) const
	{
	  uint16_t poatype = m_ptr->GetPoaType ();
	  size_t poatype_size = 0;
	  size_t poa_num = 0;

	  if (poatype == 0)
	    {
	      poa_num = m_ptr->GetNumPoa ();
	      poatype_size = 6; // Hardcoded size of a Mac48Address
	    }

	  size_t size = CommonGetSerializedSize() +         /* Common header */
	      2 +                                           /* PoA Type*/
	      2 +                                           /* Number of PoAs */
	      poa_num * poatype_size +                      /* Total size of PoAs */
	      NnnSim::SerializedSizeName(m_ptr->GetName()); /* Name size */
	  return size;
	}

	void
	TestDEN::Serialize(Buffer::Iterator start) const
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

	  // Serialize PoA Type
	  start.WriteU16(poatype);

	  // Serialize Number of PoAs
	  start.WriteU16(totalpoas);

	  // Serialize PoAs
	  for (int i = 0; i < totalpoas; i++)
	    {
	      // Use the CopyTo function to get the bit representation
	      m_ptr->GetOnePoa(i).CopyTo(buffer);

	      // Since the bit representation is in 8 bit chunks, serialize it
	      // accordingly
	      for (int j = 0; j < bufsize; j++)
		start.WriteU8(buffer[j]);
	    }

	  // Serialize NNN address
	  NnnSim::SerializeName(start, m_ptr->GetName());

	  NS_LOG_INFO("Finished serialization");
	}

	uint32_t
	TestDEN::Deserialize (Buffer::Iterator start)
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

	  // Deserialize the old name
	  m_ptr->SetName(NnnSim::DeserializeName(i));

	  NS_ASSERT (GetSerializedSize ()== (i.GetDistanceFrom (start)));

	  return i.GetDistanceFrom (start);
	}

	Ptr<Packet>
	TestDEN::ToWire (Ptr<const nnn::DEN> t_p)
	{
	  Ptr<const Packet> p = t_p->GetWire();
	  if (!p)
	    {
	      Ptr<Packet> packet = Create<Packet> ();
	      TestDEN wireEncoding(ConstCast<nnn::DEN> (t_p));
	      packet->AddHeader (wireEncoding);
	      t_p->SetWire (packet);

	      p = packet;
	    }
	  return p->Copy();
	}


	Ptr<nnn::DEN>
	TestDEN::FromWire (Ptr<Packet> packet)
	{
	  Ptr<nnn::DEN> t_p = Create<nnn::DEN> ();
	  Ptr<Packet> wire = packet->Copy ();

	  TestDEN wireEncoding (t_p);
	  packet->RemoveHeader(wireEncoding);

	  t_p->SetWire (wire);

	  return t_p;
	}

      } /* namespace nnnSIM */
    } /* namespace wire */
  } /* namespace nnn */
} /* namespace ns3 */
