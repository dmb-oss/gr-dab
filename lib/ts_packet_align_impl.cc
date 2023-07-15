/* -*- c++ -*- */
/*
 * Copyright 2023 Sungbo Eo
 * The code is copied and adapted from dvbt_energy_dispersal of gr-dtv
 *   Copyright 2015 Free Software Foundation, Inc.
 *
 * Here the output is not vectorized and the descrambling is disabled
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "ts_packet_align_impl.h"

namespace gr {
  namespace dab {

    const int ts_packet_align_impl::d_psize = 0xcc;
    const int ts_packet_align_impl::d_SYNC = 0x47;

    ts_packet_align::sptr
    ts_packet_align::make(int npacks)
    {
      return gnuradio::make_block_sptr<ts_packet_align_impl>(npacks);
    }

    /*
     * The private constructor
     */
    ts_packet_align_impl::ts_packet_align_impl(int npacks)
        : gr::block("ts_packet_align",
                    gr::io_signature::make(1, 1, sizeof(uint8_t)),
                    gr::io_signature::make(1, 1, sizeof(uint8_t))),
          d_npacks(npacks)
    {
      assert(d_npacks >= 0);
      set_output_multiple(d_psize * d_npacks);
    }

    /*
     * Our virtual destructor.
     */
    ts_packet_align_impl::~ts_packet_align_impl() {}

    void ts_packet_align_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
    {
      // Add one packet size for SYNC search
      ninput_items_required[0] = noutput_items + d_psize;
    }

    int ts_packet_align_impl::general_work(int noutput_items,
                                           gr_vector_int& ninput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
    {
      auto in = static_cast<const uint8_t*>(input_items[0]);
      auto out = static_cast<uint8_t*>(output_items[0]);

      int index = 0;
      int count = 0;
      int ret = 0;
      int is_sync = 0;

      // Search for SYNC byte
      while (is_sync == 0 && index < d_psize) {
        if (in[index] == d_SYNC) {
          is_sync = 1;
        } else {
          index++;
        }
      }

      // If we found a SYNC byte
      if (is_sync) {
        for (int i = 0; i < (noutput_items / d_psize); i++) {
          if (in[index + count] != d_SYNC) {
            this->d_logger->warn("Malformed MPEG-TS!");
          }
          memcpy(&out[count], &in[index + count], d_psize);
          count += d_psize;
        }
        consume_each(index + noutput_items);
        ret = noutput_items;
      } else {
        consume_each(index);
        ret = 0;
      }

      // Tell runtime system how many output items we produced.
      return ret;
    }

  } /* namespace dab */
} /* namespace gr */
