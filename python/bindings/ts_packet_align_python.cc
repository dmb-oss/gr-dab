/*
 * Copyright 2023 Free Software Foundation, Inc.
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
/* BINDTOOL_HEADER_FILE(ts_packet_align.h)                                         */
/* BINDTOOL_HEADER_FILE_HASH(3bc311a9003c1545d2b1cec1178a31e7)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <grdab/ts_packet_align.h>
// pydoc.h is automatically generated in the build directory
#include <ts_packet_align_pydoc.h>

void bind_ts_packet_align(py::module& m)
{

    using ts_packet_align    = gr::dab::ts_packet_align;


    py::class_<ts_packet_align, gr::block, gr::basic_block,
        std::shared_ptr<ts_packet_align>>(m, "ts_packet_align", D(ts_packet_align))

        .def(py::init(&ts_packet_align::make),
           py::arg("npacks"),
           D(ts_packet_align,make)
        )
        



        ;




}







