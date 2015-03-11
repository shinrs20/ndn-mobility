/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-face-container.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-face-container.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-face-container.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>

#include "nnn-face-container.h"

namespace ns3 {
  namespace nnn {

    FaceContainer::FaceContainer() {
    }

    FaceContainer::FaceContainer (const FaceContainer &other)
    {
      AddAll (other);
    }

    FaceContainer&
    FaceContainer::operator= (const FaceContainer &other)
    {
      m_faces.clear ();
      AddAll (other);

      return *this;
    }


    void
    FaceContainer::AddAll (Ptr<FaceContainer> other)
    {
      AddAll (*other);
    }

    void
    FaceContainer::AddAll (const FaceContainer &other)
    {
      m_faces.insert (m_faces.end (),
		      other.m_faces.begin (), other.m_faces.end ());
    }

    FaceContainer::Iterator
    FaceContainer::Begin (void) const
    {
      return m_faces.begin ();
    }

    FaceContainer::Iterator
    FaceContainer::End (void) const
    {
      return m_faces.end ();
    }

    uint32_t
    FaceContainer::GetN (void) const
    {
      return m_faces.size ();
    }

    void
    FaceContainer::Add (const Ptr<Face> &face)
    {
      m_faces.push_back (face);
    }

    void
    FaceContainer::Remove(const Ptr<Face> &face)
    {
      for (iter it = m_faces.begin(); it != m_faces.end(); ++it)
	{
	  if (face == *it)
	    {
	      m_faces.erase(it);
	      break;
	    }
	}
    }

    Ptr<Face>
    FaceContainer::Get (uint32_t i) const
    {
      Ptr<Face> tmp;
      if (0 <= i && i < GetN ())
	{
	  tmp = m_faces[i];
	}

      return tmp;
    }

    Ptr<Face>
    FaceContainer::Get (FaceContainer::Iterator i) const
    {
      return *i;
    }


  } /* namespace nnn */
} /* namespace ns3 */
