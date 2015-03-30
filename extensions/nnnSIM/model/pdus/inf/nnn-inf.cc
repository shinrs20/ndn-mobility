/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-inf.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-inf.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-inf.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/log.h>

#include "nnn-inf.h"

NS_LOG_COMPONENT_DEFINE ("nnn.INF");

namespace ns3
{
  namespace nnn
  {
    INF::INF () : NNNPDU (INF_NNN, Seconds(0))
    {
    }

    INF::INF (Ptr<NNNAddress> oldname,  Ptr<NNNAddress> newname)
    : NNNPDU (INF_NNN, Seconds(0))
    , m_old_name (oldname)
    , m_new_name (newname)
    , m_re_lease (Seconds (300))
    {
    }

    INF::INF (const NNNAddress &oldname, const NNNAddress &newname)
    : NNNPDU (INF_NNN, Seconds(0))
    , m_old_name (Create<NNNAddress> (oldname))
    , m_new_name (Create<NNNAddress> (newname))
    , m_re_lease (Seconds (300))
    {
    }

    INF::INF (const INF &inf_p)
    : NNNPDU (INF_NNN, inf_p.GetLifetime ())
    , m_old_name (Create<NNNAddress> (inf_p.GetOldName ()))
    , m_new_name (Create<NNNAddress> (inf_p.GetNewName ()))
    , m_re_lease (inf_p.GetRemainLease ())
    {
      NS_LOG_FUNCTION("INF correct copy constructor");

      SetWire (inf_p.GetWire ());
    }

    const NNNAddress&
    INF::GetOldName () const
    {
      if (m_old_name == 0) throw INFException ();
      return *m_old_name;
    }

    Ptr<const NNNAddress>
    INF::GetOldNamePtr () const
    {
      return m_old_name;
    }

    void
    INF::SetOldName (Ptr<NNNAddress> name)
    {
      m_old_name = name;
      m_wire = 0;
    }

    void
    INF::SetOldName (const NNNAddress &name)
    {
      m_old_name = Create<NNNAddress> (name);
      m_wire = 0;
    }

    const NNNAddress&
    INF::GetNewName () const
    {
      if (m_new_name == 0) throw INFException ();
      return *m_new_name;
    }

    Ptr<const NNNAddress>
    INF::GetNewNamePtr () const
    {
      return m_new_name;
    }

    void
    INF::SetNewName (Ptr<NNNAddress> name)
    {
      m_new_name = name;
      m_wire = 0;
    }

    void
    INF::SetNewName (const NNNAddress &name)
    {
      m_new_name = Create<NNNAddress> (name);
      m_wire = 0;
    }

    Time
    INF::GetRemainLease () const
    {
      return m_re_lease;
    }

    void
    INF::SetRemainLease (Time ex_lease)
    {
      m_re_lease = ex_lease;
    }

    void
    INF::Print (std::ostream &os) const
    {
      os << "<INF>" << std::endl;
      NNNPDU::Print (os);
      os << "  <OldName>" << GetOldName () << "</OldName>" << std::endl;
      os << "  <NewName>" << GetNewName () << "</NewName>" << std::endl;
      os << "  <RLease>" << GetRemainLease () << "</RLease>" << std::endl;
      os << "</INF>" << std::endl;
    }
  } // namespace nnn
} // namespace ns3
