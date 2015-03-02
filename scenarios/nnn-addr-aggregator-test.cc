/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-addr-aggregator-test.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-addr-aggregator-test.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-addr-aggregator-test.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Extensions
#include "nnnSIM/nnnSIM-module.h"
#include "nnnSIM/helper/nnn-addr-aggregator.h"

using namespace ns3;
using namespace std;
using namespace nnn;

int main (int argc, char *argv[])
{
  Ptr<NNNAddress> addr01 = Create<NNNAddress> ("ae.34.21");
  Ptr<NNNAddress> addr02 = Create<NNNAddress> ("ae.34.2");
  Ptr<NNNAddress> addr03 = Create<NNNAddress> ("ae");
  Ptr<NNNAddress> addr04 = Create<NNNAddress> ("ae.4");
  Ptr<NNNAddress> addr05 = Create<NNNAddress> ("ae.0.7");
  Ptr<NNNAddress> addr06 = Create<NNNAddress> ("ae.4.15");
  Ptr<NNNAddress> addr07 = Create<NNNAddress> ("ae.4.16");
  Ptr<NNNAddress> addr08 = Create<NNNAddress> ("ae.4.17");
  Ptr<NNNAddress> addr09 = Create<NNNAddress> ("ae.4.18");
  Ptr<NNNAddress> addr10 = Create<NNNAddress> ("ae.4.19");

  Ptr<NNNAddrAggregator> aggregation = Create<NNNAddrAggregator> ();

  if (aggregation->isEmpty())
    std::cout << "Aggregation is currently empty!" << std::endl;

  std::cout << "--Testing aggregator " << std::endl;
  std::cout << "Aggregator reports total addresses: " << aggregation->GetNumTotalDestinations() << std::endl;
  std::cout << "Aggregator report total addresses destinations: " << aggregation->GetNumDistinctDestinations()<< std::endl;
  std::cout << *aggregation << std::endl;

  std::cout << "--Adding names to aggregator " << std::endl;
  std::cout << "Adding " << *addr01 << std::endl;
  aggregation->AddDestination(addr01);
  std::cout << "Adding " << *addr02 << std::endl;
  aggregation->AddDestination(addr02);
  std::cout << "Adding " << *addr03 << std::endl;
  aggregation->AddDestination(addr03);
  std::cout << "Adding " << *addr04 << std::endl;
  aggregation->AddDestination(addr04);
  std::cout << "Adding " << *addr05 << std::endl;
  aggregation->AddDestination(addr05);
  std::cout << "Adding " << *addr06 << std::endl;
  aggregation->AddDestination(addr06);
  std::cout << "Adding " << *addr07 << std::endl;
  aggregation->AddDestination(addr07);
  std::cout << "Adding " << *addr08 << std::endl;
  aggregation->AddDestination(addr08);
  std::cout << "Adding " << *addr09 << std::endl;
  aggregation->AddDestination(addr09);
  std::cout << "Adding " << *addr10 << std::endl;
  aggregation->AddDestination(addr10);

  std::cout << "--Printing aggregator" << std::endl;
  std::cout << "Aggregator reports total addresses: " << aggregation->GetNumTotalDestinations() << std::endl;
  std::cout << "Aggregator report total addresses destinations: " << aggregation->GetNumDistinctDestinations()<< std::endl;
  std::cout << *aggregation << std::endl;

  std::cout << "Removing names" << std::endl;
  std::cout << "Trying to remove " << *addr01 << std::endl;
  aggregation->RemoveDestination(addr01);
  std::cout << "Trying to remove " << *addr02 << std::endl;
  aggregation->RemoveDestination(addr02);
  std::cout << "Trying to remove " << *addr03 << std::endl;
  aggregation->RemoveDestination(addr03);
  std::cout << "Trying to remove " << *addr04 << std::endl;
  aggregation->RemoveDestination(addr04);
  std::cout << "Trying to remove " << *addr05 << std::endl;
  aggregation->RemoveDestination(addr05);

  if (!aggregation->isEmpty())
     std::cout << "Aggregation is currently not empty!" << std::endl;

  std::cout << "--Printing resulting aggregator" << std::endl;
  std::cout << "Aggregator reports total addresses: " << aggregation->GetNumTotalDestinations() << std::endl;
  std::cout << "Aggregator report total addresses destinations: " << aggregation->GetNumDistinctDestinations()<< std::endl;
  std::cout << *aggregation << std::endl;

  std::vector<Ptr<NNNAddress> > distinct = aggregation->GetDistinctDestinations();
  std::vector<Ptr<NNNAddress> >::iterator it;

  std::cout << "Printing aggregation distinct addresses" << std::endl;
  for (it = distinct.begin() ; it != distinct.end() ; ++it)
    {
      std::cout << **it << std::endl;
    }

  distinct = aggregation->GetTotalDestinations();
  std::cout << "Printing aggregation total addresses" << std::endl;
  for (it = distinct.begin() ; it != distinct.end() ; ++it)
    {
      std::cout << **it << std::endl;
    }
}
