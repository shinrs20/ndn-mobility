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
    {
    }

    PDUBuffer::~PDUBuffer ()
    {
    }

    void
    PDUBuffer::AddDestination (Ptr<NNNAddress> addr)
    {
      NS_LOG_FUNCTION(this << *addr);

      NS_LOG_INFO("Inserting " << *addr);

      std::pair< super::iterator, bool> result = super::insert(*addr, 0);

      if (result.first != super::end ())
	{
	  if (result.second)
	    {
	      NS_LOG_INFO("New buffer for : " << *addr);

	      result.first->set_payload(Create<PDUQueue> ());
	    }
	}
    }

    void
    PDUBuffer::RemoveDestination (Ptr<NNNAddress> addr)
    {
      NS_LOG_FUNCTION(this << *addr);

      NS_LOG_INFO("Deleting " << *addr);

      super::iterator item = super::find_exact(*addr);

      if (item != super::end ())
	{
	  super::erase(item);
	}
    }

    bool
    PDUBuffer::DestinationExists (Ptr<NNNAddress> addr)
    {
      NS_LOG_FUNCTION(this << *addr);
      super::iterator item = super::find_exact(*addr);

      return (item != super::end());
    }

    void
    PDUBuffer::PushSO (Ptr<NNNAddress> addr, Ptr<const SO> so_p)
    {
      NS_LOG_FUNCTION(this << *addr);

      NS_LOG_INFO ("PushPDU SO Looking for " << *addr);

      super::iterator item = super::find_exact(*addr);

      if (item != super::end ())
	{
	  NS_LOG_INFO("PushPDU SO, found " << *addr << " inserting");
	  Ptr<PDUQueue> tmp = item->payload();

	  tmp->push(Wire::FromSO(so_p, Wire::WIRE_FORMAT_NNNSIM));
	}
    }

    void
    PDUBuffer::PushDO (Ptr<NNNAddress> addr, Ptr<const DO> do_p)
    {
      NS_LOG_FUNCTION(this << *addr);

      NS_LOG_INFO ("PushPDU DO Looking for " << *addr);

      super::iterator item = super::find_exact(*addr);

      if (item != super::end ())
	{
	  NS_LOG_INFO("PushPDU DO, found " << *addr << " inserting");
	  Ptr<PDUQueue> tmp = item->payload();

	  tmp->push(Wire::FromDO(do_p, Wire::WIRE_FORMAT_NNNSIM));
	}
    }

    void
    PDUBuffer::PushDU (Ptr<NNNAddress> addr, Ptr<const DU> du_p)
    {
      NS_LOG_FUNCTION(this << *addr);

      NS_LOG_INFO ("PushPDU DU Looking for " << *addr);

      super::iterator item = super::find_exact(*addr);

      if (item != super::end ())
	{
	  NS_LOG_INFO("PushPDU DU, found " << *addr << " inserting ToWire " << *du_p);
	  Ptr<PDUQueue> tmp = item->payload();

	  tmp->push(Wire::FromDU(du_p, Wire::WIRE_FORMAT_NNNSIM));
	}
    }

    std::queue<Ptr<Packet> >
    PDUBuffer::PopQueue (Ptr<NNNAddress> addr)
    {
      NS_LOG_FUNCTION(this << *addr);

      NS_LOG_INFO ("Looking for " << *addr);

      super::iterator item = super::find_exact(*addr);

      std::queue<Ptr<Packet> > empty;

      if (item == super::end ())
	return empty;
      else
	{
	  NS_LOG_INFO("Found 3N name " << *addr);
	  if (item->payload() == 0)
	    {
	      NS_LOG_INFO("No info found");
	      return empty;
	    }
	  else
	    {
	      NS_LOG_INFO("Found info, obtaining queue");
	      return item->payload()->popQueue();
	    }
	}
    }

    uint
    PDUBuffer::QueueSize (Ptr<NNNAddress> addr)
    {
      NS_LOG_FUNCTION(this << *addr);

      NS_LOG_INFO ("Looking for " << *addr);

      super::iterator item = super::find_exact(*addr);

      if (item == super::end ())
	return 0;
      else
	{
	  NS_LOG_INFO("Found 3N name " << *addr);
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
  } /* namespace nnn */
} /* namespace ns3 */
