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
#include <ns3-dev/ns3/mac48-address.h>

// Extensions
#include "nnnSIM/nnnSIM-module.h"
#include "nnnSIM/model/nnst/nnn-nnst.h"
#include "nnnSIM/model/nnst/nnn-nnst-entry.h"
#include "nnnSIM/model/nnst/nnn-nnst-entry-facemetric.h"

using namespace ns3;
using namespace std;
using namespace nnn;
using namespace nnst;

int main (int argc, char *argv[])
{
  // Create 48 bit MAC addresses
  Mac48Address n1_mac00 = Mac48Address ("01:02:A3:04:05:06");
  Mac48Address n1_mac01 = Mac48Address ("01:02:03:04:05:06");


  Mac48Address n2_mac00 = Mac48Address ("01:B2:03:04:05:06");
  Mac48Address n2_mac01 = Mac48Address ("01:02:03:C4:05:06");
  Mac48Address n2_mac02 = Mac48Address ("01:D2:03:04:05:06");

  Mac48Address n3_mac00 = Mac48Address ("01:02:03:04:05:B6");
  Mac48Address n3_mac01 = Mac48Address ("A1:02:03:04:05:06");
  Mac48Address n3_mac02 = Mac48Address ("01:02:E3:04:05:06");
  Mac48Address n3_mac03 = Mac48Address ("01:02:03:04:A5:06");
  Mac48Address n3_mac04 = Mac48Address ("01:1E:03:04:05:06");

  // Expire times
  Time n1_expire = Seconds (20);
  Time n2_expire = Seconds (40);
  Time n3_expire = Seconds (60);

  // Routing cost
  int32_t cost = 5;

  // Create a Node, required for Face
  NodeContainer tmpNodes;
  tmpNodes.Create (1);

  Ptr<Face> ptrFace00 = Create<Face> (tmpNodes.Get(0));
  Ptr<Face> ptrFace01 = Create<Face> (tmpNodes.Get(0));
  Ptr<Face> ptrFace02 = Create<Face> (tmpNodes.Get(0));

  // Create a FaceMetric
  FaceMetric n1_test00 (ptrFace00, n1_mac00.operator ns3::Address(), n1_expire, cost);
  FaceMetric n1_test01 (ptrFace01, n1_mac01.operator ns3::Address(), n1_expire, cost);

  std::cout << "Testing print of FaceMetric!" << std::endl;

  std::cout << n1_test00 << std::endl;
  std::cout << n1_test01 << std::endl;

  // Create our NNN Addresses
  Ptr<NNNAddress> n1_test = Create<NNNAddress> ("be.54.32");
  Ptr<NNNAddress> n2_test = Create<NNNAddress> ("af.67.31");
  Ptr<NNNAddress> n3_test = Create<NNNAddress> ("ae.34.26");

  Ptr<NNST> ptrn1_nnst = Create<NNST> ();

  nnst::Entry n1_nnst_entry (ptrn1_nnst, n1_test);

  n1_nnst_entry.AddPoA(ptrFace01, n2_mac00.operator ns3::Address(), n2_expire, cost);
  n1_nnst_entry.AddPoA(ptrFace00, n1_mac00.operator ns3::Address(), n1_expire, cost);
  n1_nnst_entry.AddPoA(ptrFace00, n1_mac01.operator ns3::Address(), n1_expire, cost);


  std::cout << "Testing print of nnst::Entry" << std::endl;

  std::cout << n1_nnst_entry << std::endl;

  std::cout << "Testing print of nnst::Entry by lease" << std::endl;

  n1_nnst_entry.printByLease();

  std::cout << "Testing print of nnst::Entry by face" << std::endl;

  n1_nnst_entry.printByFace();

  std::cout << "Testing print of nnst::Entry by address" << std::endl;

  n1_nnst_entry.printByAddress();

  Simulator::Stop (Seconds (70));
  Simulator::Run ();
  Simulator::Destroy ();

}
