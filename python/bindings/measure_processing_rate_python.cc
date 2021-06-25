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
/* BINDTOOL_HEADER_FILE(measure_processing_rate.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(329f3c159d50fa139f239e3707a7cde0)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <grdab/measure_processing_rate.h>
// pydoc.h is automatically generated in the build directory
#include <measure_processing_rate_pydoc.h>

void bind_measure_processing_rate(py::module& m)
{

    using measure_processing_rate    = gr::dab::measure_processing_rate;


    py::class_<measure_processing_rate,
        gr::sync_block,
        gr::block,
        gr::basic_block,
        std::shared_ptr<measure_processing_rate>>(m, "measure_processing_rate", D(measure_processing_rate))

        .def(py::init(&measure_processing_rate::make),
           py::arg("itemsize"),
           py::arg("samples_to_count"),
           D(measure_processing_rate,make)
        )
        



        ;




}








