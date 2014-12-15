/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
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
#include <ns3-dev/ns3/unused.h>
#include <ns3-dev/ns3/packet.h>

#include "nnn-ren.h"

NS_LOG_COMPONENT_DEFINE ("nnn.REN");

namespace ns3 {
  namespace nnn {

    REN::REN ()
    : NNNPacket (REN_NNN, Seconds (0))
    , m_re_lease (Seconds (0))
    , m_poa_type (POA_MAC48)
    , m_poas     (std::vector<Address> ())
    {
    }

    REN::REN (Ptr<NNNAddress> name)
    : NNNPacket (REN_NNN, Seconds (0))
    , m_name     (name)
    , m_re_lease (Seconds (0))
    , m_poa_type (POA_MAC48)
    , m_poas     (std::vector<Address> ())
    {
    }

    REN::REN (const NNNAddress &name)
    : NNNPacket (REN_NNN, Seconds (0))
    , m_name     (Create<NNNAddress> (name))
    , m_re_lease (Seconds (0))
    , m_poa_type (POA_MAC48)
    , m_poas     (std::vector<Address> ())
    {
    }

    REN::REN (const REN &ren_p)
    : NNNPacket (REN_NNN, ren_p.GetLifetime ())
    , m_name     (Create<NNNAddress> (ren_p.GetName()))
    , m_re_lease (ren_p.GetRemainLease ())
    , m_poa_type (ren_p.GetPoaType ())
    , m_poas     (ren_p.GetPoas ())
    {
      NS_LOG_FUNCTION("REN correct copy constructor");

      SetWire (ren_p.GetWire ());
    }

    const NNNAddress&
    REN::GetName () const
    {
      if (m_name == 0) throw RENException ();
      return *m_name;
    }

    Ptr<const NNNAddress>
    REN::GetNamePtr () const
    {
      return m_name;
    }

    void
    REN::SetName (Ptr<NNNAddress> name)
    {
      m_name = name;
      m_wire = 0;
    }

    void
    REN::SetName (const NNNAddress &name)
    {
      m_name = Create<NNNAddress> (name);
      m_wire = 0;
    }

    uint16_t
    REN::GetPoaType () const
    {
      return m_poa_type;
    }

    void
    REN::SetPoaType (uint16_t type)
    {
      m_poa_type = type;
    }

    uint32_t
    REN::GetNumPoa () const
    {
      return m_poas.size();
    }

    std::vector<Address>
    REN::GetPoas () const
    {
      return m_poas;
    }

    Address
    REN::GetOnePoa (uint32_t index) const
    {
      if (index < GetNumPoa ())
	return m_poas[index];
      else
	return Address();
    }

    void
    REN::AddPoa (Address signature)
    {
      m_poas.push_back(signature);
    }

    void
    REN::AddPoa (std::vector<Address> signatures)
    {
      m_poas.insert(m_poas.end (), signatures.begin (), signatures.end ());
    }

    Time
    REN::GetRemainLease () const
    {
      return m_re_lease;
    }

    void
    REN::SetRemainLease (Time ex_lease)
    {
      m_re_lease = ex_lease;
    }

    void
    REN::Print (std::ostream &os) const
    {
      uint32_t num = GetNumPoa ();
      uint16_t type = GetPoaType ();
      os << "<REN>\n";
      os << "  <TTL>" << GetLifetime () << "</TTL>\n";
      os << "  <Name>" << GetName () << "</Name>\n";
      os << "  <RLease>" << GetRemainLease () << "</RLease>\n";
      os << "  <POATYPE>" << type << "</POATYPE>\n";
      os << "  <POANUM>" << num << "</POANUM>\n";
      for (int i = 0; i < num; i++)
	{
	  os << "  <POA" << i << ">" << m_poas[i] << "</POA" << i << ">\n";
	}
      os << "</REN>";
    }

  } // namespace nnn
} // namespace ns3
