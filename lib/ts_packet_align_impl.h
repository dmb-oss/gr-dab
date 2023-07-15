/* -*- c++ -*- */
/*
 * Copyright 2023 Sungbo Eo
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DAB_TS_PACKET_ALIGN_IMPL_H
#define INCLUDED_DAB_TS_PACKET_ALIGN_IMPL_H

#include <grdab/ts_packet_align.h>

namespace gr {
  namespace dab {

    class ts_packet_align_impl : public ts_packet_align {
    private:
      static const int d_psize; // Packet size
      int d_npacks;             // Number of packets
      static const int d_SYNC;  // SYNC value

    public:
      ts_packet_align_impl(int npacks);
      ~ts_packet_align_impl();

      // Where all the action really happens
      void forecast(int noutput_items, gr_vector_int& ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int& ninput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items);
    };

  } // namespace dab
} // namespace gr

#endif /* INCLUDED_DAB_TS_PACKET_ALIGN_IMPL_H */
