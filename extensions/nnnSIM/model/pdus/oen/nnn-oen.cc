/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-oen.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-oen.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-oen.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "nnn-oen.h"
#include "../nnn-en-pdus.h"
#include "../../nnn-naming.h"

namespace ns3
{
  namespace nnn
  {

    OEN::OEN ()
    : NNNPDU (OEN_NNN, Seconds(0))
    , ENPDU ()
    {
    }

    OEN::~OEN ()
    {
    }

    OEN::OEN (const OEN &oen_p)
    {
      OEN tmp = OEN ();
      tmp.SetLeasetime(oen_p.GetLeasetime());
      tmp.SetLifetime(oen_p.GetLifetime());
      tmp.AddPoa(oen_p.GetPoas());
      tmp.SetName(oen_p.GetName());
      tmp.SetPoaType(oen_p.GetPoaType());
      tmp.SetVersion(oen_p.GetVersion());
      tmp.SetWire(oen_p.GetWire());
    }

    const NNNAddress&
    OEN::GetName () const
    {
      if (m_name == 0) throw OENException ();
      return *m_name;
    }

    Ptr<const NNNAddress>
    OEN::GetNamePtr () const
    {
      return m_name;
    }

    void
    OEN::SetName(Ptr<NNNAddress> name)
    {
      m_name = name;
      SetWire (0);
    }

    void
    OEN::SetName (const NNNAddress &name)
    {
      m_name = Create<NNNAddress> (name);
      SetWire (0);
    }


    Time
    OEN::GetLeasetime() const
    {
      return m_lease;
    }


    void
    OEN::SetLeasetime (Time lease)
    {
      m_lease = lease;
    }

    void
    OEN::Print (std::ostream &os) const
    {
      os << "<OEN>" << std::endl;
      NNNPDU::Print(os);
      os << "  <Name>" << GetName () << "</Name>" << std::endl;
      ENPDU::Print(os);
      os << "</OEN>" << std::endl;
    }

  } /* namespace nnn */
} /* namespace ns3 */
