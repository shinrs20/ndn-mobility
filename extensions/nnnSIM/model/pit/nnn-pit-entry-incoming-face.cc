/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-pit-entry-incoming-face.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pit-entry-incoming-face.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pit-entry-incoming-face.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ns3-dev/ns3/simulator.h>

#include "nnn-pit-entry-incoming-face.h"

#include "../naming/nnn-address.h"

namespace ns3 {
  namespace nnn {
    namespace pit {
      IncomingFace::IncomingFace (Ptr<Face> face)
      : m_face (face)
      , m_arrivalTime (Simulator::Now ())
      // , m_nonce (nonce)
      {
      }

      IncomingFace::IncomingFace (Ptr<Face> face, Ptr<NNNAddress> addr)
      : m_face (face)
      , m_arrivalTime (Simulator::Now ())
      , m_addrs (Create<NNNAddrAggregator> ())
      {
	m_addrs->AddDestination(addr);
      }

      IncomingFace::IncomingFace ()
      : m_face (0)
      , m_arrivalTime (0)
      {
      }

      void
      IncomingFace::AddDestination(Ptr<NNNAddress> addr)
      {
	m_addrs->AddDestination(addr);
      }

      /**
       * @brie Copy operator
       */
      IncomingFace &
      IncomingFace::operator = (const IncomingFace &other)
      {
	m_face = other.m_face;
	m_arrivalTime = other.m_arrivalTime;
	return *this;
      }
    } // namespace pit
  } // namespace nnn
} // namespace ns3
