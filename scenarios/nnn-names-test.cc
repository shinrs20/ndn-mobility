/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-names-test.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-names-test.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-names-test.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Standard C++ modules
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iterator>
#include <iostream>
#include <map>
#include <string>
#include <sys/time.h>
#include <vector>

// Random modules
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/variate_generator.hpp>

// ns3 modules
#include <ns3-dev/ns3/applications-module.h>
#include <ns3-dev/ns3/bridge-helper.h>
#include <ns3-dev/ns3/buffer.h>
#include <ns3-dev/ns3/csma-module.h>
#include <ns3-dev/ns3/core-module.h>
#include <ns3-dev/ns3/mobility-module.h>
#include <ns3-dev/ns3/network-module.h>
#include <ns3-dev/ns3/point-to-point-module.h>
#include <ns3-dev/ns3/wifi-module.h>

// Extensions
#include "nnnSIM/nnnSIM-module.h"

using namespace ns3;
using namespace std;
using namespace boost;
using namespace nnn;

namespace br = boost::random;

char scenario[250] = "nnn-names-test";

NS_LOG_COMPONENT_DEFINE (scenario);

// Number generator
br::mt19937_64 gen;

// Obtains a random number from a uniform distribution between min and max.
// Must seed number generator to ensure randomness at runtime.
int obtain_Num(int min, int max)
{
	br::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}

// Obtain a random double from a uniform distribution between min and max.
// Must seed number generator to ensure randomness at runtime.
double obtain_Num(double min, double max)
{
	br::uniform_real_distribution<> dist(min, max);
	return dist(gen);
}

int main (int argc, char *argv[])
{

	NNNAddress errorProne;
	NNNAddress working;

	try
	{
		NS_LOG_INFO("Too long address");
		errorProne = NNNAddress ("0.1.2.3.4.5.6.7.8.9.A.B.C.D.E.F.F");

	}
	catch (nnn::error::NNNAddress &e)
	{
		cout << "Error!" << endl;
	}

	try {
		NS_LOG_INFO("Not hex address");
		errorProne = NNNAddress ("A.4.5.U");

	}
	catch (nnn::error::NNNAddress &e)
	{
		cout << "Error!" << endl;
	}

	try {
		NS_LOG_INFO("Normal address");
		working = NNNAddress ("AE.34.25");
	}
	catch (nnn::error::NNNAddress &e)
	{
		cout << "Error!" << endl;
	}

	try {
		NS_LOG_INFO("Double dot address");
		errorProne = NNNAddress ("AE.34..25");
	}
	catch (nnn::error::NNNAddress &e)
	{
		cout << "Error!" << endl;
	}

	try {
		NS_LOG_INFO("Dot empty end address");
		errorProne = NNNAddress ("AE.34.25.");
	}
	catch (nnn::error::NNNAddress &e)
	{
		cout << "Error!" << endl;
	}

	NNNAddress dest ("A.34");
	NNNAddress maxd ("0.1.2.3.4.5.6.7.8.9.A.B.C.D.E.F");
	NNNAddress tests ("111111111111111.A");
	NNNAddress samesecM ("ae.34.26");
	NNNAddress samesecL ("ae.34.24");

	NNNAddress lowerStart ("ad");
	NNNAddress higherStart ("af");

	NNNAddress higherSub ("ae.35");

	NNNAddress maxTest = maxd + lowerStart;
	NNNAddress maxTest2 = lowerStart + maxd;

	cout << "Max append test: " << maxd << " + " << lowerStart << endl;
	cout << "Max append test result: " << maxTest << endl;

	cout << "Max append test: " << lowerStart << " + " << maxd << endl;
	cout << "Max append test result: " << maxTest2 << endl;

	cout << "Working address is: " << working << endl;

	NNNAddress workingSector = working.getSectorName();
	NNNAddress lastLabel = working.getLastLabel();
	NNNAddress samesecSector = samesecM.getSectorName();

	cout << "Sector is: " << workingSector << endl;
	cout << "Last label is: " << lastLabel << endl;

	NNNAddress appendTest = lowerStart + lastLabel;

	cout << "Append test for: " << lowerStart << " with " << lastLabel << endl;

	cout << "Result is: " << appendTest << endl;
	cout << "Address used is: " << lowerStart << endl;
	cout << "Label used is: " << lastLabel << endl;

	cout << "Second working address is: " << samesecM << endl;

	cout << "Sector is: " << samesecSector << endl;

	NNNAddress closestSector = working.getClosestSector(workingSector);
	NNNAddress closestSector2 = workingSector.getClosestSector(working);
	NNNAddress closestSector5 = working.getClosestSector(samesecM);

	cout << "Closest working sector between " << working << " and " << workingSector << " is " << closestSector << endl;
	cout << "Closest working sector between " << workingSector << " and " << working << " is " << closestSector2 << endl;

	cout << "--------------------" << endl;
	NNNAddress closestSector3 = working.getClosestSector(dest);

	cout << "Closest working sector between " << working << " and " << dest << " is " << closestSector3 << endl;

	cout << "--------------------" << endl;
	NNNAddress closestSector4 = dest.getClosestSector(working);

	cout << "Closest working sector between " << dest << " and " << working << " is " << closestSector4 << endl;

	cout << "--------------------" << endl;
	cout << "Closest working sector between " << working << " and " << samesecM << " is " << closestSector5 << endl;
	cout << "--------------------" << endl;

	cout << "Is " << working << " same as " << working << "?: " << working.compare(working) << endl;
	cout << "Is " << working << " same as " << samesecL << "?: " << working.compare(samesecL) << endl;
	cout << "Is " << working << " same as " << samesecM << "?: " << working.compare(samesecM) << endl;
	cout << "--------------------" << endl;

	cout << "Is " << working << " same as " << higherStart << "?: " << working.compare(higherStart) << endl;
	cout << "Is " << working << " same as " << lowerStart << "?: " << working.compare(lowerStart) << endl;

	cout << "--------------------" << endl;
	cout << "Is " << working << " same as " << workingSector << "?: " << working.compare(workingSector) << endl;
	cout << "Is " << working << " same as " << higherSub << "?: " << working.compare(higherSub) << endl;

	cout << "--------------------" << endl;

	NNNAddress start ("a.1.23");
	NNNAddress test0 ("a.1.24");
	NNNAddress test1 ("a.1");
	NNNAddress test2 ("a.3");
	NNNAddress test3 ("a");
	NNNAddress test4 ("b");
	NNNAddress test5 ("b.1");
	NNNAddress test6 ("b.1.1");
	NNNAddress test7 ("1");
	NNNAddress test8 ("a.2");
	NNNAddress test9 ("a.1.23.4");
	NNNAddress test10 ("a.2.3.5");

	NNNAddress nnstTest1 ("a.1");
	NNNAddress nnstTest2 ("a.1.0.0");

	NNNAddress testee ("a.0.0.0");

	cout << "Closest working sector between " << start << " and " << test0 << " is " <<  start.getClosestSector(test0) << endl;
	cout << "Closest working sector between " << start << " and " << test1 << " is " <<  start.getClosestSector(test1) << endl;
	cout << "Closest working sector between " << start << " and " << test2 << " is " <<  start.getClosestSector(test2) << endl;
	cout << "Closest working sector between " << start << " and " << test3 << " is " <<  start.getClosestSector(test3) << endl;
	cout << "Closest working sector between " << start << " and " << test4 << " is " <<  start.getClosestSector(test4) << endl;
	cout << "Closest working sector between " << start << " and " << test5 << " is " <<  start.getClosestSector(test5) << endl;
	cout << "Closest working sector between " << start << " and " << test6 << " is " <<  start.getClosestSector(test6) << endl;

	cout << "Distance between " << start << " and " << start << " is " << start.distance(start) << endl;
	cout << "Distance between " << start << " and " << test0 << " is " << start.distance(test0) << endl;
	cout << "Distance between " << start << " and " << test1 << " is " << start.distance(test1) << endl;
	cout << "Distance between " << start << " and " << test2 << " is " << start.distance(test2) << endl;
	cout << "Distance between " << start << " and " << test3 << " is " << start.distance(test3) << endl;
	cout << "Distance between " << start << " and " << test4 << " is " << start.distance(test4) << endl;
	cout << "Distance between " << start << " and " << test5 << " is " << start.distance(test5) << endl;
	cout << "Distance between " << start << " and " << test6 << " is " << start.distance(test6) << endl;
	cout << "Distance between " << test3 << " and " << test4 << " is " << test3.distance(test4) << endl;
	cout << "Distance between " << test4 << " and " << test3 << " is " << test4.distance(test3) << endl;
	cout << "Distance between " << test1 << " and " << test2 << " is " << test1.distance(test2) << endl;
	cout << "Distance between " << test2 << " and " << test8 << " is " << test2.distance(test8) << endl;
	cout << "Distance between " << test1 << " and " << test2 << " is " << test1.distance(test2) << endl;
	cout << "Distance between " << test1 << " and " << test3 << " is " << test1.distance(test3) << endl;
	cout << "Distance between " << test1 << " and " << test0 << " is " << test1.distance(test0) << endl;

	cout << "--------------------" << endl;

	cout << "Distance between " << nnstTest1 << " and " << testee << " is " << nnstTest1.distance(testee) << endl;
	cout << "Distance between " << nnstTest2 << " and " << testee << " is " << nnstTest2.distance(testee) << endl;

	cout << "--------------------" << endl;

	cout << "Distance ordering results" << endl;
	cout << test1 << " > " << start << ": " << (test1 > start) << endl;
	cout << test1 << " < " << start << ": " << (test1 < start) << endl;
	cout << test1 << " > " << test3 << ": " << (test1 > test3) << endl;
	cout << test1 << " < " << test3 << ": " << (test1 < test3) << endl;
	cout << test1 << " == " << start << ": " << (start == test1) << endl;
	cout << test1 << " == " << test3 << ": " << (test3 == test1) << endl;
	cout << test1 << " == " << test1 << ": " << (test1 == test1) << endl;

	cout << "#####################" << endl;

	cout << test8 << " > " << start << ": " << (test8 > start) << endl;
	cout << test8 << " < " << start << ": " << (test8 < start) << endl;
	cout << test8 << " > " << test1 << ": " << (test8 > test1) << endl;
	cout << test8 << " < " << test1 << ": " << (test8 < test1) << endl;
	cout << test8 << " == " << start << ": " << (start == test8) << endl;
	cout << test8 << " == " << test3 << ": " << (test3 == test8) << endl;
	cout << test8 << " == " << test8 << ": " << (test8 == test8) << endl;
	cout << "--------------------" << endl;

	cout << "Testing subsectors" << endl;

	cout << "Is " << test9 << " sub-sector of " << test9 << ": " << (test9.isSubSector(test9)) << endl;
	cout << "Is " << test9 << " sub-sector of " << start << ": " << (test9.isSubSector(start)) << endl;
	cout << "Is " << test9 << " sub-sector of " << test1 << ": " << (test9.isSubSector(test1)) << endl;
	cout << "Is " << test9 << " sub-sector of " << test3 << ": " << (test9.isSubSector(test3)) << endl;
	cout << "Is " << test9 << " sub-sector of " << test10 << ": " << (test9.isSubSector(test10)) << endl;
	cout << "Is " << test9 << " sub-sector of " << test4 << ": " << (test9.isSubSector(test4)) << endl;
	cout << "Is " << test9 << " sub-sector of " << test5 << ": " << (test9.isSubSector(test5)) << endl;
	cout << "Is " << test9 << " sub-sector of " << test0 << ": " << (test9.isSubSector(test0)) << endl;
	cout << "Is " << test9 << " sub-sector of " << test8 << ": " << (test9.isSubSector(test8)) << endl;

	cout << "--------------------" << endl;

	cout << "Testing parent sectors" << endl;

	cout << "Is " << test9 << " parent sector of " << test9 << ": " << (test9.isParentSector(test9)) << endl;
	cout << "Is " << start << " parent sector of " << test9 << ": " << (start.isParentSector(test9)) << endl;
	cout << "Is " << test1 << " parent sector of " << test9 << ": " << (test1.isParentSector(test9)) << endl;
	cout << "Is " << test3 << " parent sector of " << test9 << ": " << (test3.isParentSector(test9)) << endl;
	cout << "Is " << test10 << " parent sector of " << test9 << ": " << (test10.isParentSector(test9)) << endl;
	cout << "Is " << test4 << " parent sector of " << test9 << ": " << (test4.isParentSector(test9)) << endl;
	cout << "Is " << test5 << " parent sector of " << test9 << ": " << (test5.isParentSector(test9)) << endl;

	cout << "--------------------" << endl;

	Buffer buf;

	size_t name_size = wire::NnnSim::SerializedSizeName(working);
	size_t name_sized = wire::NnnSim::SerializedSizeName(dest);
	size_t name_sizeM = wire::NnnSim::SerializedSizeName(maxd);
	size_t name_size15 = wire::NnnSim::SerializedSizeName(tests);

	cout << "Size obtained for " << working << " wire is " << name_size << endl;
	cout << "Size obtained for " << dest << " by wire is " << name_sized << endl;
	cout << "Size obtained for " << maxd << " by wire is " << name_sizeM << endl;
	cout << "Size obtained for " << tests << " by wire is " << name_size15 << endl;

	buf.AddAtStart(name_size);

	Buffer::Iterator i = buf.Begin();

	wire::NnnSim::SerializeName(i, working);

	cout << "Testing ordering of 3N names " << endl;

	cout << "By operators" << endl;
	cout << start << " and " << start << ": " << (start > start) << endl;
	cout << start << " and " << test0 << ": " << (start > test0) << endl;
	cout << start << " and " << test4 << ": " << (start > test4) << endl;
	cout << start << " and " << test7 << ": " << (start > test7) << endl;
	cout << start << " and " << test1 << ": " << (start > test1) << endl;
	cout << start << " and " << test2 << ": " << (start > test2) << endl;
	cout << start << " and " << test8 << ": " << (start > test8) << endl;

	cout << "By compareLabels" << endl;
	cout << start << " and " << start << ": " << (start.compareLabels(start)) << endl;
	cout << start << " and " << test0 << ": " << (start.compareLabels(test0)) << endl;
	cout << start << " and " << test4 << ": " << (start.compareLabels(test4)) << endl;
	cout << start << " and " << test7 << ": " << (start.compareLabels(test7)) << endl;
	cout << start << " and " << test1 << ": " << (start.compareLabels(test1)) << endl;
	cout << start << " and " << test2 << ": " << (start.compareLabels(test2)) << endl;
	cout << start << " and " << test8 << ": " << (start.compareLabels(test8)) << endl;

	Simulator::Stop (Seconds (1.0));
	Simulator::Run ();
	Simulator::Destroy ();

}
