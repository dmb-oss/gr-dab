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
/* BINDTOOL_HEADER_FILE(unpuncture_vff.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(f4612558acc29ae52a1af913c265e434)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <grdab/unpuncture_vff.h>
// pydoc.h is automatically generated in the build directory
#include <unpuncture_vff_pydoc.h>

void bind_unpuncture_vff(py::module& m)
{

    using unpuncture_vff    = gr::dab::unpuncture_vff;


    py::class_<unpuncture_vff,
        gr::sync_block,
        gr::block,
        gr::basic_block,
        std::shared_ptr<unpuncture_vff>>(m, "unpuncture_vff", D(unpuncture_vff))

        .def(py::init(&unpuncture_vff::make),
           py::arg("puncturing_vector"),
           py::arg("fillval") = 0,
           D(unpuncture_vff,make)
        )
        



        ;




}







