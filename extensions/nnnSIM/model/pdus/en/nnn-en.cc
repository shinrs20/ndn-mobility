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

namespace ns3
{
  namespace nnn
  {
    EN::EN ()
    : NNNPDU (EN_NNN, Seconds(0))
    , ENPDU ()
    {
    }

    EN::EN (std::vector<Address> signatures)
    : NNNPDU (EN_NNN, Seconds(0))
    , ENPDU ()
    {
    }

    EN::EN (const EN &en_p)
    {
      NS_LOG_FUNCTION("EN correct copy constructor");
      EN ();
      SetVersion (en_p.GetVersion ());
      SetLifetime (en_p.GetLifetime ());
      AddPoa (en_p.GetPoas ());
      SetWire (en_p.GetWire ());
    }

    void
    EN::Print (std::ostream &os) const
    {
      os << "<EN>"<< std::endl;
      NNNPDU::Print(os);
      ENPDU::Print(os);
      os << "</EN>" << std::endl;
    }
  } // namespace nnn
} // namespace ns3
