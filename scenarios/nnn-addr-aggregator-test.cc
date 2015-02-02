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

  Ptr<NNNAddrAggregator> aggregation = Create<NNNAddrAggregator> ();

  std::cout << "Testing aggregator " << std::endl;
  std::cout << "Aggregator reports total addresses: " << aggregation->GetNumTotalDestinations() << std::endl;
  std::cout << "Aggregator report total addresses destinations: " << aggregation->GetNumDistinctDestinations()<< std::endl;
  std::cout << *aggregation << std::endl;

  std::cout << "Adding names to aggregator " << std::endl;
  std::cout << "Adding " << *addr01 << std::endl;
  std::cout << "Adding " << *addr02 << std::endl;
  std::cout << "Adding " << *addr03 << std::endl;
  std::cout << "Adding " << *addr04 << std::endl;

  aggregation->AddDestination(addr01);
  aggregation->AddDestination(addr02);
  aggregation->AddDestination(addr03);
  aggregation->AddDestination(addr04);

  std::cout << "Printing aggregator" << std::endl;
  std::cout << "Aggregator reports total addresses: " << aggregation->GetNumTotalDestinations() << std::endl;
  std::cout << "Aggregator report total addresses destinations: " << aggregation->GetNumDistinctDestinations()<< std::endl;
  std::cout << *aggregation << std::endl;

  std::cout << "Removing names" << std::endl;
  std::cout << "Removing " << *addr03 << std::endl;

  aggregation->RemoveDestination(addr03);

  std::cout << "Printing resulting aggregator" << std::endl;
  std::cout << "Aggregator reports total addresses: " << aggregation->GetNumTotalDestinations() << std::endl;
  std::cout << "Aggregator report total addresses destinations: " << aggregation->GetNumDistinctDestinations()<< std::endl;
  std::cout << *aggregation << std::endl;
}
