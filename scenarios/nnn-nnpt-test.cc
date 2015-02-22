/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Takahiro Miyamoto <mt3.mos@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnpt-test.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnpt-test.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnpt-test.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Extensions
#include "nnnSIM/nnnSIM-module.h"
#include "nnnSIM/model/nnpt/nnn-nnpt.h"
#include "nnnSIM/model/nnpt/nnn-nnpt-entry.h"

using namespace ns3;
using namespace std;
using namespace nnn;

int main (int argc, char *argv[])
{
  NNPT test1;

  Ptr<NNNAddress> nn_test1 = Create<NNNAddress> ("be.54.32");
  Ptr<NNNAddress> nn_test2 = Create<NNNAddress> ("af.67.31");
  Ptr<NNNAddress> nn_test3 = Create<NNNAddress> ("ae.34.26");
  Ptr<NNNAddress> nn_test4 = Create<NNNAddress> ("4.23.5.6");

  Time t_test1 = Seconds (20);
  Time t_test2 = Seconds (60);
  Time t_test3 = Seconds (10);

  Time updateTime = Seconds (80);

  NNPTEntry nnpte_test1 = NNPTEntry (nn_test1, nn_test2, t_test1);
  NNPTEntry nnpte_test2 = NNPTEntry (nn_test2, nn_test3, t_test2);
  NNPTEntry nnpte_test3 = NNPTEntry (nn_test3, nn_test1, t_test3);
  NNPTEntry nnpte_test4 = NNPTEntry (nn_test1, nn_test4, t_test2);

  test1.addEntry(nnpte_test1);
  test1.addEntry(nnpte_test2);
  test1.addEntry(nnpte_test3);
  test1.addEntry(nnpte_test4);

  std::cout << "We have a NNPT of size: " << test1.size() << std::endl;

  std::cout << "Printing ordering by address" << std::endl;
  test1.printByAddress();

  std::cout << "\"" << *nn_test1 <<"\"'s New address is \"" << *test1.findPairedName(nn_test1) << "\"" << std::endl;

  std::cout << "Printing ordering by lease expire time" << std::endl;
  test1.printByLease();

  std::cout << "Expire time for " << *nn_test2 << " is " << test1.findNameExpireTime(nn_test2) << std::endl;
  std::cout << "Updating expire time for " << *nn_test2 << " to " << updateTime << std::endl;

  test1.updateLeaseTime(nn_test2, updateTime);

  std::cout << "Deleting " << *nn_test3 << " from container..." << std::endl;

  test1.deleteEntry(nn_test3);

  std::cout << "We have a NNPT of size: " << test1.size() << std::endl;
  std::cout << "Printing ordering by address" << std::endl;
  test1.printByAddress();

  Simulator::Stop (Seconds (70));
  Simulator::Run ();
  Simulator::Destroy ();

  std::cout << "Printing ordering by address" << std::endl;
  test1.printByAddress();

  std::cout << "\"" << *nn_test1 <<"\"'s New address is \"" << *(test1.findPairedName(nn_test1)) << "\"" << std::endl;

}


