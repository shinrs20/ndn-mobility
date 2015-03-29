/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-names-cointainer-test.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-names-cointainer-test.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-names-cointainer-test.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Extensions
#include "nnnSIM/nnnSIM-module.h"
#include "nnnSIM/helper/nnn-names-container.h"
#include "nnnSIM/helper/nnn-names-container-entry.h"

using namespace ns3;
using namespace std;
using namespace nnn;

int main (int argc, char *argv[])
{
  NamesContainer test1;

  Ptr<const NNNAddress> nn_test1 = Create<NNNAddress> ("be.54.32");
  Ptr<const NNNAddress> nn_test2 = Create<NNNAddress> ("af.67.31");
  Ptr<const NNNAddress> nn_test3 = Create<NNNAddress> ("ae.34.26");
  Ptr<const NNNAddress> nn_test4 = Create<NNNAddress> ("6.8.9.3.2");

  Time t_test1 = Seconds (100);
  Time t_test2 = Seconds (300);
  Time t_test3 = Seconds (350);
  Time t_test4 = Seconds (200);

  Time updateTime = Seconds (400);

  test1.addEntry(nn_test1, t_test1);
  test1.addEntry(nn_test2, t_test2);
  test1.addEntry(nn_test3, t_test3);
  test1.addEntry(nn_test4, t_test4);

  std::cout << "We have a NamesContainer of size: " << test1.size() << std::endl;

  std::cout << "Printing ordering by address" << std::endl;
  test1.printByAddress();

  std::cout << "Printing ordering by lease expire time" << std::endl;
  test1.printByLease();

  std::cout << "Expire time for " << *nn_test2 << " is " << test1.findNameExpireTime(nn_test2) << std::endl;
  std::cout << "Updating expire time for " << *nn_test2 << " to " << updateTime << std::endl;

  test1.updateLeaseTime(nn_test2, updateTime);

  std::cout << "Deleting " << *nn_test3 << " from container..." << std::endl;

  test1.deleteEntry(nn_test3);

  std::cout << "We have a NamesContainer of size: " << test1.size() << std::endl;
  std::cout << "Printing ordering by lease expire time" << std::endl;
  test1.printByLease();

  std::cout << "Deleting everything from container..." << std::endl;
  test1.clear();

  std::cout << "Printing ordering by lease expire time" << std::endl;
  test1.printByLease();

  std::cout << "Reinserting everything into NamesContainer" << std::endl;

  test1.addEntry(nn_test1, t_test1);
  test1.addEntry(nn_test2, t_test2);
  test1.addEntry(nn_test3, t_test3);
  test1.addEntry(nn_test4, t_test4);

  std::cout << "We have a NamesContainer of size: " << test1.size() << std::endl;

  Ptr<const NNNAddress> tmp = test1.findNewestName();

  std::cout << "Last address to die will be " << *tmp << " at " << test1.findNameExpireTime(tmp) << std::endl;
  std::cout << "Printing ordering by address" << std::endl;
  test1.printByAddress();

  std::cout << "Printing ordering by lease expire time" << std::endl;
  test1.printByLease();

  std::cout << "Printing ordering by << operator" << std::endl;
  std::cout << test1;


  Simulator::Stop (Seconds (50));
  Simulator::Run ();
  Simulator::Destroy ();
}
