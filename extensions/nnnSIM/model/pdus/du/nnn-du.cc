/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-du.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-du.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-du.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ns3-dev/ns3/log.h>

#include "nnn-du.h"

NS_LOG_COMPONENT_DEFINE ("nnn.DU");

namespace ns3
{
  namespace nnn
  {
    DU::DU ()
    : NNNPDU (DU_NNN, Seconds (0))
    , DATAPDU ()
    , m_src (Create<NNNAddress> ())
    , m_dst (Create<NNNAddress> ())
    {
    }

    DU::DU (Ptr<NNNAddress> src, Ptr<NNNAddress> dst, Ptr<Packet> payload)
    : NNNPDU (DU_NNN, Seconds (0))
    , DATAPDU ()
    , m_src (src)
    , m_dst (dst)
    {
      if (m_payload == 0)
	m_payload = Create<Packet> ();
      else
	m_payload = payload;
    }

    DU::DU (const NNNAddress &src, const NNNAddress &dst, Ptr<Packet> payload)
    : NNNPDU (DU_NNN, Seconds(0))
    , DATAPDU ()
    , m_src     (Create<NNNAddress> (src))
    , m_dst      (Create<NNNAddress> (dst))
    {
      if (m_payload == 0)
	m_payload = Create<Packet> ();
      else
	m_payload = payload;
    }

    DU::DU (const DU &du_p)
    {
      NS_LOG_FUNCTION("DU correct copy constructor");
      DU ();
      SetVersion (du_p.GetVersion ());
      SetLifetime (du_p.GetLifetime ());
      SetSrcName (du_p.GetSrcName ());
      SetDstName (du_p.GetDstName ());
      SetPDUPayloadType (du_p.GetPDUPayloadType ());
      SetPayload (du_p.GetPayload()->Copy ());
      SetWire (du_p.GetWire ());
    }

    const NNNAddress&
    DU::GetSrcName () const
    {
      if (m_src == 0) throw DUException ();
      return *m_src;
    }

    const NNNAddress&
    DU::GetDstName () const
    {
      if (m_dst == 0) throw DUException ();
      return *m_dst;
    }

    Ptr<const NNNAddress>
    DU::GetSrcNamePtr () const
    {
      return m_src;
    }

    Ptr<const NNNAddress>
    DU::GetDstNamePtr () const
    {
      return m_dst;
    }

    void
    DU::SetSrcName (Ptr<NNNAddress> src)
    {
      m_src = src;
      SetWire(0);
    }

    void
    DU::SetSrcName (const NNNAddress &src)
    {
      m_src = Create<NNNAddress> (src);
      SetWire(0);
    }

    void
    DU::SetDstName (Ptr<NNNAddress> dst)
    {
      m_dst = dst;
      SetWire(0);
    }

    void
    DU::SetDstName (const NNNAddress &dst)
    {
      m_dst = Create<NNNAddress> (dst);
      SetWire(0);
    }

    void
    DU::Print (std::ostream &os) const
    {
      os << "<DU>" << std::endl;
      NNNPDU::Print (os);
      os << "  <Name Src>" << GetSrcName () << "</Name Src>" << std::endl;
      os << "  <Name Dst>" << GetDstName () << "</Name Dst>" << std::endl;
      DATAPDU::Print (os);
      os << "</DU>" << std::endl;
    }
  } /* namespace nnn */
} /* namespace ns3 */
