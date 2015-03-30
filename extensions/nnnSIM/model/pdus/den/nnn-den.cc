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

#include "nnn-den.h"

NS_LOG_COMPONENT_DEFINE ("nnn.DEN");

namespace ns3
{
  namespace nnn
  {
    DEN::DEN ()
    : NNNPDU (DEN_NNN, Seconds(0))
    , ENPDU ()
    {
    }

    DEN::DEN (Ptr<NNNAddress> name)
    : NNNPDU (DEN_NNN, Seconds(0))
    , ENPDU ()
    {
      SetName (name);
    }

    DEN::DEN (const NNNAddress &name)
    : NNNPDU (DEN_NNN, Seconds(0))
    , ENPDU ()
    {
      SetName (name);
    }

    DEN::DEN (const DEN &den_p)
    {
      NS_LOG_FUNCTION("DEN correct copy constructor");
      DEN ();
      SetVersion (den_p.GetVersion ());
      SetLifetime (den_p.GetLifetime ());
      SetPoaType (den_p.GetPoaType ());
      AddPoa (den_p.GetPoas ());
      SetName (den_p.GetName ());
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

    void
    DEN::Print (std::ostream &os) const
    {
      os << "<DEN>" << std::endl;
      NNNPDU::Print(os);
      os << "  <Name>" << GetName () << "</Name>" << std::endl;
      ENPDU::Print(os);
      os << "</DEN>" << std::endl;
    }

  } // namespace nnn
} // namespace ns3
