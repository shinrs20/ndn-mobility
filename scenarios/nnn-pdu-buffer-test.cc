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

  packet = wire::nnnSIM::DO::ToWire(source2);

  // Test EN packet serialization
  Ptr<nnn::EN> source3 = Create<nnn::EN> ();

  source3->AddPoa(n1_mac00.operator ns3::Address());
  source3->SetLifetime(ttl);

  packet = wire::nnnSIM::EN::ToWire(source3);

  // Test INF packet serialization
  Ptr<nnn::INF> source4 = Create<nnn::INF> ();

  source4->SetLifetime(ttl);
  source4->SetOldName(addr);
  source4->SetNewName(addr2);
  source4->SetRemainLease(release);

  packet = wire::nnnSIM::INF::ToWire(source4);

  // Test REN packet serialization
  Ptr<nnn::REN> source5 = Create<nnn::REN> ();

  source5->SetLifetime(ttl);
  source5->SetName(addr);
  source5->SetRemainLease(release);
  source5->AddPoa(n1_mac00.operator ns3::Address());
  source5->AddPoa(n1_mac01.operator ns3::Address());

  packet = wire::nnnSIM::REN::ToWire(source5);

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

  std::cout << "############################ QUEUE ########################" << std::endl;

  buf->AddDestination(addr);

  buf->PushPDU(addr, source8);
  buf->PushPDU(addr, source6);
  buf->PushPDU(addr, source2);

  std::queue<Ptr<Packet> > tmp = buf->PopQueue(addr);

  Ptr<nnn::SO> target6;
  Ptr<nnn::DO> target2;
  Ptr<nnn::DU> target8;

  while (! tmp.empty())
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
