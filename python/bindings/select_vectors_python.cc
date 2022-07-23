/*
 * Copyright 2022 Free Software Foundation, Inc.
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
/* BINDTOOL_HEADER_FILE(select_vectors.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(c8d958532cee64f2b1f97fb3878dd183)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <grdab/select_vectors.h>
// pydoc.h is automatically generated in the build directory
#include <select_vectors_pydoc.h>

void bind_select_vectors(py::module& m)
{

    using select_vectors    = ::gr::dab::select_vectors;


    py::class_<select_vectors, gr::block, gr::basic_block,
        std::shared_ptr<select_vectors>>(m, "select_vectors", D(select_vectors))

        .def(py::init(&select_vectors::make),
           py::arg("itemsize"),
           py::arg("length"),
           py::arg("num_select"),
           py::arg("num_skip"),
           D(select_vectors,make)
        )
        



        ;




}








