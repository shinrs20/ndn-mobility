/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-common-hdr.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-common-hdr.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-common-hdr.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/ptr.h>

#include "nnnsim-common.h"

#ifndef NNNSIM_COMMON_HDR_H_
#define NNNSIM_COMMON_HDR_H_



namespace ns3
{
  namespace nnn
  {
    namespace wire
    {
      namespace nnnSIM
      {

	template <class T>
	class CommonHeader : public Header
	{
	  public:

	  CommonHeader ()
	  : m_ptr (Create<T> ())
	  , m_packet_len (0)
	  {
	  }

	  CommonHeader (Ptr<T> t_p)
	  : m_ptr (t_p)
	  , m_packet_len (0)
	  {
	  }

	  virtual
	  ~CommonHeader ()
	  {
	  }

	  Ptr<T>
	  GetPtr()
	  {
	    return m_ptr;
	  }

	  size_t
	  CommonGetSerializedSize () const
	  {
	    return (4 +    /* PacketId */
		2 +        /* TTL */
		2 +        /* Version */
		2          /* Packet Length */
	    );
	  }

	  void
	  CommonSerialize (Buffer::Iterator start) const
	  {
	    // Serialize packetid
	    start.WriteU32(m_ptr->GetPacketId ());

	    // Check that the lifetime is within the limits
	    NS_ASSERT_MSG (0 <= static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S))
	    && static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S)) < 65535,
	                   "Incorrect Lifetime (should not be smaller than 0 and larger than 65535");

	    // Round lifetime to seconds and serialize
	    start.WriteU16 (static_cast<uint16_t> (m_ptr->GetLifetime ().ToInteger (Time::S)));

	    // Serialize the version
	    start.WriteU16(m_ptr->GetVersion ());

	    // return because everything else is Header specific
	  }

	  uint32_t
	  CommonDeserialize (Buffer::Iterator start)
	  {
	    Buffer::Iterator i = start;

	    // Read the packet id
	    i.ReadU32 ();

	    // Read TTL
	    m_ptr->SetLifetime(Seconds (i.ReadU16 ()));

	    // Read Version
	    m_ptr->SetVersion(i.ReadU16 ());

	    // Read packet length
	    m_packet_len = i.ReadU16 ();

	    NS_ASSERT (CommonGetSerializedSize () == (i.GetDistanceFrom (start)));

	    // return because everything else is Header specific
	    return i.GetDistanceFrom (start);
	  }

	  void Print (std::ostream &os) const
	  {
	    m_ptr->Print(os);
	  }

	  virtual uint32_t GetSerializedSize (void) const
	  {
	  }

	  virtual void Serialize (Buffer::Iterator start) const
	  {
	  }

	  virtual uint32_t Deserialize (Buffer::Iterator start)
	  {
	  }

	  protected:
	  uint16_t m_packet_len;
	  Ptr<T> m_ptr;
	};

      } /* namespace nnnSIM */
    } /* namespace wire */
  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNNSIM_COMMON_HDR_H_ */
