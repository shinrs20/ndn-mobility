/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-pdu-buffer.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pdu-buffer.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pdu-buffer.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>

#include <ns3-dev/ns3/log.h>

#include "nnn-pdu-buffer.h"

NS_LOG_COMPONENT_DEFINE ("nnn.PDUBuffer");

namespace ns3
{
  namespace nnn
  {
    TypeId
    PDUBuffer::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::PacketBuffer")
        	.SetParent<Object> ()
		.AddConstructor<PDUBuffer> ()
		;
      return tid;
    }

    PDUBuffer::PDUBuffer ()
    : m_retx (MilliSeconds (50))
    {
    }

    PDUBuffer::PDUBuffer (Time retx)
    : m_retx (retx)
    {
    }

    PDUBuffer::~PDUBuffer ()
    {
    }

    void
    PDUBuffer::AddDestination (const NNNAddress &addr)
    {
      NS_LOG_FUNCTION(this << addr);

      NS_LOG_INFO("Inserting " << addr);

      std::pair< super::iterator, bool> result = super::insert(addr, 0);

      if (result.first != super::end ())
	{
	  if (result.second)
	    {
	      NS_LOG_INFO("New buffer for : " << addr);

	      result.first->set_payload(Create<PDUQueue> ());
	    }
	}
    }

    void
    PDUBuffer::AddDestination (Ptr<NNNAddress> addr)
    {
      AddDestination (*addr);
    }

    void
    PDUBuffer::RemoveDestination (const NNNAddress &addr)
    {
      NS_LOG_FUNCTION(this << addr);

      NS_LOG_INFO("Deleting " << addr);

      super::iterator item = super::find_exact(addr);

      if (item != super::end ())
	{
	  super::erase(item);
	}
    }

    void
    PDUBuffer::RemoveDestination (Ptr<NNNAddress> addr)
    {
     RemoveDestination (*addr);
    }

    bool
    PDUBuffer::DestinationExists (const NNNAddress &addr)
    {
      NS_LOG_FUNCTION(this << addr);
      super::iterator item = super::find_exact(addr);

      return (item != super::end());
    }

    bool
    PDUBuffer::DestinationExists (Ptr<NNNAddress> addr)
    {
      DestinationExists (*addr);
    }

    void
    PDUBuffer::PushSO (const NNNAddress &addr, Ptr<const SO> so_p)
    {
      NS_LOG_FUNCTION(this << addr);

      NS_LOG_INFO ("PushPDU SO Looking for " << addr);

      super::iterator item = super::find_exact(addr);

      if (item != super::end ())
	{
	  NS_LOG_INFO("PushPDU SO, found " << addr << " inserting");
	  Ptr<PDUQueue> tmp = item->payload();

	  tmp->pushSO(so_p, m_retx);
	}
    }

    void
    PDUBuffer::PushSO (Ptr<NNNAddress> addr, Ptr<const SO> so_p)
    {
      PushSO (*addr, so_p);
    }

    void
    PDUBuffer::PushDO (const NNNAddress& addr, Ptr<const DO> do_p)
    {
      NS_LOG_FUNCTION(this << addr);

      NS_LOG_INFO ("PushPDU DO Looking for " << addr);

      super::iterator item = super::find_exact(addr);

      if (item != super::end ())
	{
	  NS_LOG_INFO("PushPDU DO, found " << addr << " inserting");
	  Ptr<PDUQueue> tmp = item->payload();

	  tmp->pushDO(do_p, m_retx);
	}
    }

    void
    PDUBuffer::PushDO (Ptr<NNNAddress> addr, Ptr<const DO> do_p)
    {
      PushDO (*addr, do_p);
    }

    void
    PDUBuffer::PushDU (const NNNAddress &addr, Ptr<const DU> du_p)
    {
      NS_LOG_FUNCTION(this << addr);

      NS_LOG_INFO ("PushPDU DU Looking for " << addr);

      super::iterator item = super::find_exact(addr);

      if (item != super::end ())
	{
	  NS_LOG_INFO("PushPDU DU, found " << addr << " inserting");
	  Ptr<PDUQueue> tmp = item->payload();

	  tmp->pushDU(du_p, m_retx);
	}
    }

    void
    PDUBuffer::PushDU (Ptr<NNNAddress> addr, Ptr<const DU> du_p)
    {
      PushDU (*addr, du_p);
    }

    std::queue<Ptr<Packet> >
    PDUBuffer::PopQueue (const NNNAddress &addr)
    {
      NS_LOG_FUNCTION(this << addr);

      NS_LOG_INFO ("Looking for (" << addr << ")");

      super::iterator item = super::find_exact(addr);

      std::queue<Ptr<Packet> > pdu_queue;
      uint32_t pushed = 0;
      uint32_t discarded = 0;

      if (item != super::end ())
	{
	  NS_LOG_INFO("Found 3N name (" << addr << ")");
	  if (item->payload() == 0)
	    {
	      NS_LOG_INFO("No info found");
	    }
	  else
	    {
	      NS_LOG_INFO("Found info, obtaining queue");
	      std::queue<std::pair<Time, Ptr<Packet> > > queue_with_time = item->payload()->popQueue();

	      Time now = Simulator::Now ();

	      // Go through the queue and get those PDUs that haven't yet hit the retransmission time
	      while (!queue_with_time.empty ())
		{
		  // Get the PDU at the front of the queue
		  std::pair<Time,Ptr<Packet> > queue_pair = queue_with_time.front ();

		  Time expiry = queue_pair.first;

		  // If the expiry time has not been reached, add it to the returning queue
		  if (now <= expiry)
		    {
		      NS_LOG_DEBUG ("Now is " << now << " PDU has expiry of " << expiry << " inserting PDU");

		      pdu_queue.push (queue_pair.second);

		      pushed++;
		    }
		  else
		    {
		      NS_LOG_DEBUG ("Now is " << now << " PDU has expiry of " << expiry << " discarding PDU");
		      discarded++;
		    }

		  // Pop the queue and continue
		  queue_with_time.pop ();
		}
	    }
	}

      NS_LOG_INFO ("Buffer for (" << addr << ") being returned with " << std::dec << pushed << " PDUs having discarded " << discarded);

      return pdu_queue;
    }

    std::queue<Ptr<Packet> >
    PDUBuffer::PopQueue (Ptr<NNNAddress> addr)
    {
      return PopQueue (*addr);
    }

    uint
    PDUBuffer::QueueSize (const NNNAddress &addr)
    {
      NS_LOG_FUNCTION(this << addr);

      NS_LOG_INFO ("Looking for " << addr);

      super::iterator item = super::find_exact(addr);

      if (item == super::end ())
	return 0;
      else
	{
	  NS_LOG_INFO("Found 3N name " << addr);
	  if (item->payload() == 0)
	    {
	      NS_LOG_INFO("No info found");
	      return 0;
	    }
	  else
	    {
	      NS_LOG_INFO("Found info, obtaining queue");
	      return item->payload()->size ();
	    }
	}
    }

    uint
    PDUBuffer::QueueSize (Ptr<NNNAddress> addr)
    {
      return QueueSize (*addr);
    }

    void
    PDUBuffer::SetReTX (Time rtx)
    {
      m_retx = rtx;
    }

    Time
    PDUBuffer::GetReTX () const
    {
      return m_retx;
    }
  } /* namespace nnn */
} /* namespace ns3 */
