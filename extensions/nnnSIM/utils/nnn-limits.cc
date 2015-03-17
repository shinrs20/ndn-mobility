/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-limits.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-limits.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-limits.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-limits.h"

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/random-variable.h>

NS_LOG_COMPONENT_DEFINE ("nnn.Limits");

namespace ns3 {
  namespace nnn {

    TypeId
    Limits::GetTypeId ()
    {
      static TypeId tid = TypeId ("ns3::ndn::Limits")
	.SetGroupName ("Ndn")
	.SetParent <Object> ()

	;
      return tid;
    }

    Limits::Limits ()
    : m_maxRate (-1)
    , m_maxDelay (1.0)
    , m_handler (MakeNullCallback<void> ())
    , m_linkDelay (0)
    {
    }

    void
    Limits::RegisterAvailableSlotCallback (CallbackHandler handler)
    {
      m_handler = handler;
    }

    void
    Limits::FireAvailableSlotCallback ()
    {
      if (!m_handler.IsNull ())
	m_handler ();
    }
  } // namespace nnn
} // namespace ns3
