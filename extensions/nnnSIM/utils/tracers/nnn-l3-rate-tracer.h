/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-l3-rate-tracer.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-l3-rate-tracer.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-l3-rate-tracer.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef NNN_L3_RATE_TRACER_H_
#define NNN_L3_RATE_TRACER_H_

#include "nnn-l3-tracer.h"

#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/event-id.h>
#include <ns3-dev/ns3/node-container.h>

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <list>

namespace ns3
{
  namespace nnn
  {

    class L3RateTracer : public L3Tracer
    {
    public:
      /**
       * @brief Trace constructor that attaches to the node using node pointer
       * @param os    reference to the output stream
       * @param node  pointer to the node
       */
      L3RateTracer (boost::shared_ptr<std::ostream> os, Ptr<Node> node);

      /**
       * @brief Trace constructor that attaches to the node using node name
       * @param os        reference to the output stream
       * @param nodeName  name of the node registered using Names::Add
       */
      L3RateTracer (boost::shared_ptr<std::ostream> os, const std::string &node);

      /**
       * @brief Destructor
       */
      virtual ~L3RateTracer ();

      /**
       * @brief Helper method to install tracers on all simulation nodes
       *
       * @param file File to which traces will be written.  If filename is -, then std::out is used
       * @param averagingPeriod Defines averaging period for the rate calculation,
       *        as well as how often data will be written into the trace file (default, every half second)
       */
      static void
      InstallAll (const std::string &file, Time averagingPeriod = Seconds (0.5));

      /**
       * @brief Helper method to install tracers on the selected simulation nodes
       *
       * @param nodes Nodes on which to install tracer
       * @param file File to which traces will be written.  If filename is -, then std::out is used
       * @param averagingPeriod How often data will be written into the trace file (default, every half second)
       */
      static void
      Install (const NodeContainer &nodes, const std::string &file, Time averagingPeriod = Seconds (0.5));

      /**
       * @brief Helper method to install tracers on a specific simulation node
       *
       * @param nodes Nodes on which to install tracer
       * @param file File to which traces will be written.  If filename is -, then std::out is used
       * @param averagingPeriod How often data will be written into the trace file (default, every half second)
       */
      static void
      Install (Ptr<Node> node, const std::string &file, Time averagingPeriod = Seconds (0.5));

      /**
       * @brief Explicit request to remove all statically created tracers
       *
       * This method can be helpful if simulation scenario contains several independent run,
       * or if it is desired to do a postprocessing of the resulting data
       */
      static void
      Destroy ();

      /**
       * @brief Helper method to install tracers on a specific simulation node
       *
       * @param nodes Nodes on which to install tracer
       * @param outputStream Smart pointer to a stream
       * @param averagingPeriod How often data will be written into the trace file (default, every half second)
       *
       * @returns a tuple of reference to output stream and list of tracers. !!! Attention !!! This tuple needs to be preserved
       *          for the lifetime of simulation, otherwise SEGFAULTs are inevitable
       */
      static Ptr<L3RateTracer>
      Install (Ptr<Node> node, boost::shared_ptr<std::ostream> outputStream, Time averagingPeriod = Seconds (0.5));

      // from L3Tracer
      virtual void
      PrintHeader (std::ostream &os) const;

      virtual void
      Print (std::ostream &os) const;

    protected:
      // from L3Tracer
      virtual void
      OutInterests  (Ptr<const ndn::Interest>, Ptr<const Face>);

      virtual void
      InInterests   (Ptr<const ndn::Interest>, Ptr<const Face>);

      virtual void
      DropInterests (Ptr<const ndn::Interest>, Ptr<const Face>);

      virtual void
      OutNacks  (Ptr<const ndn::Interest>, Ptr<const Face>);

      virtual void
      InNacks   (Ptr<const ndn::Interest>, Ptr<const Face>);

      virtual void
      DropNacks (Ptr<const ndn::Interest>, Ptr<const Face>);

      virtual void
      OutData  (Ptr<const ndn::Data>, bool fromCache, Ptr<const Face>);

      virtual void
      InData   (Ptr<const ndn::Data>, Ptr<const Face>);

      virtual void
      DropData (Ptr<const ndn::Data>, Ptr<const Face>);

      virtual void
      SatisfiedInterests (Ptr<const pit::Entry>);

      virtual void
      TimedOutInterests (Ptr<const pit::Entry>);

      virtual void
      OutAENs  (Ptr<const AEN>, Ptr<const Face>);

      virtual void
      InAENs   (Ptr<const AEN>, Ptr<const Face>);

      virtual void
      DropAENs (Ptr<const AEN>, Ptr<const Face>);

      virtual void
      OutDENs  (Ptr<const DEN>, Ptr<const Face>);

      virtual void
      InDENs   (Ptr<const DEN>, Ptr<const Face>);

      virtual void
      DropDENs (Ptr<const DEN>, Ptr<const Face>);

      virtual void
      OutENs  (Ptr<const EN>, Ptr<const Face>);

      virtual void
      InENs   (Ptr<const EN>, Ptr<const Face>);

      virtual void
      DropENs (Ptr<const EN>, Ptr<const Face>);

      virtual void
      OutOENs  (Ptr<const OEN>, Ptr<const Face>);

      virtual void
      InOENs   (Ptr<const OEN>, Ptr<const Face>);

      virtual void
      DropOENs (Ptr<const OEN>, Ptr<const Face>);

      virtual void
      OutRENs  (Ptr<const REN>, Ptr<const Face>);

      virtual void
      InRENs   (Ptr<const REN>, Ptr<const Face>);

      virtual void
      DropRENs (Ptr<const REN>, Ptr<const Face>);

      virtual void
      OutINFs  (Ptr<const INF>, Ptr<const Face>);

      virtual void
      InINFs   (Ptr<const INF>, Ptr<const Face>);

      virtual void
      DropINFs (Ptr<const INF>, Ptr<const Face>);

      virtual void
      OutDOs  (Ptr<const DO>, Ptr<const Face>);

      virtual void
      InDOs   (Ptr<const DO>, Ptr<const Face>);

      virtual void
      DropDOs (Ptr<const DO>, Ptr<const Face>);

      virtual void
      OutDUs  (Ptr<const DU>, Ptr<const Face>);

      virtual void
      InDUs   (Ptr<const DU>, Ptr<const Face>);

      virtual void
      DropDUs (Ptr<const DU>, Ptr<const Face>);

      virtual void
      OutSOs  (Ptr<const SO>, Ptr<const Face>);

      virtual void
      InSOs   (Ptr<const SO>, Ptr<const Face>);

      virtual void
      DropSOs (Ptr<const SO>, Ptr<const Face>);

      virtual void
      OutNULLps  (Ptr<const NULLp>, Ptr<const Face>);

      virtual void
      InNULLps   (Ptr<const NULLp>, Ptr<const Face>);

      virtual void
      DropNULLps (Ptr<const NULLp>, Ptr<const Face>);

    private:
        void
        SetAveragingPeriod (const Time &period);

        void
        PeriodicPrinter ();

        void
        Reset ();

    private:
        boost::shared_ptr<std::ostream> m_os;
        Time m_period;
        EventId m_printEvent;

        mutable std::map<Ptr<const Face>, boost::tuple<Stats, Stats, Stats, Stats> > m_stats;
    };

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_L3_RATE_TRACER_H_ */
