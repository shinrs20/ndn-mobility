/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-l3-tracer.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-l3-tracer.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-l3-tracer.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _NNN_L3_TRACER_H_
#define _NNN_L3_TRACER_H_

#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/simple-ref-count.h>

namespace ns3
{
  class Node;
  class Packet;

  namespace ndn
  {
    class Interest;
    class Data;
  }

  namespace nnn
  {

    namespace pit {
      class Entry;
    }

    class Face;
    class EN;
    class OEN;
    class AEN;
    class REN;
    class DEN;
    class NULLp;
    class SO;
    class DO;
    class DU;
    class INF;

    class L3Tracer : public SimpleRefCount<L3Tracer>
    {
    public:
      /**
       * @brief Trace constructor that attaches to the node using node pointer
       * @param node  pointer to the node
       */
      L3Tracer (Ptr<Node> node);

      /**
       * @brief Trace constructor that attaches to the node using node name
       * @param nodeName  name of the node registered using Names::Add
       */
      L3Tracer (const std::string &node);

      /**
       * @brief Destructor
       */
      virtual ~L3Tracer ();

      /**
       * @brief Print head of the trace (e.g., for post-processing)
       *
       * @param os reference to output stream
       */
      virtual void
      PrintHeader (std::ostream &os) const = 0;

      /**
       * @brief Print current trace data
       *
       * @param os reference to output stream
       */
      virtual void
      Print (std::ostream &os) const = 0;

    protected:
      void
      Connect ();

      virtual void
      OutInterests  (Ptr<const ndn::Interest>, Ptr<const Face>) = 0;

      virtual void
      InInterests   (Ptr<const ndn::Interest>, Ptr<const Face>) = 0;

      virtual void
      DropInterests (Ptr<const ndn::Interest>, Ptr<const Face>) = 0;

      virtual void
      OutNacks  (Ptr<const ndn::Interest>, Ptr<const Face>) = 0;

      virtual void
      InNacks   (Ptr<const ndn::Interest>, Ptr<const Face>) = 0;

      virtual void
      DropNacks (Ptr<const ndn::Interest>, Ptr<const Face>) = 0;

      virtual void
      OutData  (Ptr<const ndn::Data>, bool fromCache, Ptr<const Face>) = 0;

      virtual void
      InData   (Ptr<const ndn::Data>, Ptr<const Face>) = 0;

      virtual void
      DropData (Ptr<const ndn::Data>, Ptr<const Face>) = 0;

      virtual void
      SatisfiedInterests (Ptr<const pit::Entry>) = 0;

      virtual void
      TimedOutInterests (Ptr<const pit::Entry>) = 0;

      virtual void
      OutAENs  (Ptr<const AEN>, Ptr<const Face>) = 0;

      virtual void
      InAENs   (Ptr<const AEN>, Ptr<const Face>) = 0;

      virtual void
      DropAENs (Ptr<const AEN>, Ptr<const Face>) = 0;

      virtual void
      OutDENs  (Ptr<const DEN>, Ptr<const Face>) = 0;

      virtual void
      InDENs   (Ptr<const DEN>, Ptr<const Face>) = 0;

      virtual void
      DropDENs (Ptr<const DEN>, Ptr<const Face>) = 0;

      virtual void
      OutENs  (Ptr<const EN>, Ptr<const Face>) = 0;

      virtual void
      InENs   (Ptr<const EN>, Ptr<const Face>) = 0;

      virtual void
      DropENs (Ptr<const EN>, Ptr<const Face>) = 0;

      virtual void
      OutOENs  (Ptr<const OEN>, Ptr<const Face>) = 0;

      virtual void
      InOENs   (Ptr<const OEN>, Ptr<const Face>) = 0;

      virtual void
      DropOENs (Ptr<const OEN>, Ptr<const Face>) = 0;

      virtual void
      OutRENs  (Ptr<const REN>, Ptr<const Face>) = 0;

      virtual void
      InRENs   (Ptr<const REN>, Ptr<const Face>) = 0;

      virtual void
      DropRENs (Ptr<const REN>, Ptr<const Face>) = 0;

      virtual void
      OutINFs  (Ptr<const INF>, Ptr<const Face>) = 0;

      virtual void
      InINFs   (Ptr<const INF>, Ptr<const Face>) = 0;

      virtual void
      DropINFs (Ptr<const INF>, Ptr<const Face>) = 0;

      virtual void
      OutDOs  (Ptr<const DO>, Ptr<const Face>) = 0;

      virtual void
      InDOs   (Ptr<const DO>, Ptr<const Face>) = 0;

      virtual void
      DropDOs (Ptr<const DO>, Ptr<const Face>) = 0;

      virtual void
      OutDUs  (Ptr<const DU>, Ptr<const Face>) = 0;

      virtual void
      InDUs   (Ptr<const DU>, Ptr<const Face>) = 0;

      virtual void
      DropDUs (Ptr<const DU>, Ptr<const Face>) = 0;

      virtual void
      OutSOs  (Ptr<const SO>, Ptr<const Face>) = 0;

      virtual void
      InSOs   (Ptr<const SO>, Ptr<const Face>) = 0;

      virtual void
      DropSOs (Ptr<const SO>, Ptr<const Face>) = 0;

      virtual void
      OutNULLps  (Ptr<const NULLp>, Ptr<const Face>) = 0;

      virtual void
      InNULLps   (Ptr<const NULLp>, Ptr<const Face>) = 0;

      virtual void
      DropNULLps (Ptr<const NULLp>, Ptr<const Face>) = 0;

    protected:
      std::string m_node;
      Ptr<Node> m_nodePtr;

      struct Stats
      {
	inline void Reset ()
	{
	  m_inInterests   = 0;
	  m_outInterests  = 0;
	  m_dropInterests = 0;
	  m_inNacks       = 0;
	  m_outNacks      = 0;
	  m_dropNacks     = 0;
	  m_inData        = 0;
	  m_outData       = 0;
	  m_dropData      = 0;
	  m_satisfiedInterests = 0;
	  m_timedOutInterests = 0;

	  m_outSatisfiedInterests = 0;
	  m_outTimedOutInterests = 0;

	  m_inAENs = 0;
	  m_outAENs = 0;
	  m_dropAENs = 0;

	  m_inDENs = 0;
	  m_outDENs = 0;
	  m_dropDENs = 0;

	  m_inDOs = 0;
	  m_outDOs = 0;
	  m_dropDOs = 0;

	  m_inDUs = 0;
	  m_outDUs = 0;
	  m_dropDUs = 0;

	  m_inENs = 0;
	  m_outENs = 0;
	  m_dropENs = 0;

	  m_inINFs = 0;
	  m_outINFs = 0;
	  m_dropINFs = 0;

	  m_inNULLps = 0;
	  m_outNULLps = 0;
	  m_dropNULLps = 0;

	  m_inOENs = 0;
	  m_outOENs = 0 ;
	  m_dropOENs = 0;

	  m_inRENs = 0;
	  m_outRENs = 0;
	  m_dropRENs = 0;

	  m_inSOs = 0;
	  m_outSOs = 0;
	  m_dropSOs = 0;
	}

	double m_inInterests;
	double m_outInterests;
	double m_dropInterests;
	double m_inNacks;
	double m_outNacks;
	double m_dropNacks;
	double m_inData;
	double m_outData;
	double m_dropData;
	double m_satisfiedInterests;
	double m_timedOutInterests;
	double m_outSatisfiedInterests;
	double m_outTimedOutInterests;

	double m_inAENs;
	double m_outAENs;
	double m_dropAENs;

	double m_inDENs;
	double m_outDENs;
	double m_dropDENs;

	double m_inDOs;
	double m_outDOs;
	double m_dropDOs;

	double m_inDUs;
	double m_outDUs;
	double m_dropDUs;

	double m_inENs;
	double m_outENs;
	double m_dropENs;

	double m_inINFs;
	double m_outINFs;
	double m_dropINFs;

	double m_inNULLps;
	double m_outNULLps;
	double m_dropNULLps;

	double m_inOENs;
	double m_outOENs;
	double m_dropOENs;

	double m_inRENs;
	double m_outRENs;
	double m_dropRENs;

	double m_inSOs;
	double m_outSOs;
	double m_dropSOs;
      };
    };

    /**
     * @brief Helper to dump the trace to an output stream
     */
    inline std::ostream&
    operator << (std::ostream &os, const L3Tracer &tracer)
    {
      os << "# ";
      tracer.PrintHeader (os);
      os << "\n";
      tracer.Print (os);
      return os;
    }
  } /* namespace nnn */
} /* namespace ns3 */

#endif /* _NNN_L3_TRACER_H_ */
