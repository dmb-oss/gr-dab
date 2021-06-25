/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(ofdm_ffe_all_in_one.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(4f170a62ea562049be72784feed966bc)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <grdab/ofdm_ffe_all_in_one.h>
// pydoc.h is automatically generated in the build directory
#include <ofdm_ffe_all_in_one_pydoc.h>

void bind_ofdm_ffe_all_in_one(py::module& m)
{

    using ofdm_ffe_all_in_one    = gr::dab::ofdm_ffe_all_in_one;


    py::class_<ofdm_ffe_all_in_one,
        gr::sync_block,
        gr::block,
        gr::basic_block,
        std::shared_ptr<ofdm_ffe_all_in_one>>(m, "ofdm_ffe_all_in_one", D(ofdm_ffe_all_in_one))

        .def(py::init(&ofdm_ffe_all_in_one::make),
           py::arg("symbol_length"),
           py::arg("fft_length"),
           py::arg("num_symbols"),
           py::arg("alpha"),
           py::arg("sample_rate"),
           D(ofdm_ffe_all_in_one,make)
        )
        



        ;




}








