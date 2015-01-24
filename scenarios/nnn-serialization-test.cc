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

using namespace ns3;
using namespace std;
using namespace nnn;
using namespace wire;
using namespace nnnSIM;


int main (int argc, char *argv[])
{


  Mac48Address n1_mac00 = Mac48Address ("01:B2:03:04:05:06");
  Mac48Address n1_mac01 = Mac48Address ("01:02:03:04:05:06");

  Ptr<NNNAddress> addr = Create<NNNAddress> ("ae.34.24");
  Ptr<NNNAddress> addr2 = Create<NNNAddress> ("45.34.76");

  Time ttl = Seconds (20);
  Time lease = Seconds(120);
  Time release = Seconds (30);

  // Test DEN packet serialization
  Ptr<nnn::DEN> source = Create<nnn::DEN> ();

  source->SetName(addr);
  source->SetLifetime(ttl);
  source->AddPoa(n1_mac00.operator ns3::Address());
  source->AddPoa(n1_mac01.operator ns3::Address());

  std::cout << "Before " << std::endl << *source << std::endl;

  Ptr<Packet> packet = wire::nnnSIM::DEN::ToWire(source);

  Ptr<nnn::DEN> target = wire::nnnSIM::DEN::FromWire(packet);

  std::cout << std::endl << "After " << std::endl << *target << std::endl;

  // Test AEN packet serialization
  Ptr<nnn::AEN> source1 = Create<nnn::AEN> ();

  source1->SetName(addr);
  source1->SetLifetime(ttl);
  source1->SetLeasetime(lease);

  std::cout << std::endl << "Before" << std::endl << *source1 << std::endl;

  packet = wire::nnnSIM::AEN::ToWire(source1);

  Ptr<nnn::AEN> target1 = wire::nnnSIM::AEN::FromWire(packet);

  std::cout << std::endl << "After " << std::endl << *target1 << std::endl;

  Ptr<Packet> packet1 = Create<Packet>(*packet->Copy());

  // Test DO packet serialization
  Ptr<nnn::DO> source2 = Create<nnn::DO> ();

  source2->SetName(addr);
  source2->SetLifetime(ttl);
  source2->SetPayload(packet1);
  source2->SetPDUPayloadType(NNN_NNN);

  std::cout << std::endl << "Before" << std::endl << *source2 << std::endl;

  packet = wire::nnnSIM::DO::ToWire(source2);

  Ptr<nnn::DO> target2 = wire::nnnSIM::DO::FromWire(packet);

  std::cout << std::endl << "After " << std::endl << *target2 << std::endl;

  // Test EN packet serialization
  Ptr<nnn::EN> source3 = Create<nnn::EN> ();

  source3->AddPoa(n1_mac00.operator ns3::Address());
  source3->SetLifetime(ttl);

  std::cout << std::endl << "Before" << std::endl << *source3 << std::endl;

  packet = wire::nnnSIM::EN::ToWire(source3);

  Ptr<nnn::EN> target3 = wire::nnnSIM::EN::FromWire(packet);

  std::cout << std::endl << "Before" << std::endl << *target3 << std::endl;

  // Test INF packet serialization
  Ptr<nnn::INF> source4 = Create<nnn::INF> ();

  source4->SetLifetime(ttl);
  source4->SetOldName(addr);
  source4->SetNewName(addr2);
  source4->SetRemainLease(release);

  std::cout << std::endl << "Before" << std::endl << *source4 << std::endl;

  packet = wire::nnnSIM::INF::ToWire(source4);

  Ptr<nnn::INF> target4 = wire::nnnSIM::INF::FromWire(packet);

  std::cout << std::endl << "Before" << std::endl << *target4 << std::endl;

  // Test REN packet serialization
  Ptr<nnn::REN> source5 = Create<nnn::REN> ();

  source5->SetLifetime(ttl);
  source5->SetName(addr);
  source5->SetRemainLease(release);
  source5->AddPoa(n1_mac00.operator ns3::Address());
  source5->AddPoa(n1_mac01.operator ns3::Address());

  std::cout << std::endl << "Before" << std::endl << *source5 << std::endl;

  packet = wire::nnnSIM::REN::ToWire(source5);

  Ptr<nnn::REN> target5 = wire::nnnSIM::REN::FromWire(packet);

  std::cout << std::endl << "After" << std::endl << *target5 << std::endl;

  // Test SO packet serialization
  Ptr<nnn::SO> source6 = Create<nnn::SO> ();

  source6->SetName(addr);
  source6->SetLifetime(ttl);
  source6->SetPayload(packet1);
  source6->SetPDUPayloadType(NNN_NNN);

  std::cout << std::endl << "Before" << std::endl << *source6 << std::endl;

  packet = wire::nnnSIM::SO::ToWire(source6);

  Ptr<nnn::SO> target6 = wire::nnnSIM::SO::FromWire(packet);

  std::cout << std::endl << "After " << std::endl << *target6 << std::endl;

  // Test NULLp packet serialization
  Ptr<nnn::NULLp> source7 = Create<nnn::NULLp> ();

  source7->SetLifetime(ttl);
  source7->SetPayload(packet1);
  source7->SetPDUPayloadType(NNN_NNN);

  std::cout << std::endl << "Before" << std::endl << *source7 << std::endl;

  packet = wire::nnnSIM::NULLp::ToWire(source7);

  Ptr<nnn::NULLp> target7 = wire::nnnSIM::NULLp::FromWire(packet);

  std::cout << std::endl << "After " << std::endl << *target7 << std::endl;

  // Test DU packet serialization
  Ptr<nnn::DU> source8 = Create<nnn::DU> ();

  source8->SetLifetime(ttl);
  source8->SetPayload(packet1);
  source8->SetPDUPayloadType(NNN_NNN);
  source8->SetSrcName(addr);
  source8->SetDstName(addr2);

  std::cout << std::endl << "Before" << std::endl << *source8 << std::endl;

  packet = wire::nnnSIM::DU::ToWire(source8);

  Ptr<nnn::DU> target8 = wire::nnnSIM::DU::FromWire(packet);

  std::cout << std::endl << "After " << std::endl << *target8 << std::endl;
}
