/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-l3-rate-tracer.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-l3-rate-tracer.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-l3-rate-tracer.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-l3-rate-tracer.h"

#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/config.h>
#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/node-list.h>

#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-data.h>

#include "../../model/nnn-app-face.h"
#include "../../model/nnn-pdus.h"
#include "../../model/nnn-face.h"
#include "../../model/pit/nnn-pit-entry.h"

#include <fstream>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("nnn.L3RateTracer");

namespace ns3
{
  namespace nnn
  {
    static std::list< boost::tuple< boost::shared_ptr<std::ostream>, std::list<Ptr<L3RateTracer> > > > g_tracers;

    template<class T>
    static inline void
    NullDeleter (T *ptr)
    {
    }

    L3RateTracer::L3RateTracer (boost::shared_ptr<std::ostream> os, Ptr<Node> node)
    : L3Tracer (node)
    , m_os (os)
    {
      SetAveragingPeriod (Seconds (1.0));
    }

    L3RateTracer::L3RateTracer (boost::shared_ptr<std::ostream> os, const std::string &node)
    : L3Tracer (node)
    , m_os (os)
    {
      SetAveragingPeriod (Seconds (1.0));
    }

    L3RateTracer::~L3RateTracer ()
    {
      m_printEvent.Cancel ();
    }

    void
    L3RateTracer::InstallAll (const std::string &file, Time averagingPeriod/* = Seconds (0.5)*/)
    {
      std::list<Ptr<L3RateTracer> > tracers;
      boost::shared_ptr<std::ostream> outputStream;
      if (file != "-")
	{
	  boost::shared_ptr<std::ofstream> os (new std::ofstream ());
	  os->open (file.c_str (), std::ios_base::out | std::ios_base::trunc);

	  if (!os->is_open ())
	    {
	      NS_LOG_ERROR ("File " << file << " cannot be opened for writing. Tracing disabled");
	      return;
	    }

	  outputStream = os;
	}
      else
	{
	  outputStream = boost::shared_ptr<std::ostream> (&std::cout, NullDeleter<std::ostream>);
	}

      for (NodeList::Iterator node = NodeList::Begin ();
	  node != NodeList::End ();
	  node++)
	{
	  Ptr<L3RateTracer> trace = Install (*node, outputStream, averagingPeriod);
	  tracers.push_back (trace);
	}

      if (tracers.size () > 0)
	{
	  // *m_l3RateTrace << "# "; // not necessary for R's read.table
	  tracers.front ()->PrintHeader (*outputStream);
	  *outputStream << "\n";
	}

      g_tracers.push_back (boost::make_tuple (outputStream, tracers));
    }

    void
    L3RateTracer::Install (const NodeContainer &nodes, const std::string &file, Time averagingPeriod/* = Seconds (0.5)*/)
    {
      using namespace boost;
      using namespace std;

      std::list<Ptr<L3RateTracer> > tracers;
      boost::shared_ptr<std::ostream> outputStream;
      if (file != "-")
	{
	  boost::shared_ptr<std::ofstream> os (new std::ofstream ());
	  os->open (file.c_str (), std::ios_base::out | std::ios_base::trunc);

	  if (!os->is_open ())
	    {
	      NS_LOG_ERROR ("File " << file << " cannot be opened for writing. Tracing disabled");
	      return;
	    }

	  outputStream = os;
	}
      else
	{
	  outputStream = boost::shared_ptr<std::ostream> (&std::cout, NullDeleter<std::ostream>);
	}

      for (NodeContainer::Iterator node = nodes.Begin ();
	  node != nodes.End ();
	  node++)
	{
	  Ptr<L3RateTracer> trace = Install (*node, outputStream, averagingPeriod);
	  tracers.push_back (trace);
	}

      if (tracers.size () > 0)
	{
	  // *m_l3RateTrace << "# "; // not necessary for R's read.table
	  tracers.front ()->PrintHeader (*outputStream);
	  *outputStream << "\n";
	}

      g_tracers.push_back (boost::make_tuple (outputStream, tracers));
    }

    void
    L3RateTracer::Install (Ptr<Node> node, const std::string &file, Time averagingPeriod/* = Seconds (0.5)*/)
    {
      using namespace boost;
      using namespace std;

      std::list<Ptr<L3RateTracer> > tracers;
      boost::shared_ptr<std::ostream> outputStream;
      if (file != "-")
	{
	  boost::shared_ptr<std::ofstream> os (new std::ofstream ());
	  os->open (file.c_str (), std::ios_base::out | std::ios_base::trunc);

	  if (!os->is_open ())
	    {
	      NS_LOG_ERROR ("File " << file << " cannot be opened for writing. Tracing disabled");
	      return;
	    }

	  outputStream = os;
	}
      else
	{
	  outputStream = boost::shared_ptr<std::ostream> (&std::cout, NullDeleter<std::ostream>);
	}

      Ptr<L3RateTracer> trace = Install (node, outputStream, averagingPeriod);
      tracers.push_back (trace);

      if (tracers.size () > 0)
	{
	  // *m_l3RateTrace << "# "; // not necessary for R's read.table
	  tracers.front ()->PrintHeader (*outputStream);
	  *outputStream << "\n";
	}

      g_tracers.push_back (boost::make_tuple (outputStream, tracers));
    }

    void
    L3RateTracer::Destroy ()
    {
      g_tracers.clear ();
    }

    Ptr<L3RateTracer>
    L3RateTracer::Install (Ptr<Node> node,
                           boost::shared_ptr<std::ostream> outputStream,
                           Time averagingPeriod/* = Seconds (0.5)*/)
    {
      NS_LOG_DEBUG ("Node: " << node->GetId ());

      Ptr<L3RateTracer> trace = Create<L3RateTracer> (outputStream, node);
      trace->SetAveragingPeriod (averagingPeriod);

      return trace;
    }

    void
    L3RateTracer::PrintHeader (std::ostream &os) const
    {
      os << "Time" << "\t"

	  << "Node" << "\t"
	  << "FaceId" << "\t"
	  << "FaceDescr" << "\t"

	  << "Type" << "\t"
	  << "Packets" << "\t"
	  << "Kilobytes" << "\t"
	  << "PacketRaw" << "\t"
	  << "KilobytesRaw";
    }

#define STATS(INDEX) stats->second.get<INDEX> ()
#define RATE(INDEX, fieldName) STATS(INDEX).fieldName / m_period.ToDouble (Time::S)

#define PRINTER(printName, fieldName) \
    STATS(2).fieldName = /*new value*/alpha * RATE(0, fieldName) + /*old value*/(1-alpha) * STATS(2).fieldName; \
    STATS(3).fieldName = /*new value*/alpha * RATE(1, fieldName) / 1024.0 + /*old value*/(1-alpha) * STATS(3).fieldName; \
    \
    os << time.ToDouble (Time::S) << "\t"                                 \
    << m_node << "\t";                                                    \
    if (stats->first)                                                     \
    {                                                                   \
	os                                                                \
	<< stats->first->GetId () << "\t"                               \
	<< *stats->first << "\t";                                       \
    }                                                                   \
    else                                                                  \
    {                                                                   \
	os << "-1\tall\t";                                                \
    }                                                                   \
    os                                                                    \
    << printName << "\t"                                                  \
    << STATS(2).fieldName << "\t"                                         \
    << STATS(3).fieldName << "\t"                                         \
    << STATS(0).fieldName << "\t"                                         \
    << STATS(1).fieldName / 1024.0 << "\n";

    const double alpha = 0.8;

    void
    L3RateTracer::Print (std::ostream &os) const
    {
      Time time = Simulator::Now ();

      for (std::map<Ptr<const Face>, boost::tuple<Stats, Stats, Stats, Stats> >::iterator stats = m_stats.begin ();
	  stats != m_stats.end ();
	  stats++)
	{
	  if (!stats->first)
	    continue;

	  PRINTER ("InInterests",   m_inInterests);
	  PRINTER ("OutInterests",  m_outInterests);
	  PRINTER ("DropInterests", m_dropInterests);

	  PRINTER ("InNacks",   m_inNacks);
	  PRINTER ("OutNacks",  m_outNacks);
	  PRINTER ("DropNacks", m_dropNacks);

	  PRINTER ("InData",   m_inData);
	  PRINTER ("OutData",  m_outData);
	  PRINTER ("DropData", m_dropData);

	  PRINTER ("InSatisfiedInterests", m_satisfiedInterests);
	  PRINTER ("InTimedOutInterests", m_timedOutInterests);

	  PRINTER ("OutSatisfiedInterests", m_outSatisfiedInterests);
	  PRINTER ("OutTimedOutInterests", m_outTimedOutInterests);

	  PRINTER ("InNULLps",   m_inNULLps);
	  PRINTER ("OutNULLps",  m_outNULLps);
	  PRINTER ("DropNULLps", m_dropNULLps);

	  PRINTER ("InSOs",   m_inSOs);
	  PRINTER ("OutSOs",  m_outSOs);
	  PRINTER ("DropSOs", m_dropSOs);

	  PRINTER ("InDOs",   m_inDOs);
	  PRINTER ("OutDOs",  m_outDOs);
	  PRINTER ("DropDOs", m_dropDOs);

	  PRINTER ("InDUs",   m_inDUs);
	  PRINTER ("OutDUs",  m_outDUs);
	  PRINTER ("DropDUs", m_dropDUs);

	  PRINTER ("InENs",   m_inENs);
	  PRINTER ("OutENs",  m_outENs);
	  PRINTER ("DropENs", m_dropENs);

	  PRINTER ("InOENs",   m_inOENs);
	  PRINTER ("OutOENs",  m_outOENs);
	  PRINTER ("DropOENs", m_dropOENs);

	  PRINTER ("InAENs",   m_inAENs);
	  PRINTER ("OutAENs",  m_outAENs);
	  PRINTER ("DropAENs", m_dropAENs);

	  PRINTER ("InRENs",   m_inRENs);
	  PRINTER ("OutRENs",  m_outRENs);
	  PRINTER ("DropRENs", m_dropRENs);

	  PRINTER ("InINFs",   m_inINFs);
	  PRINTER ("OutINFs",  m_outINFs);
	  PRINTER ("DropINFs", m_dropINFs);
	}

      {
	std::map<Ptr<const Face>, boost::tuple<Stats, Stats, Stats, Stats> >::iterator stats = m_stats.find (Ptr<const Face> (0));
	if (stats != m_stats.end ())
	  {
	    PRINTER ("SatisfiedInterests", m_satisfiedInterests);
	    PRINTER ("TimedOutInterests", m_timedOutInterests);
	  }
      }
    }

    void
    L3RateTracer::OutInterests  (Ptr<const ndn::Interest> interest, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outInterests ++;
      if (interest->GetWire ())
	{
	  m_stats[face].get<1> ().m_outInterests += interest->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InInterests   (Ptr<const ndn::Interest> interest, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inInterests ++;
      if (interest->GetWire ())
	{
	  m_stats[face].get<1> ().m_inInterests += interest->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropInterests (Ptr<const ndn::Interest> interest, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropInterests ++;
      if (interest->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropInterests += interest->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutNacks  (Ptr<const ndn::Interest> interest, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outNacks ++;
      if (interest->GetWire ())
	{
	  m_stats[face].get<1> ().m_outNacks += interest->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InNacks   (Ptr<const ndn::Interest> interest, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inNacks ++;
      if (interest->GetWire ())
	{
	  m_stats[face].get<1> ().m_inNacks += interest->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropNacks (Ptr<const ndn::Interest> interest, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropNacks ++;
      if (interest->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropNacks += interest->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutData  (Ptr<const ndn::Data> data,
                            bool fromCache, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outData ++;
      if (data->GetWire ())
	{
	  m_stats[face].get<1> ().m_outData += data->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InData   (Ptr<const ndn::Data> data,
                            Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inData ++;
      if (data->GetWire ())
	{
	  m_stats[face].get<1> ().m_inData += data->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropData (Ptr<const ndn::Data> data,
                            Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropData ++;
      if (data->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropData += data->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::SatisfiedInterests (Ptr<const pit::Entry> entry)
    {
      m_stats[0].get<0> ().m_satisfiedInterests ++;
      // no "size" stats

      for (pit::Entry::in_container::const_iterator i = entry->GetIncoming ().begin ();
	  i != entry->GetIncoming ().end ();
	  i++)
	{
	  m_stats[i->m_face].get<0> ().m_satisfiedInterests ++;
	}

      for (pit::Entry::out_container::const_iterator i = entry->GetOutgoing ().begin ();
	  i != entry->GetOutgoing ().end ();
	  i++)
	{
	  m_stats[i->m_face].get<0> ().m_outSatisfiedInterests ++;
	}
    }

    void
    L3RateTracer::TimedOutInterests (Ptr<const pit::Entry> entry)
    {
      m_stats[0].get<0> ().m_timedOutInterests ++;
      // no "size" stats

      for (pit::Entry::in_container::const_iterator i = entry->GetIncoming ().begin ();
	  i != entry->GetIncoming ().end ();
	  i++)
	{
	  m_stats[i->m_face].get<0> ().m_timedOutInterests ++;
	}

      for (pit::Entry::out_container::const_iterator i = entry->GetOutgoing ().begin ();
	  i != entry->GetOutgoing ().end ();
	  i++)
	{
	  m_stats[i->m_face].get<0> ().m_outTimedOutInterests ++;
	}
    }

    void
    L3RateTracer::SetAveragingPeriod (const Time &period)
    {
      m_period = period;
      m_printEvent.Cancel ();
      m_printEvent = Simulator::Schedule (m_period, &L3RateTracer::PeriodicPrinter, this);
    }

    void
    L3RateTracer::PeriodicPrinter ()
    {
      Print (*m_os);
      Reset ();

      m_printEvent = Simulator::Schedule (m_period, &L3RateTracer::PeriodicPrinter, this);
    }

    void
    L3RateTracer::OutAENs  (Ptr<const AEN> aen_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outAENs ++;
      if (aen_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outAENs += aen_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InAENs   (Ptr<const AEN> aen_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inAENs ++;
      if (aen_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inAENs += aen_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropAENs (Ptr<const AEN> aen_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropAENs ++;
      if (aen_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropAENs += aen_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutDENs  (Ptr<const DEN> den_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outDENs ++;
      if (den_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outDENs += den_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InDENs   (Ptr<const DEN> den_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inDENs ++;
      if (den_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inDENs += den_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropDENs (Ptr<const DEN> den_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropDENs ++;
      if (den_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropDENs += den_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutENs  (Ptr<const EN> en_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outENs ++;
      if (en_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outENs += en_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InENs   (Ptr<const EN> en_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inENs ++;
      if (en_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inENs += en_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropENs (Ptr<const EN> en_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropENs ++;
      if (en_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropENs += en_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutOENs  (Ptr<const OEN> oen_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outOENs ++;
      if (oen_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outOENs += oen_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InOENs   (Ptr<const OEN> oen_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inOENs ++;
      if (oen_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inOENs += oen_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropOENs (Ptr<const OEN> oen_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropOENs ++;
      if (oen_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropENs += oen_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutRENs  (Ptr<const REN> ren_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outRENs ++;
      if (ren_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outRENs += ren_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InRENs   (Ptr<const REN> ren_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inRENs ++;
      if (ren_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inRENs += ren_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropRENs (Ptr<const REN> ren_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropRENs ++;
      if (ren_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropRENs += ren_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutINFs  (Ptr<const INF> inf_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outINFs ++;
      if (inf_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outINFs += inf_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InINFs   (Ptr<const INF> inf_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inINFs ++;
      if (inf_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inINFs += inf_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropINFs (Ptr<const INF> inf_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropINFs ++;
      if (inf_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropINFs += inf_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutDOs  (Ptr<const DO> do_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outDOs ++;
      if (do_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outDOs += do_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InDOs   (Ptr<const DO> do_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inDOs ++;
      if (do_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inDOs += do_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropDOs (Ptr<const DO> do_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropDOs ++;
      if (do_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropDOs += do_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutDUs  (Ptr<const DU> du_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outDUs ++;
      if (du_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outDUs += du_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InDUs   (Ptr<const DU> du_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inDUs ++;
      if (du_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inDUs += du_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropDUs (Ptr<const DU> du_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropDUs ++;
      if (du_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropDUs += du_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutSOs  (Ptr<const SO> so_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outSOs ++;
      if (so_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outSOs += so_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InSOs   (Ptr<const SO> so_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inSOs ++;
      if (so_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inSOs += so_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropSOs (Ptr<const SO> so_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropSOs ++;
      if (so_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropSOs += so_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::OutNULLps  (Ptr<const NULLp> null_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_outNULLps ++;
      if (null_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_outNULLps += null_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::InNULLps   (Ptr<const NULLp> null_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_inNULLps ++;
      if (null_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_inNULLps += null_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::DropNULLps (Ptr<const NULLp> null_p, Ptr<const Face> face)
    {
      m_stats[face].get<0> ().m_dropNULLps ++;
      if (null_p->GetWire ())
	{
	  m_stats[face].get<1> ().m_dropNULLps += null_p->GetWire ()->GetSize ();
	}
    }

    void
    L3RateTracer::Reset ()
    {
      for (std::map<Ptr<const Face>, boost::tuple<Stats, Stats, Stats, Stats> >::iterator stats = m_stats.begin ();
	  stats != m_stats.end ();
	  stats++)
	{
	  stats->second.get<0> ().Reset ();
	  stats->second.get<1> ().Reset ();
	}
    }
  } /* namespace nnn */
} /* namespace ns3 */
