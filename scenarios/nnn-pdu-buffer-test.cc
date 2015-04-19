/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnst-test.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnst-test.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnst-test.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ns3-dev/ns3/core-module.h>
#include <ns3-dev/ns3/network-module.h>

// Extensions
#include "nnnSIM/nnnSIM-module.h"

#include "nnnSIM/model/buffers/nnn-pdu-buffer.h"
#include "nnnSIM/model/buffers/nnn-pdu-buffer-queue.h"
#include "nnnSIM/helper/nnn-header-helper.h"

using namespace ns3;
using namespace std;
using namespace nnn;
using namespace wire;

void dealQueue (Ptr<PDUBuffer> buf, NNNAddress &addr)
{
  Ptr<nnn::SO> target6;
  Ptr<nnn::DO> target2;
  Ptr<nnn::DU> target8;

  std::queue<Ptr<Packet> > tmp = buf->PopQueue(addr);
  std::cout << "Queue size for " << addr << " received " << tmp.size () << std::endl;

  std::cout << "---- Dealing with queue for " << addr << std::endl;
  while (!tmp.empty())
    {
      Ptr<Packet> tmp2 = tmp.front ();

      switch (HeaderHelper::GetNNNHeaderType(tmp2))
      {
	case SO_NNN:
	  target6 = wire::nnnSIM::SO::FromWire(tmp2);
	  std::cout << std::endl << "After " << std::endl << *target6 << std::endl;
	  break;
	case DO_NNN:
	  target2 = wire::nnnSIM::DO::FromWire(tmp2);
	  std::cout << std::endl << "After " << std::endl << *target2 << std::endl;
	  break;
	case DU_NNN:
	  target8 = wire::nnnSIM::DU::FromWire(tmp2);
	  std::cout << std::endl << "After " << std::endl << *target8 << std::endl;
	  break;
	default:
	  std::cout << "Something unexpected happened" << std::endl;
      }
      tmp.pop ();
    }
}

int main (int argc, char *argv[])
{

  Mac48Address n1_mac00 = Mac48Address ("01:B2:03:04:05:06");
  Mac48Address n1_mac01 = Mac48Address ("01:02:03:04:05:06");

  Ptr<NNNAddress> addr = Create<NNNAddress> ("ae.34.24");
  Ptr<NNNAddress> addr2 = Create<NNNAddress> ("45.34.76");
  Ptr<NNNAddress> addr3 = Create<NNNAddress> ("45.34.5");
  Ptr<NNNAddress> addr4 = Create<NNNAddress> ("45.34");
  Ptr<NNNAddress> addr5 = Create<NNNAddress> ("ae.34.3");
  Ptr<NNNAddress> addr6 = Create<NNNAddress> ("6");

  Time ttl = Seconds (20);
  Time lease = Seconds(120);
  Time release = Seconds (30);

  // Test DEN packet serialization
  Ptr<nnn::DEN> source = Create<nnn::DEN> ();

  source->SetName(addr);
  source->SetLifetime(ttl);
  source->AddPoa(n1_mac00.operator ns3::Address());
  source->AddPoa(n1_mac01.operator ns3::Address());

  Ptr<Packet> packet = wire::nnnSIM::DEN::ToWire(source);

  // Test AEN packet serialization
  Ptr<nnn::AEN> source1 = Create<nnn::AEN> ();

  source1->SetName(addr);
  source1->SetLifetime(ttl);
  source1->SetLeasetime(lease);

  packet = wire::nnnSIM::AEN::ToWire(source1);

  Ptr<Packet> packet1 = Create<Packet>(*packet->Copy());

  // Test DO packet serialization
  Ptr<nnn::DO> source2 = Create<nnn::DO> ();

  source2->SetName(addr);
  source2->SetLifetime(ttl);
  source2->SetPayload(packet1);
  source2->SetPDUPayloadType(NNN_NNN);

  std::cout << std::endl << "Before" << std::endl << *source2 << std::endl;

  // Test SO packet serialization
  Ptr<nnn::SO> source6 = Create<nnn::SO> ();

  source6->SetName(addr);
  source6->SetLifetime(ttl);
  source6->SetPayload(packet1);
  source6->SetPDUPayloadType(NNN_NNN);

  std::cout << std::endl << "Before" << std::endl << *source6 << std::endl;

  packet = wire::nnnSIM::SO::ToWire(source6);

  // Test DU packet serialization
  Ptr<nnn::DU> source8 = Create<nnn::DU> ();

  source8->SetLifetime(ttl);
  source8->SetPayload(packet1);
  source8->SetPDUPayloadType(NNN_NNN);
  source8->SetSrcName(addr);
  source8->SetDstName(addr2);

  std::cout << std::endl << "Before" << std::endl << *source8 << std::endl;

  packet = wire::nnnSIM::DU::ToWire(source8);

  Ptr<PDUBuffer> buf= Create<PDUBuffer> ();

  std::cout << "Queue size for " << *addr << ": " << buf->QueueSize(addr) << std::endl;

  std::cout << "############################ QUEUE ########################" << std::endl;

  buf->AddDestination(addr);
  buf->AddDestination(addr2);
  buf->AddDestination(addr3);

  std::cout << "Push DU" << std::endl;
  buf->PushDU(addr, source8);
  buf->PushDU(addr2, source8);
  std::cout << "Push DO" << std::endl;
  buf->PushDO(addr, source2);
  buf->PushDO(addr3, source2);
  std::cout << "Push SO" << std::endl;
  buf->PushSO(addr, source6);
  buf->PushSO(addr3, source6);

  std::cout << "Queue size for " << *addr << " after pushes : " << buf->QueueSize(addr) << std::endl;
  std::cout << "Queue size for " << *addr2 << " after pushes : " << buf->QueueSize(addr2) << std::endl;
  std::cout << "Queue size for " << *addr3 << " after pushes : " << buf->QueueSize(addr3) << std::endl;

  buf->RemoveDestination(addr3);

  if (!buf->DestinationExists(addr3))
    std::cout << "Queue for " << *addr3 << " doesn't exist!" << std::endl;

  Simulator::Schedule(MilliSeconds (20), &dealQueue, buf, *addr2);

  Simulator::Schedule(MilliSeconds (60), &dealQueue, buf, *addr);

  Simulator::Stop (Seconds (20));
  Simulator::Run ();
  Simulator::Destroy ();

}
