/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template by University of California written by
 *   Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *
 *  nnn-app-delay-tracer.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-app-delay-tracer.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-app-delay-tracer.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "nnn-app-delay-tracer.h"

#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/config.h>
#include <ns3-dev/ns3/names.h>
#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/node-list.h>
#include <ns3-dev/ns3/log.h>

#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-data.h>

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <fstream>

#include "../../apps/nnn-app.h"

NS_LOG_COMPONENT_DEFINE ("nnn.AppDelayTracer");

namespace ns3
{
  namespace nnn
  {
    static std::list< boost::tuple< boost::shared_ptr<std::ostream>, std::list<Ptr<AppDelayTracer> > > > g_tracers;

    template<class T>
    static inline void
    NullDeleter (T *ptr)
    {
    }

    AppDelayTracer::AppDelayTracer (boost::shared_ptr<std::ostream> os, Ptr<Node> node)
    : m_nodePtr (node)
    , m_os (os)
    {
      m_node = boost::lexical_cast<std::string> (m_nodePtr->GetId ());

      Connect ();

      std::string name = Names::FindName (node);
      if (!name.empty ())
	{
	  m_node = name;
	}
    }

    AppDelayTracer::AppDelayTracer (boost::shared_ptr<std::ostream> os, const std::string &node)
    : m_node (node)
    , m_os (os)
    {
      Connect ();
    }

    AppDelayTracer::~AppDelayTracer ()
    {
    }

    void
    AppDelayTracer::Install (Ptr<Node> node, const std::string &file)
    {
      std::list<Ptr<AppDelayTracer> > tracers;
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

      Ptr<AppDelayTracer> trace = Install (node, outputStream);
      tracers.push_back (trace);

      if (tracers.size () > 0)
	{
	  // *m_l3RateTrace << "# "; // not necessary for R's read.table
	  tracers.front ()->PrintHeader (*outputStream);
	  *outputStream << "\n";
	}

      g_tracers.push_back (boost::make_tuple (outputStream, tracers));
    }

    Ptr<AppDelayTracer>
    AppDelayTracer::Install (Ptr<Node> node,
                             boost::shared_ptr<std::ostream> outputStream)
    {
      NS_LOG_DEBUG ("Node: " << node->GetId ());

      Ptr<AppDelayTracer> trace = Create<AppDelayTracer> (outputStream, node);

      return trace;
    }

    void
    AppDelayTracer::Install (const NodeContainer &nodes, const std::string &file)
    {
      std::list<Ptr<AppDelayTracer> > tracers;
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
	  Ptr<AppDelayTracer> trace = Install (*node, outputStream);
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
    AppDelayTracer::InstallAll (const std::string &file)
    {
      using namespace boost;
      using namespace std;

      std::list<Ptr<AppDelayTracer> > tracers;
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
	  Ptr<AppDelayTracer> trace = Install (*node, outputStream);
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
    AppDelayTracer::Destroy ()
    {
      g_tracers.clear ();
    }

    void
    AppDelayTracer::PrintHeader (std::ostream &os) const
    {
      os << "Time" << "\t"
	  << "Node" << "\t"
	  << "AppId" << "\t"
	  << "SeqNo" << "\t"

	  << "Type" << "\t"
	  << "DelayS" << "\t"
	  << "DelayUS" << "\t"
	  << "RetxCount" << "\t"
	  << "HopCount"  << "";
    }

    void
    AppDelayTracer::Connect ()
    {
      Config::ConnectWithoutContext ("/NodeList/"+m_node+"/ApplicationList/*/LastRetransmittedInterestDataDelay",
                                     MakeCallback (&AppDelayTracer::LastRetransmittedInterestDataDelay, this));

      Config::ConnectWithoutContext ("/NodeList/"+m_node+"/ApplicationList/*/FirstInterestDataDelay",
                                     MakeCallback (&AppDelayTracer::FirstInterestDataDelay, this));
    }

    void
    AppDelayTracer::LastRetransmittedInterestDataDelay (Ptr<App> app, uint32_t seqno, Time delay, int32_t hopCount)
    {
      *m_os << Simulator::Now ().ToDouble (Time::S) << "\t"
	  << m_node << "\t"
	  << app->GetId () << "\t"
	  << seqno << "\t"
	  << "LastDelay" << "\t"
	  << delay.ToDouble (Time::S) << "\t"
	  << delay.ToDouble (Time::US) << "\t"
	  << 1 << "\t"
	  << hopCount << "\n";
    }

    void
    AppDelayTracer::FirstInterestDataDelay (Ptr<App> app, uint32_t seqno, Time delay, uint32_t retxCount, int32_t hopCount)
    {
      *m_os << Simulator::Now ().ToDouble (Time::S) << "\t"
	  << m_node << "\t"
	  << app->GetId () << "\t"
	  << seqno << "\t"
	  << "FullDelay" << "\t"
	  << delay.ToDouble (Time::S) << "\t"
	  << delay.ToDouble (Time::US) << "\t"
	  << retxCount << "\t"
	  << hopCount << "\n";
    }
  } /* namespace nnn */
} /* namespace ns3 */
