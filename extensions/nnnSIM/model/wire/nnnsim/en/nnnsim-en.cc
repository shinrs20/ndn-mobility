/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-en.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-en.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-en.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnnsim-en.h"

NNN_NAMESPACE_BEGIN

namespace wire {
  namespace nnnSIM {

    NS_OBJECT_ENSURE_REGISTERED (EN);

    NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.EN");

    EN::EN ()
    : CommonHeader<nnn::EN> ()
    {
    }

    EN::EN (Ptr<nnn::EN> en_p)
    : CommonHeader<nnn::EN> (en_p)
    {
    }

    TypeId
    EN::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::EN::nnnSIM")
	  .SetGroupName ("Nnn")
	  .SetParent<Header> ()
	  .AddConstructor<EN> ()
	  ;
      return tid;
    }

    TypeId
    EN::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    Ptr<Packet>
    EN::ToWire (Ptr<const nnn::EN> en_p)
    {
      Ptr<const Packet> p = en_p->GetWire ();
      if (!p)
	{
	  // Mechanism packets have no payload, make an empty packet
	  Ptr<Packet> packet = Create<Packet> ();
	  EN wireEncoding (ConstCast<nnn::EN> (en_p));
	  packet->AddHeader (wireEncoding);
	  en_p->SetWire (packet);

	  p = packet;
	}
      return p->Copy ();
    }

    Ptr<nnn::EN>
    EN::FromWire (Ptr<Packet> packet)
    {
      Ptr<nnn::EN> en_p = Create<nnn::EN> ();
      Ptr<Packet> wire = packet->Copy ();

      EN wireEncoding (en_p);
      packet->RemoveHeader (wireEncoding);

      // Mechanism packets have no payload, make an empty packet
      en_p->SetWire (wire);

      return en_p;
    }

    uint32_t
    EN::GetSerializedSize (void) const
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
	  CommonGetSerializedSize() +         /* Common header */
	  2 +                                 /* PoA Type*/
	  2 +                                 /* Number of PoAs */
	  poa_num * poatype_size              /* Total size of PoAs */
	  ;
      return size;
    }

    void
    EN::Serialize (Buffer::Iterator start) const
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
      NS_LOG_INFO("Finished serialization");
    }

    uint32_t
    EN::Deserialize (Buffer::Iterator start)
    {
      Buffer::Iterator i = start;

      // Deserialize the header
      uint32_t skip = CommonDeserialize (i);

      NS_LOG_INFO ("Deserialize -> PktID = " << m_ptr->GetPacketId());
      NS_LOG_INFO ("Deserialize -> TTL = " << Seconds(static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))));
      NS_LOG_INFO ("Deserialize -> Version = " << m_ptr->GetVersion ());
      NS_LOG_INFO ("Deserialize -> Pkt len = " << m_packet_len);

      // Check packet ID
      if (m_ptr->GetPacketId() != nnn::EN_NNN)
	throw new ENException ();

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

      NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

      return i.GetDistanceFrom (start);
    }
  }
}

NNN_NAMESPACE_END
