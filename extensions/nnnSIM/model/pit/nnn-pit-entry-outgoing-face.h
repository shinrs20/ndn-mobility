/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-pit-entry-outgoing-face.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pit-entry-outgoing-face.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pit-entry-outgoing-face.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNN_PIT_ENTRY_OUTGOING_FACE_H_
#define	_NNN_PIT_ENTRY_OUTGOING_FACE_H_

#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/ptr.h>

#include "../nnn-face.h"

namespace ns3 {
  namespace nnn {
    namespace pit {
      /**
       * @ingroup ndn-pit
       * @brief PIT state component for each outgoing interest
       */
      struct OutgoingFace
      {
	Ptr<Face> m_face;     ///< \brief face of the outgoing Interest
	Time m_sendTime;          ///< \brief time when the first outgoing interest is sent (for RTT measurements)
	///< \todo handle problem of retransmitted interests... Probably, we should include something similar
	///<       to TimeStamp TCP option for retransmitted (i.e., only lost interests will suffer)
	uint32_t m_retxCount;     ///< \brief number of retransmission
	bool m_waitingInVain;     ///< \brief when flag is set, we do not expect data for this interest, only a small hope that it will happen

      public:
	/**
	 * @brief Constructor to create PitEntryOutgoingFace
	 * \param face face of the outgoing interest
	 */
	OutgoingFace (Ptr<Face> face);

	/**
	 * @brief Default constructor, necessary for Python bindings, but should not be used anywhere else.
	 */
	OutgoingFace ();

	/**
	 * @brief Copy operator
	 */
	OutgoingFace &
	operator = (const OutgoingFace &other);

	/**
	 * @brief Update outgoing entry upon retransmission
	 */
	void
	UpdateOnRetransmit ();

	/**
	 * @brief Compare to PitEntryOutgoingFace
	 */
	bool operator== (const OutgoingFace &dst) { return *m_face==*dst.m_face; }

	/**
	 * @brief Compare PitEntryOutgoingFace with Face
	 */
	bool operator== (Ptr<Face> face) { return *m_face==*face; }

	/**
	 * \brief Comparison operator used by boost::multi_index::identity<>
	 */
	bool
	operator< (const OutgoingFace &m) const { return *m_face < *(m.m_face); } // return identity of the face
      };
    } // namespace pit
  } // namespace nnn
} // namespace ns3

#endif	/* NNN_PIT_ENTRY_OUTGOING_FACE_H */
