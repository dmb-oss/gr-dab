/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * config.h is generated by configure.  It contains the results
 * of probing for features, options etc.  It should be the first
 * file included in your .cc file.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dab_magnitude_equalizer_vcc.h>
#include <gr_io_signature.h>

/*
 * Create a new instance of dab_magnitude_equalizer_vcc and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
dab_magnitude_equalizer_vcc_sptr 
dab_make_magnitude_equalizer_vcc (unsigned int vlen, float alpha, unsigned int decimate, float magnitude)
{
  return dab_magnitude_equalizer_vcc_sptr (new dab_magnitude_equalizer_vcc (vlen, alpha, decimate, magnitude));
}

dab_magnitude_equalizer_vcc::dab_magnitude_equalizer_vcc (unsigned int vlen, float alpha, unsigned int decimate, float magnitude) : 
  gr_sync_block ("magnitude_equalizer_vcc",
             gr_make_io_signature (1, 1, sizeof(gr_complex)*vlen),
             gr_make_io_signature (1, 1, sizeof(gr_complex)*vlen)),
  d_vlen(vlen), d_alpha(alpha), d_decimate(decimate), d_magnitude(magnitude), d_equalisation_factors(), d_count(0)
{
  d_equalisation_factors = new float[vlen];
  for (unsigned int i=0;i<vlen;i++)
    d_equalisation_factors[i] = 1;
}

dab_magnitude_equalizer_vcc::~dab_magnitude_equalizer_vcc (void)
{
  delete [] d_equalisation_factors;
}

int 
dab_magnitude_equalizer_vcc::work (int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
{
  gr_complex const *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];


  for (int i=0;i<noutput_items;i++) {

    d_count++;

    /* update equalisation factors ? */
    if (d_count==d_decimate) {
      d_decimate = 0;
      for (unsigned int j=0;i<;j++) {
        <+do_something+>
      }
    }

    /* equalize it ... */
    for (unsigned int j=0;j<d_vlen;j++) {
      ival=in[0];
      while (ival>M_PI_HALF) // poor man's modulo pi/2
        ival-=M_PI_HALF;
      while (ival<0)
        ival+=M_PI_HALF;
      d_offset_estimation[j] = (1-d_alpha)*d_offset_estimation[j] + d_alpha*(ival-M_PI_QUARTER);
      out[0] = in[0] - d_offset_estimation[j];
      out++;
      in++;
    }
  }

  return noutput_items;
}