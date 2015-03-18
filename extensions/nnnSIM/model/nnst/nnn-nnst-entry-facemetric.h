/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnst-entry-facemetric.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnst-entry-facemetric.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnst-entry-facemetric.h.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NNN_NNST_ENTRY_FACEMETRIC_H_
#define NNN_NNST_ENTRY_FACEMETRIC_H_

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/traced-value.h>

#include "../nnn-face.h"

#define NNN_RTO_ALPHA 0.125
#define NNN_RTO_BETA 0.25
#define NNN_RTO_K 4

namespace ns3
{
  namespace nnn
  {
    namespace nnst
    {
      class FaceMetric {
      public:
	enum Status { NNN_NNST_GREEN = 1,
	  NNN_NNST_YELLOW = 2,
	  NNN_NNST_RED = 3
	};

	FaceMetric(Ptr<Face> face, Address addr, Time lease_expire, int32_t cost);
	virtual ~FaceMetric();

	/**
	 * \brief Comparison operator used by boost::multi_index::identity<>
	 */
	bool
	operator< (const FaceMetric &fm) const {
	  if (*m_face == *fm.m_face)
	    {
	      return m_dst_addr < fm.GetAddress ();
	    } else
	      {
		return *m_face < *fm.m_face;
	      }

	} // return identity of the face

	/**
	 * @brief Comparison between FaceMetric and Face
	 */
	bool
	operator< (const Ptr<Face> &face) const { return *m_face < *face; }

	/**
	 * @brief Return Face associated with FaceMetric
	 */
	Ptr<Face>
	GetFace () const { return m_face; }

	void
	SetStatus (Status status)
	{
	  m_status.Set (status);
	}

	Status
	GetStatus () const
	{
	  return m_status;
	}

	/**
	 * @brief Get current routing cost
	 */
	int32_t
	GetRoutingCost () const
	{
	  return m_routingCost;
	}

	/**
	 * @brief Set routing cost
	 */
	void
	SetRoutingCost (int32_t routingCost)
	{
	  m_routingCost = routingCost;
	}

	/**
	 * @brief Get current estimate for smoothed RTT value
	 */
	Time
	GetSRtt () const
	{
	  return m_sRtt;
	}

	/**
	 * @brief Get current estimate for the RTT variation
	 */
	Time
	GetRttVar () const
	{
	  return m_rttVar;
	}

	/**
	 * \brief Recalculate smoothed RTT and RTT variation
	 * \param rttSample RTT sample
	 */
	void
	UpdateRtt (const Time &rttSample);

	Address
	GetAddress () const
	{
	  return m_dst_addr;
	}

	void
	SetAddress (Address poa)
	{
	  m_dst_addr = poa;
	}

	Time
	GetExpireTime () const
	{
	  return m_lease_expire;
	}

	void
	UpdateExpireTime (Time lease)
	{
	  m_lease_expire = lease;
	}

      private:
	friend std::ostream& operator<< (std::ostream& os, const FaceMetric &metric);

      private:
	Ptr<Face> m_face;             ///< Face
	Address m_dst_addr;           ///< \brief Destination address to use on Face
	Time m_lease_expire;          ///< \brief Time when this entry will expire
	Time m_sRtt;                  ///< \brief Round-trip time variation
	Time m_rttVar;                ///< \brief round-trip time variation
	int32_t m_routingCost;        ///< \brief routing protocol cost (interpretation of the value depends on the underlying routing protocol)
	TracedValue<Status> m_status; ///< \brief Status of next hop
      };

      std::ostream& operator<< (std::ostream& os, const FaceMetric &metric);

    } /* namespace nnst */
  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_NNST_ENTRY_FACEMETRIC_H_ */
