/* -*- c++ -*- */
/*
 * Copyright 2023 Sungbo Eo
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DAB_TS_PACKET_ALIGN_H
#define INCLUDED_DAB_TS_PACKET_ALIGN_H

#include <grdab/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dab {

    /*!
     * \brief TS Packet Boundary Align
     * \ingroup dab
     *
     * @param npacks Number of packets (204 bytes)
     */
    class DAB_API ts_packet_align : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<ts_packet_align> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of dab::ts_packet_align.
       *
       * To avoid accidental use of raw pointers, dab::ts_packet_align's
       * constructor is in a private implementation
       * class. dab::ts_packet_align::make is the public interface for
       * creating new instances.
       */
      static sptr make(int npacks);
    };
  
  } // namespace dab
} // namespace gr

#endif /* INCLUDED_DAB_TS_PACKET_ALIGN_H */
