/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-fib-entry.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-fib-entry.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-fib-entry.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/ndn-name.h>
#include <ns3-dev/ns3/simulator.h>

#define NDN_RTO_ALPHA 0.125
#define NDN_RTO_BETA 0.25
#define NDN_RTO_K 4

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/ref.hpp>

namespace ll = boost::lambda;

#include "nnn-fib.h"
#include "nnn-fib-entry.h"

NS_LOG_COMPONENT_DEFINE ("nnn.fib.Entry");

namespace ns3
{
  namespace nnn
  {
    namespace fib
    {
      //////////////////////////////////////////////////////////////////////
      // Helpers
      //////////////////////////////////////////////////////////////////////

      struct FaceMetricByFace
      {
	typedef FaceMetricContainer::type::index<i_face>::type
	    type;
      };


      void
      FaceMetric::UpdateRtt (const Time &rttSample)
      {
	// const Time & this->m_rttSample

	//update srtt and rttvar (RFC 2988)
	if (m_sRtt.IsZero ())
	  {
	    //first RTT measurement
	    NS_ASSERT_MSG (m_rttVar.IsZero (), "SRTT is zero, but variation is not");

	    m_sRtt = rttSample;
	    m_rttVar = Time (m_sRtt / 2.0);
	  }
	else
	  {
	    m_rttVar = Time ((1 - NDN_RTO_BETA) * m_rttVar + 1.0 * NDN_RTO_BETA * Abs(m_sRtt - rttSample));
	    m_sRtt = Time ((1 - NDN_RTO_ALPHA) * m_sRtt + 1.0 * NDN_RTO_ALPHA * rttSample);
	  }
      }

      /////////////////////////////////////////////////////////////////////

      void
      Entry::UpdateFaceRtt (Ptr<Face> face, const Time &sample)
      {
	FaceMetricByFace::type::iterator record = m_faces.get<i_face> ().find (face);
	if (record == m_faces.get<i_face> ().end ())
	  {
	    return;
	  }

	m_faces.modify (record,
			ll::bind (&FaceMetric::UpdateRtt, ll::_1, sample));

	// reordering random access index same way as by metric index
	m_faces.get<i_nth> ().rearrange (m_faces.get<i_metric> ().begin ());
      }

      void
      Entry::UpdateStatus (Ptr<Face> face, FaceMetric::Status status)
      {
	NS_LOG_FUNCTION (this << boost::cref(*face) << status);

	NS_LOG_INFO ("Searching for Face " << boost::cref(*face));

	FaceMetricByFace::type::iterator record = m_faces.get<i_face> ().find (face);
	if (record == m_faces.get<i_face> ().end ())
	  {
	    NS_LOG_INFO ("Face " << boost::cref(*face) << " not found, returning!");
	    return;
	  }

	NS_LOG_INFO ("Face " << boost::cref(*face) << " found, modifying status");
	m_faces.modify (record,
			ll::bind (&FaceMetric::SetStatus, ll::_1, status));

	NS_LOG_INFO ("Face " << boost::cref(*face) << " reordering random access");
	// reordering random access index same way as by metric index
	m_faces.get<i_nth> ().rearrange (m_faces.get<i_metric> ().begin ());
	NS_LOG_INFO ("Face " << boost::cref(*face) << " update complete");
      }

      void
      Entry::AddOrUpdateRoutingMetric (Ptr<Face> face, int32_t metric)
      {
	NS_LOG_FUNCTION (this);
	NS_ASSERT_MSG (face != NULL, "Trying to Add or Update NULL face");

	FaceMetricByFace::type::iterator record = m_faces.get<i_face> ().find (face);
	if (record == m_faces.get<i_face> ().end ())
	  {
	    m_faces.insert (FaceMetric (face, metric));
	  }
	else
	  {
	    // don't update metric to higher value
	    if (record->GetRoutingCost () > metric || record->GetStatus () == FaceMetric::NDN_FIB_RED)
	      {
		m_faces.modify (record,
				ll::bind (&FaceMetric::SetRoutingCost, ll::_1, metric));

		m_faces.modify (record,
				ll::bind (&FaceMetric::SetStatus, ll::_1, FaceMetric::NDN_FIB_YELLOW));
	      }
	  }

	// reordering random access index same way as by metric index
	m_faces.get<i_nth> ().rearrange (m_faces.get<i_metric> ().begin ());
      }

      void
      Entry::SetRealDelayToProducer (Ptr<Face> face, Time delay)
      {
	NS_LOG_FUNCTION (this);
	NS_ASSERT_MSG (face != NULL, "Trying to Update NULL face");

	FaceMetricByFace::type::iterator record = m_faces.get<i_face> ().find (face);
	if (record != m_faces.get<i_face> ().end ())
	  {
	    m_faces.modify (record,
			    ll::bind (&FaceMetric::SetRealDelay, ll::_1, delay));
	  }
      }


      void
      Entry::Invalidate ()
      {
	for (FaceMetricByFace::type::iterator face = m_faces.begin ();
	    face != m_faces.end ();
	    face++)
	  {
	    m_faces.modify (face,
			    ll::bind (&FaceMetric::SetRoutingCost, ll::_1, std::numeric_limits<uint16_t>::max ()));

	    m_faces.modify (face,
			    ll::bind (&FaceMetric::SetStatus, ll::_1, FaceMetric::NDN_FIB_RED));
	  }
      }

      const FaceMetric &
      Entry::FindBestCandidate (uint32_t skip/* = 0*/) const
      {
	if (m_faces.size () == 0) throw Entry::NoFaces ();
	skip = skip % m_faces.size();
	return m_faces.get<i_nth> () [skip];
      }

      Ptr<Fib>
      Entry::GetFib ()
      {
	return m_fib;
      }


      std::ostream& operator<< (std::ostream& os, const Entry &entry)
      {
	for (FaceMetricContainer::type::index<i_nth>::type::iterator metric =
	    entry.m_faces.get<i_nth> ().begin ();
	    metric != entry.m_faces.get<i_nth> ().end ();
	    metric++)
	  {
	    if (metric != entry.m_faces.get<i_nth> ().begin ())
	      os << ", ";

	    os << *metric;
	  }
	return os;
      }

      std::ostream& operator<< (std::ostream& os, const FaceMetric &metric)
      {
	static const std::string statusString[] = {"","g","y","r"};

	os << *metric.m_face << "(" << metric.m_routingCost << ","<< statusString [metric.m_status] << "," << metric.m_face->GetMetric () << ")";
	return os;
      }

    } // namespace fib
  } // namespace nnn
} // namespace ns3
