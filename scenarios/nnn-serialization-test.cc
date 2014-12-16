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
#include "nnnSIM/model/wire/nnnsim/testDEN/nnnsim-testDEN.h"

using namespace ns3;
using namespace std;
using namespace nnn;
using namespace wire;
using namespace nnnSIM;


int main (int argc, char *argv[])
{
  Ptr<nnn::DEN> source = Create<nnn::DEN> ();
  Mac48Address n1_mac00 = Mac48Address ("01:B2:03:04:05:06");
  Mac48Address n1_mac01 = Mac48Address ("01:02:03:04:05:06");

  source->SetName(Create<NNNAddress> ("ae.34.24"));
  source->SetLifetime(Seconds (20));
  source->AddPoa(n1_mac00.operator ns3::Address());
  source->AddPoa(n1_mac01.operator ns3::Address());

  std::cout << "Before " << std::endl << *source << std::endl;

  Ptr<Packet> packet = wire::nnnSIM::TestDEN::ToWire(source);

  Ptr<nnn::DEN> target = wire::nnnSIM::TestDEN::FromWire(packet);

  std::cout << std::endl << "After " << std::endl << *target << std::endl;

}
