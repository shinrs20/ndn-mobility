/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-pit-entry-incoming-face.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pit-entry-incoming-face.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pit-entry-incoming-face.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNN_PIT_ENTRY_INCOMING_FACE_H_
#define	_NNN_PIT_ENTRY_INCOMING_FACE_H_

#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/ptr.h>

#include "../nnn-face.h"
#include "../addr-aggr/nnn-addr-aggregator.h"

namespace ns3 {
  namespace nnn {

    class NNNAddress;

    namespace pit {

      /**
       * @ingroup nnn-pit
       * @brief PIT state component for each incoming interest (not including duplicates)
       */
      struct IncomingFace
      {
	Ptr<Face> m_face; ///< \brief face of the incoming Interest
	Ptr<NNNAddrAggregator> m_addrs; ///< \brief 3N names seen in incoming Interests
	Time m_arrivalTime;   ///< \brief arrival time of the incoming Interest

      public:
	/**
	 * \brief Constructor
	 * \param face face of the incoming interest
	 */
	IncomingFace (Ptr<Face> face);

	/**
	 * @brief Default constructor, necessary for Python bindings, but should not be used anywhere else.
	 */
	IncomingFace ();

	/**
	 * \brief Constructor
	 * \param face face of the incoming interest
	 * \param addr 3N name we will aggregate
	 */
	IncomingFace (Ptr<Face> face, Ptr<const NNNAddress> addr);

	/**
	 * \brief Add the 3N name to the associated face
	 * \param addr 3N name we will aggregate
	 */
	void
	AddDestination (Ptr<const NNNAddress> addr);

	/**
	 * \brief Add the 3N name to the associated face
	 * \param addr 3N name we will aggregate
	 */
	void
	RemoveDestination (Ptr<const NNNAddress> addr);

	bool
	NoAddresses ();

	/**
	 * @brief Copy operator
	 */
	IncomingFace &
	operator = (const IncomingFace &other);

	/**
	 * @brief Compare two PitEntryIncomingFace
	 */
	bool operator== (const IncomingFace &dst) const { return *m_face==*(dst.m_face); }

	/**
	 * @brief Compare PitEntryIncomingFace with Face
	 */
	bool operator== (Ptr<Face> face) const { return *m_face==*face; }

	/**
	 * \brief Comparison operator used by boost::multi_index::identity<>
	 */
	bool
	operator< (const IncomingFace &m) const { return *m_face < *(m.m_face); } // return identity of the face
      };
    } // namespace pit
  } // namespace nnn
} // namespace ns3

#endif	/* NNN_PIT_ENTRY_INCOMING_FACE_H */
