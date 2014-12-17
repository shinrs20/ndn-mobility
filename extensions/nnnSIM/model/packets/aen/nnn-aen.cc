/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *           Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnn-do.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-do.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-en.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/unused.h>

#include "nnn-aen.h"

NS_LOG_COMPONENT_DEFINE ("nnn.AEN");

namespace ns3 {
  namespace nnn {

    AEN::AEN () : NNNPacket (AEN_NNN, Seconds(0))
    {
    }

    AEN::AEN (Ptr<NNNAddress> name)
    : NNNPacket (AEN_NNN, Seconds (300))
    , m_name     (name)
    {
    }

    AEN::AEN (const NNNAddress &name)
    : NNNPacket (AEN_NNN, Seconds (300))
    , m_name     (Create<NNNAddress> (name))
    {
    }

    AEN::AEN (const AEN &aen_p)
    : NNNPacket (AEN_NNN, aen_p.GetLifetime())
    , m_name     (Create<NNNAddress> (aen_p.GetName()))
    {
      NS_LOG_FUNCTION("AEN correct copy constructor");

      SetLeasetime (aen_p.GetLeasetime ());
      SetWire (aen_p.GetWire ());
    }

    const NNNAddress&
    AEN::GetName () const
    {
      if (m_name == 0) throw AENException ();
      return *m_name;
    }

    Ptr<const NNNAddress>
    AEN::GetNamePtr () const
    {
      return m_name;
    }

    void
    AEN::SetName (Ptr<NNNAddress> name)
    {
      m_name = name;
      SetWire (0);
    }

    void
    AEN::SetName (const NNNAddress &name)
    {
      m_name = Create<NNNAddress> (name);
      SetWire (0);
    }

    Time
    AEN::GetLeasetime () const
    {
      return m_lease;
    }

    void
    AEN::SetLeasetime (Time lease)
    {
      m_lease = lease;
    }

    void
    AEN::Print (std::ostream &os) const
    {
      os << "<AEN>" << std::endl;
      os << "  <TTL>" << GetLifetime () << "</TTL>" << std::endl;
      os << "  <Version>" << GetVersion () << "</Version>" << std::endl;
      os << "  <Name>" << GetName () << "</Name>" << std::endl;
      os << "  <Lease>" << GetLeasetime () << "</Lease>" << std::endl;
      os << "</AEN>"  << std::endl;
    }

  } // namespace nnn
} // namespace ns3
