/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-pit-impl.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pit-impl.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pit-impl.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include  <ns3-dev/ns3/log.h>
#include  <ns3-dev/ns3/simulator.h>
#include  <ns3-dev/ns3/string.h>
#include  <ns3-dev/ns3/uinteger.h>

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

NS_LOG_COMPONENT_DEFINE ("nnn.pit.PitImpl");

#include <ns3-dev/ns3/ndnSIM/utils/trie/aggregate-stats-policy.h>
#include <ns3-dev/ns3/ndnSIM/utils/trie/empty-policy.h>
#include <ns3-dev/ns3/ndnSIM/utils/trie/lru-policy.h>
#include <ns3-dev/ns3/ndnSIM/utils/trie/multi-policy.h>
#include <ns3-dev/ns3/ndnSIM/utils/trie/persistent-policy.h>
#include <ns3-dev/ns3/ndnSIM/utils/trie/random-policy.h>
#include <ns3-dev/ns3/ndnSIM/model/pit/custom-policies/serialized-size-policy.h>

#include "nnn-pit-impl.h"

using namespace boost::tuples;
using namespace boost;
namespace ll = boost::lambda;

#define NS_OBJECT_ENSURE_REGISTERED_TEMPL(type, templ)  \
    static struct X ## type ## templ ## RegistrationClass \
    {                                                     \
  X ## type ## templ ## RegistrationClass () {        \
      ns3::TypeId tid = type<templ>::GetTypeId ();      \
      tid.GetParent ();                                 \
    }                                                   \
    } x_ ## type ## templ ## RegistrationVariable

namespace ns3 {
  namespace nnn {
    namespace pit {

      template<>
      uint32_t
      PitImpl<ndn::ndnSIM::serialized_size_policy_traits>::GetCurrentSize () const
      {
	return super::getPolicy ().get_current_space_used ();
      }

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

      // explicit instantiation and registering
      template class PitImpl<ndn::ndnSIM::persistent_policy_traits>;
      template class PitImpl<ndn::ndnSIM::random_policy_traits>;
      template class PitImpl<ndn::ndnSIM::lru_policy_traits>;
      template class PitImpl<ndn::ndnSIM::serialized_size_policy_traits>;

      typedef ndn::ndnSIM::persistent_policy_traits ppt;
      typedef ndn::ndnSIM::random_policy_traits rpt;
      typedef ndn::ndnSIM::lru_policy_traits lpt;
      typedef ndn::ndnSIM::serialized_size_policy_traits sspt;

      NS_OBJECT_ENSURE_REGISTERED_TEMPL(PitImpl, ppt);
      NS_OBJECT_ENSURE_REGISTERED_TEMPL(PitImpl, rpt);
      NS_OBJECT_ENSURE_REGISTERED_TEMPL(PitImpl, lpt);
      NS_OBJECT_ENSURE_REGISTERED_TEMPL(PitImpl, sspt);


      typedef ndn::ndnSIM::multi_policy_traits< boost::mpl::vector2< ndn::ndnSIM::persistent_policy_traits,
	  ndn::ndnSIM::aggregate_stats_policy_traits > > PersistentWithCountsTraits;
      typedef ndn::ndnSIM::multi_policy_traits< boost::mpl::vector2< ndn::ndnSIM::random_policy_traits,
	  ndn::ndnSIM::aggregate_stats_policy_traits > > RandomWithCountsTraits;
      typedef ndn::ndnSIM::multi_policy_traits< boost::mpl::vector2< ndn::ndnSIM::lru_policy_traits,
	  ndn::ndnSIM::aggregate_stats_policy_traits > > LruWithCountsTraits;
      typedef ndn::ndnSIM::multi_policy_traits< boost::mpl::vector2< ndn::ndnSIM::serialized_size_policy_traits,
	  ndn::ndnSIM::aggregate_stats_policy_traits > > SerializedSizeWithCountsTraits;

      template class PitImpl<PersistentWithCountsTraits>;
      NS_OBJECT_ENSURE_REGISTERED_TEMPL(PitImpl, PersistentWithCountsTraits);

      template class PitImpl<RandomWithCountsTraits>;
      NS_OBJECT_ENSURE_REGISTERED_TEMPL(PitImpl, RandomWithCountsTraits);

      template class PitImpl<LruWithCountsTraits>;
      NS_OBJECT_ENSURE_REGISTERED_TEMPL(PitImpl, LruWithCountsTraits);

      template class PitImpl<SerializedSizeWithCountsTraits>;
      NS_OBJECT_ENSURE_REGISTERED_TEMPL(PitImpl, SerializedSizeWithCountsTraits);

#ifdef DOXYGEN
// /**
//  * \brief PIT in which new entries will be rejected if PIT size reached its limit
      //  */
      class Persistent : public PitImpl<persistent_policy_traits> { };

      /**
       * \brief PIT in which PIT reaches its limit, random entry (could be the newly created one) will be removed from PIT
       */
      class Random : public PitImpl<random_policy_traits> { };

      /**
       * \brief PIT in which  the least recently used entry (the oldest entry with minimum number of incoming faces)
       * will be removed when PIT size reached its limit
       */
      class Lru : public PitImpl<lru_policy_traits> { };

      /**
       * @brief A variant of persistent PIT implementation where size of PIT is based on size of interests in bytes (MaxSize parameter)
       */
      class SerializedSize : public PitImpl<serialized_size_policy_traits> { };

#endif

    } // namespace pit
  } // namespace nnn
} // namespace ns3
