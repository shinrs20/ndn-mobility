/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-pit.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pit.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pit.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/uinteger.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/packet.h>

#include <ns3-dev/ns3/ndn-name.h>
#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-data.h>

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include "nnn-pit.h"

NS_LOG_COMPONENT_DEFINE ("nnn.Pit");

namespace ns3 {
  namespace nnn {

    NS_OBJECT_ENSURE_REGISTERED (Pit);

    TypeId

    Pit::GetTypeId ()
    {
      static TypeId tid = TypeId ("ns3::nnn::Pit")
	    .SetGroupName ("Nnn")
	    .SetParent<Object> ()

	    .AddAttribute ("PitEntryPruningTimout",
	                   "Timeout for PIT entry to live after being satisfied. To make sure recently satisfied interest will not be satisfied again",
	                   TimeValue (), // by default, PIT entries are removed instantly
	                   MakeTimeAccessor (&Pit::m_PitEntryPruningTimout),
	                   MakeTimeChecker ())

	    .AddAttribute ("MaxPitEntryLifetime",
	                   "Maximum amount of time for which a router is willing to maintain a PIT entry. "
	                   "Actual PIT lifetime should be minimum of MaxPitEntryLifetime and InterestLifetime specified in the Interest packet",
	                   TimeValue (), // by default, PIT entries are kept for the time, specified by the InterestLifetime
	                   MakeTimeAccessor (&Pit::GetMaxPitEntryLifetime, &Pit::SetMaxPitEntryLifetime),
	                   MakeTimeChecker ())
	;
      return tid;
    }

    Pit::Pit ()
    {
    }

    Pit::~Pit ()
    {
    }

  } // namespace nnn
} // namespace ns3
