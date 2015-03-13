/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *	         Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnn-en.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-en.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-en.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/log.h>

#include "nnn-en.h"

NS_LOG_COMPONENT_DEFINE ("nnn.EN");

namespace ns3 {

  class Packet;

  namespace nnn {

    EN::EN () : NNNPDU (EN_NNN, Seconds(0))
    , m_poa_type (POA_MAC48)
    , m_poas     (std::vector<Address> ())
    {
    }

    EN::EN (std::vector<Address> signatures)
    : NNNPDU (EN_NNN, Seconds(0))
    , m_poa_type (POA_MAC48)
    , m_poas     (signatures)
    {
    }

    EN::EN (const EN &en_p)
    : NNNPDU (EN_NNN, en_p.GetLifetime ())
    , m_poa_type  (en_p.GetPoaType ())
    , m_poas      (en_p.GetPoas ())
    {
      NS_LOG_FUNCTION("EN correct copy constructor");

      SetWire (en_p.GetWire ());
    }

    uint16_t
    EN::GetPoaType () const
    {
      return m_poa_type;
    }

    void
    EN::SetPoaType (uint16_t type)
    {
      m_poa_type = type;
    }

    void
    EN::AddPoa (Address signature)
    {
      m_poas.push_back(signature);
    }

    void
    EN::AddPoa (std::vector<Address> signatures)
    {
      m_poas.insert(m_poas.end (), signatures.begin (), signatures.end ());
    }

    uint32_t
    EN::GetNumPoa () const
    {
      return m_poas.size();
    }

    std::vector<Address>
    EN::GetPoas () const
    {
      return m_poas;
    }

    Address
    EN::GetOnePoa (uint32_t index) const
    {
      if (index < GetNumPoa ())
	return m_poas[index];
      else
	return Address();
    }

    void
    EN::Print (std::ostream &os) const
    {
      uint32_t num = GetNumPoa ();
      uint16_t type = GetPoaType ();
      os << "<EN>"<< std::endl;
      os << "  <TTL>" << GetLifetime () << "</TTL>" << std::endl;
      os << "  <Version>" << GetVersion () << "</Version>" << std::endl;
      os << "  <POA Type>" << type << "</POA Type>"<< std::endl;
      os << "  <POA Num>" << num << "</POA Num>"<< std::endl;
      for (int i = 0; i < num; i++)
	{
	  os << "  <POA" << i << ">" << m_poas[i] << "</POA" << i << ">"<< std::endl;
	}
      os << "</EN>" << std::endl;
    }

  } // namespace nnn
} // namespace ns3
