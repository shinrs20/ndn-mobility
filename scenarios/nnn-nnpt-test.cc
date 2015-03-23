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
using namespace nnpt;

int main (int argc, char *argv[])
{
  Ptr<NNPT> test1 = Create<NNPT> ();

  Ptr<const NNNAddress> nn_test1 = Create<const NNNAddress> ("be.54.32");
  Ptr<const NNNAddress> nn_test2 = Create<const NNNAddress> ("af.67.31");
  Ptr<const NNNAddress> nn_test3 = Create<const NNNAddress> ("ae.34.26");
  Ptr<const NNNAddress> nn_test4 = Create<const NNNAddress> ("4.23.5.6");
  Ptr<const NNNAddress> nn_test5 = Create<const NNNAddress> ("90.243.2");

  Ptr<const NNNAddress> nn_test6 = Create<const NNNAddress> ("af.67.31");

  Time t_test1 = Seconds (20);
  Time t_test2 = Seconds (60);
  Time t_test3 = Seconds (10);
  Time t_test4 = Seconds (120);

  Time updateTime = Seconds (80);

  // Designate when the simulator should stop
  Simulator::Stop (Seconds (70));

  // Start the simulator
  Simulator::Run ();

  test1->addEntry(nn_test1, nn_test2, Simulator::Now () + t_test1);
  test1->addEntry(nn_test2, nn_test3, Simulator::Now () + t_test2);
  test1->addEntry(nn_test3, nn_test1, Simulator::Now () + t_test3);
  test1->addEntry(nn_test1, nn_test4, Simulator::Now () + t_test2);

  std::cout << *test1 << std::endl;

  std::cout << "We have a NNPT of size: " << test1->size() << std::endl;

  std::cout << "Printing ordering by address" << std::endl;
  test1->printByAddress();

  std::cout << "\"" << *nn_test1 <<"\"'s New address is \"" << test1->findPairedName (nn_test1) << "\"" << std::endl;
  std::cout << "\"" << *nn_test3 << "\"'s Old address is \"" << test1->findPairedOldName(nn_test3) << "\"" << std::endl;

  std::cout << "Printing ordering by lease expire time" << std::endl;
  test1->printByLease();

  std::cout << "Expire time for " << *nn_test2 << " is " << test1->findNameExpireTime(nn_test2) << std::endl;
  std::cout << "Updating expire time for " << *nn_test2 << " by " << updateTime << " at " << Simulator::Now () << std::endl;

  test1->updateLeaseTime(nn_test2, Simulator::Now () + updateTime);

  std::cout << "Deleting " << *nn_test3 << " from container..." << std::endl;

  test1->deleteEntry(nn_test3);

  std::cout << "We have a NNPT of size: " << test1->size() << std::endl;
  std::cout << "Printing ordering by address" << std::endl;
  test1->printByAddress();

  std::cout << "Inserting " << *nn_test4 << " with lease expiry time " << t_test4 << std::endl;
  test1->addEntry(nn_test4, nn_test5, t_test4);

  Simulator::Destroy ();

  std::cout << "Printing ordering by address" << std::endl;
  test1->printByAddress();

  if (test1->foundNewName(nn_test2))
    {
      std::cout << "Found " << *nn_test2 << " within our structure!" << std::endl;
      std::cout << "\"" << *nn_test2 << "\" old name is \"" << test1->findPairedOldName(nn_test2) << "\"" << std::endl;
    }

  // Using different pointer
  std::cout << "Testing a different pointer!" << std::endl;
  if (test1->foundNewName(nn_test6))
    {
      std::cout << "Found " << *nn_test6 << " within our structure!" << std::endl;
      std::cout << "\"" << *nn_test6 << "\" old name is \"" << test1->findPairedOldName(nn_test6) << "\"" << std::endl;
    }

  std::cout << "\"" << *nn_test1 <<"\"'s New address is \"" << test1->findPairedName(nn_test1) << "\"" << std::endl;

}
