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
#include "nnnSIM/model/fw/nnn-forwarding-strategy.h"
#include "nnnSIM/model/nnst/nnn-nnst.h"
#include "nnnSIM/model/nnst/nnn-nnst-entry.h"
#include "nnnSIM/model/nnst/nnn-nnst-entry-facemetric.h"

using namespace ns3;
using namespace std;
using namespace nnn;
using namespace nnst;

void
printNNST(Ptr<NNST> nnst)
{
  std::cout << "Attempt NNST printing " <<  Simulator::Now() << std::endl;

  std::cout << *nnst << std::endl;
}

void
printAddrs (std::vector<Ptr<const NNNAddress> > ret)
{
  std::vector<Ptr<const NNNAddress> >::iterator it;

  for (it = ret.begin () ; it != ret.end(); ++it)
    {
      std::cout << **it << endl;
    }
}

int main (int argc, char *argv[])
{
  // Create 48 bit MAC addresses
  Mac48Address n1_mac00 = Mac48Address ("01:B2:03:04:05:06");
  Mac48Address n1_mac01 = Mac48Address ("01:02:03:04:05:06");

  Mac48Address n2_mac00 = Mac48Address ("01:02:A3:04:05:06");
  Mac48Address n2_mac01 = Mac48Address ("01:02:03:C4:05:06");
  Mac48Address n2_mac02 = Mac48Address ("01:D2:03:04:05:06");

  Mac48Address n3_mac00 = Mac48Address ("01:02:03:04:05:B6");
  Mac48Address n3_mac01 = Mac48Address ("A1:02:03:04:05:06");
  Mac48Address n3_mac02 = Mac48Address ("01:02:E3:04:05:06");
  Mac48Address n3_mac03 = Mac48Address ("01:02:03:04:A5:06");
  Mac48Address n3_mac04 = Mac48Address ("01:1E:03:04:05:06");

  Mac48Address n4_mac01 = Mac48Address ("20:1E:03:04:05:06");

  Mac48Address n5_mac00 = Mac48Address ("20:1E:03:04:FF:06");
  Mac48Address n5_mac01 = Mac48Address ("20:1E:03:04:05:F6");

  // Expire times
  Time n1_expire = Seconds (5);
  Time n2_expire = Seconds (10);
  Time n3_expire = Seconds (15);

  std::vector<Address> n1_poas;

  n1_poas.push_back(n1_mac00.operator ns3::Address());
  n1_poas.push_back(n1_mac01.operator ns3::Address());

  std::vector<Address> n2_poas_1;

  n2_poas_1.push_back(n2_mac01.operator ns3::Address());
  n2_poas_1.push_back(n2_mac02.operator ns3::Address());

  std::vector<Address> n3_poas_1;

  n3_poas_1.push_back(n3_mac01.operator ns3::Address());
  n3_poas_1.push_back(n3_mac02.operator ns3::Address());
  n3_poas_1.push_back(n3_mac03.operator ns3::Address());

  std::vector<Address> n5_poas_1;

  n5_poas_1.push_back(n5_mac00.operator ns3::Address());
  n5_poas_1.push_back(n5_mac01.operator ns3::Address());

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
  Ptr<NNNAddress> n1_test = Create<NNNAddress> ("be");
  Ptr<NNNAddress> n2_test = Create<NNNAddress> ("be.34");
  Ptr<NNNAddress> n3_test = Create<NNNAddress> ("be.34.25");
  Ptr<NNNAddress> n4_test = Create<NNNAddress> ("be.34.26");

  Ptr<NNNAddress> prefixTest = Create<NNNAddress> ("be.34");

  Ptr<NNST> ptrn1_nnst = CreateObject<NNST> ();
  Ptr<ForwardingStrategy> fw = CreateObject<ForwardingStrategy> ();
  Ptr<Node> node = tmpNodes.Get(0);

  ptrn1_nnst->AggregateObject(fw);

  nnst::Entry n1_nnst_entry (ptrn1_nnst, n1_test);

  n1_nnst_entry.AddPoA(ptrFace00, n1_mac00.operator ns3::Address(), n1_expire, cost);
  n1_nnst_entry.AddPoA(ptrFace00, n1_mac01.operator ns3::Address(), n1_expire, 1);
  n1_nnst_entry.AddPoA(ptrFace01, n2_mac00.operator ns3::Address(), n2_expire, cost);

  std::cout << "Testing print of nnst::Entry" << std::endl;

  std::cout << n1_nnst_entry << std::endl;

  std::cout << "Testing print of nnst::Entry by lease" << std::endl;

  n1_nnst_entry.printByLease();

  std::cout << "Testing print of nnst::Entry by face" << std::endl;

  n1_nnst_entry.printByFace();

  std::cout << "Testing print of nnst::Entry by address" << std::endl;

  n1_nnst_entry.printByAddress();

  std::cout << "Testing print of nnst::Entry by metric" << std::endl;

  n1_nnst_entry.printByMetric();

  Time updateTime = Seconds(60);

  std::cout << "Testing updating lease time to " << updateTime << std::endl;

  n1_nnst_entry.UpdateLeaseTime(updateTime);

  n1_nnst_entry.printByLease();

  std::cout << "Retrieving all PoAs from Entry" << std::endl;

  std::vector<Address> tmp = n1_nnst_entry.GetPoAs();

  std::vector<Address>::iterator it = tmp.begin();

  while (it != tmp.end())
    {
      std::cout << *it << std::endl;
      ++it;
    }

  std::cout << "Retrieving PoAs from Entry using " << *ptrFace01 << std::endl;

  tmp = n1_nnst_entry.GetPoAs(ptrFace01);

  it = tmp.begin();

  while (it != tmp.end())
    {
      std::cout << *it << std::endl;
      ++it;
    }

  Ptr<Face> tmp1 = n1_nnst_entry.GetFace(n1_mac01.operator ns3::Address());

  std::cout << "Testing getting best FaceMetric!" << std::endl;

  FaceMetric fm_tmp = n1_nnst_entry.FindBestCandidate();

  std::cout << "Face: " << *(fm_tmp.GetFace()) << std::endl;
  std::cout << "Address: " << fm_tmp.GetAddress() << std::endl;
  std::cout << "Lease: " << fm_tmp.GetExpireTime() << std::endl;
  std::cout << "Cost: " << fm_tmp.GetRoutingCost() << std::endl;
  std::cout << std::endl;

  std::cout << "Testing deletion of PoA " << n2_mac00.operator ns3::Address() << " at " << Simulator::Now() << std::endl;

  n1_nnst_entry.RemovePoA(n2_mac00.operator ns3::Address());

  n1_nnst_entry.printByAddress();

  std::cout << "Testing deletion of Face " <<  *ptrFace00 << " at " << Simulator::Now() << std::endl;

  n1_nnst_entry.RemoveFace(ptrFace00);

  n1_nnst_entry.printByAddress();

  std::cout << "End deletion tests" << std::endl;

  std::cout << "Printing ordering by address at " << Simulator::Now() << std::endl;
  n1_nnst_entry.printByAddress();

  std::cout << std::setfill('*') << std::setw(80) << "" << std::endl;

  ptrn1_nnst->Add(n1_test, ptrFace00, n1_poas, n1_expire, cost);

  ptrn1_nnst->Add(n2_test, ptrFace00, n2_mac00.operator ns3::Address(), n2_expire, cost);
  ptrn1_nnst->Add(n2_test, ptrFace01, n1_poas, n2_expire, cost);
  ptrn1_nnst->Add(n3_test, ptrFace00, n3_mac00.operator ns3::Address(), n3_expire, cost);
  ptrn1_nnst->Add(n3_test, ptrFace01, n3_poas_1, n3_expire, cost);
  ptrn1_nnst->Add(n3_test, ptrFace02, n3_mac04.operator ns3::Address(), n3_expire, cost);
  ptrn1_nnst->Add(n4_test, ptrFace00, n4_mac01.operator ns3::Address(), n3_expire, cost);

  std::cout << "###########################################" << std::endl;

  std::cout << "Testing sector info" << std::endl;
  std::vector<Ptr<const NNNAddress> > addrs;

  std::cout << "Closest sector info with: " << *prefixTest << std::endl;

  std::cout << *ptrn1_nnst->ClosestSectorNameInfo(prefixTest) << std::endl;

  std::cout << "One hop sector info with: " << *prefixTest << std::endl;

  addrs = ptrn1_nnst->OneHopNameInfo(prefixTest);
  printAddrs(addrs);

  std::cout << "One hop sub-sector info with: " << *prefixTest << std::endl;
  addrs = ptrn1_nnst->OneHopSubSectorNameInfo(prefixTest);
  printAddrs(addrs);

  std::cout << "One hop parent sector info with: " << *prefixTest << std::endl;
  addrs = ptrn1_nnst->OneHopParentSectorNameInfo(prefixTest);
  printAddrs(addrs);

  std::cout << "###########################################" << std::endl;

  std::cout << "Begin testing of NNST at " <<  Simulator::Now() << std::endl;

  std::cout << std::setfill(' ') << *ptrn1_nnst << std::endl;

  std::cout << "Begin to wait at " <<  Simulator::Now() << std::endl;
  Simulator::Schedule(Seconds(8), &printNNST, ptrn1_nnst);


  std::cout << "Searching for " << *n3_test << std::endl;
  Ptr<nnst::Entry> ptr_ent = ptrn1_nnst->Find(*n3_test);

  std::cout << "Obtained for " << *n3_test << std::endl;
  std::cout << *ptr_ent << std::endl;

  std::cout << "Attempting to update " << *n3_test << std::endl;
  Simulator::Schedule(Seconds(15), &printNNST, ptrn1_nnst);
  ptrn1_nnst->UpdateLeaseTime(*n3_test, Seconds(20));

  Simulator::Schedule(Seconds(24), &printNNST, ptrn1_nnst);

  Simulator::Stop (Seconds (25));
  Simulator::Run ();
  Simulator::Destroy ();

  std::cout << "End everything " <<  Simulator::Now() << std::endl;
}
