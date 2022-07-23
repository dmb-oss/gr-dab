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
/* BINDTOOL_HEADER_FILE(peak_detector_fb.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(8eee8c84d9de7d3f5405c0df38dd6a72)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <grdab/peak_detector_fb.h>
// pydoc.h is automatically generated in the build directory
#include <peak_detector_fb_pydoc.h>

void bind_peak_detector_fb(py::module& m)
{

    using peak_detector_fb    = gr::dab::peak_detector_fb;


    py::class_<peak_detector_fb,
        gr::sync_block,
        gr::block,
        gr::basic_block,
        std::shared_ptr<peak_detector_fb>>(m, "peak_detector_fb", D(peak_detector_fb))

        .def(py::init(&peak_detector_fb::make),
           py::arg("threshold_factor_rise") =  0.25,
           py::arg("threshold_factor_fall") =  0.40,
           py::arg("look_ahead") =  10,
           py::arg("alpha") =  0.001,
           D(peak_detector_fb,make)
        )
        



        ;




}







