/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-face-container.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-face-container.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-face-container.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_FACE_CONTAINER_H_
#define NNN_FACE_CONTAINER_H_

#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/simple-ref-count.h>

#include "../model/nnn-face.h"

namespace ns3 {
  namespace nnn {

    class FaceContainer: public SimpleRefCount<FaceContainer>
    {
    private:
      typedef std::vector< Ptr<Face> > Container;

    public:
      typedef Container::const_iterator Iterator;
      typedef Container::iterator iter;

      FaceContainer();

      /**
       * \brief Copy constructor for FaceContainer. Calls AddAll method
       *
       * \see FaceContainer::AddAll
       */
      FaceContainer (const FaceContainer &other);

      /**
       * \brief Copy operator for FaceContainer. Empties vector and calls AddAll method
       *
       * All previously obtained iterators (Begin() and End()) will be invalidated
       *
       * \see FaceContainer::AddAll
       */
      FaceContainer& operator= (const FaceContainer &other);

      /**
       * \brief Add all entries from other container
       *
       * \param other smart pointer to a container
       */
      void AddAll (Ptr<FaceContainer> other);

      /**
       * \brief Add all entries from other container
       *
       * \param other container
       */
      void AddAll (const FaceContainer &other);

      /**
       * \brief Get an iterator which refers to the first pair in the
       * container.
       *
       * \returns an iterator which refers to the first pair in the container.
       */
      Iterator Begin () const;

      /**
       * \brief Get an iterator which indicates past-the-last Node in the
       * container.
       *
       * \returns an iterator which indicates an ending condition for a loop.
       */
      Iterator End () const;

      /**
       * \brief Get the number of faces stored in this container
       *
       * \returns the number of faces stored in this container
       */
      uint32_t GetN () const;

      /**
       * Add an entry to the container
       *
       * \param face a smart pointer to a Face-derived object
       *
       * @see Face
       */
      void Add (const Ptr<Face> &face);

      /**
       * Remove an entry to the container
       *
       * \param face a smart pointer to a Face-derived object
       *
       * @see Face
       */
      void Remove (const Ptr<Face> &face);

      /**
       * Get a smart pointer to Face-derived object stored in the container
       *
       * \param i the iterator corresponding to the requested object
       *
       * This method is redundant and simple dereferencing of the iterator should be used instead
       *
       * @see Face
       */
      Ptr<Face> Get (Iterator i) const;

    private:
      Container m_faces;
    };

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_FACE_CONTAINER_H_ */
