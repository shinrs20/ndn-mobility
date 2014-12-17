/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *           Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnn-den.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-den.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-den.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#include<ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/unused.h>

#include "nnn-den.h"

NS_LOG_COMPONENT_DEFINE ("nnn.DEN");

namespace ns3 {
  namespace nnn{

    DEN::DEN ()
    : NNNPacket (DEN_NNN, Seconds(0))
    , m_poa_type (POA_MAC48)
    , m_poas     (std::vector<Address> ())
    {
    }

    DEN::DEN (Ptr<NNNAddress> name)
    : NNNPacket (DEN_NNN, Seconds(300))
    , m_name     (name)
    , m_poa_type (POA_MAC48)
    , m_poas     (std::vector<Address> ())
    {
    }

    DEN::DEN (const NNNAddress &name)
    : NNNPacket (DEN_NNN, Seconds(300))
    , m_name     (Create<NNNAddress> (name))
    , m_poa_type (POA_MAC48)
    , m_poas     (std::vector<Address> ())
    {
    }

    DEN::DEN (const DEN &den_p)
    : NNNPacket (DEN_NNN, den_p.GetLifetime ())
    , m_name     (Create<NNNAddress> (den_p.GetName()))
    , m_poa_type (den_p.GetPoaType())
    , m_poas (den_p.GetPoas())
    {
      NS_LOG_FUNCTION("DEN correct copy constructor");

      SetWire (den_p.GetWire ());
    }

    const NNNAddress&
    DEN::GetName () const
    {
      if (m_name == 0) throw DENException ();
      return *m_name;
    }

    Ptr<const NNNAddress>
    DEN::GetNamePtr () const
    {
      return m_name;
    }

    void
    DEN::SetName(Ptr<NNNAddress> name)
    {
      m_name = name;
      SetWire (0);
    }

    void
    DEN::SetName (const NNNAddress &name)
    {
      m_name = Create<NNNAddress> (name);
      SetWire (0);
    }

    uint16_t
    DEN::GetPoaType () const
    {
      return m_poa_type;
    }

    void
    DEN::SetPoaType (uint16_t type)
    {
      m_poa_type = type;
    }

    void
    DEN::AddPoa (Address signature)
    {
      m_poas.push_back(signature);
    }

    void
    DEN::AddPoa (std::vector<Address> signatures)
    {
      m_poas.insert(m_poas.end (), signatures.begin (), signatures.end ());
    }

    uint32_t
    DEN::GetNumPoa () const
    {
      return m_poas.size();
    }

    std::vector<Address>
    DEN::GetPoas () const
    {
      return m_poas;
    }

    Address
    DEN::GetOnePoa (uint32_t index) const
    {
      if (index < GetNumPoa ())
	return m_poas[index];
      else
	return Address();
    }

    void
    DEN::Print (std::ostream &os) const
    {
      os << "<DEN>" << std::endl;
      os << "  <TTL>" << GetLifetime () << "</TTL>" << std::endl;
      os << "  <Version>" << GetVersion () << "</Version>" << std::endl;
      os << "  <Name>" << GetName () << "</Name>" << std::endl;
      for (int i = 0; i < GetNumPoa (); i++)
	{
	  os << "  <POA" << i << ">" << GetOnePoa(i) << "</POA" << i << ">"<< std::endl;
	}
      os << "</DEN>" << std::endl;
    }

  } // namespace nnn
} // namespace ns3
