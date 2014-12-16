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
    : m_den_p (Create<nnn::DEN>())
    {
    }

    DEN::DEN(Ptr<nnn::DEN> den_p)
    : m_den_p (den_p)
    {
    }

    Ptr<nnn::DEN>
    DEN::GetDEN()
    {
      return m_den_p;
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
      size_t size =
	  4 +									/* Packetid */
	  2 +									/* Length of packet */
	  2 +									/* Timestamp */
	  NnnSim::SerializedSizeName(m_den_p->GetName()); /* Name size */

      NS_LOG_INFO ("Serialize size = " << size);
      return size;
    }

    void
    DEN::Serialize(Buffer::Iterator start) const
    {
      // Get the total size of the serialized packet
      uint32_t totalsize = GetSerializedSize();

      //Serialize packetid
      start.WriteU32(m_den_p->GetPacketId());

      // Get the length of the packet
      start.WriteU16(totalsize - 4); // Minus packetid size of 32 bits

      // Check that the lifetime is within the limits
      NS_ASSERT_MSG (0 <= m_den_p->GetLifetime().ToInteger (Time::S) &&
                     m_den_p->GetLifetime().ToInteger(Time::S) < 65535,
                     "Incorrect Lifetime (should not be smaller than 0 and larger than 65535");

      // Round lifetime to seconds
      start.WriteU16 (static_cast<uint16_t> (m_den_p->GetLifetime().ToInteger(Time::S)));

      // Serialize NNN address
      NnnSim::SerializeName(start, m_den_p->GetName());
    }

    uint32_t
    DEN::Deserialize (Buffer::Iterator start)
    {
      Buffer::Iterator i = start;

      // Read packet id
      if (i.ReadU32() != 6)
	throw new DENException ();

      // Read length of packet
      uint16_t pakcet_len = i.ReadU16();

      // Read lifetime of the packet
      m_den_p->SetLifetime (Seconds (i.ReadU16()));

      NS_ASSERT (GetSerializedSize ()== (i.GetDistanceFrom (start)));

      return i.GetDistanceFrom (start);
    }

    void
    DEN::Print (std::ostream &os) const
    {
      m_den_p->Print(os);
    }

  }
}

NNN_NAMESPACE_END
