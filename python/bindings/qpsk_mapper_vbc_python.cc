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
/* BINDTOOL_HEADER_FILE(qpsk_mapper_vbc.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(2195bfef8e5eb067161e9e30849b29e2)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <grdab/qpsk_mapper_vbc.h>
// pydoc.h is automatically generated in the build directory
#include <qpsk_mapper_vbc_pydoc.h>

void bind_qpsk_mapper_vbc(py::module& m)
{

    using qpsk_mapper_vbc    = gr::dab::qpsk_mapper_vbc;


    py::class_<qpsk_mapper_vbc,
        gr::sync_block,
        gr::block,
        gr::basic_block,
        std::shared_ptr<qpsk_mapper_vbc>>(m, "qpsk_mapper_vbc", D(qpsk_mapper_vbc))

        .def(py::init(&qpsk_mapper_vbc::make),
           py::arg("symbol_length"),
           D(qpsk_mapper_vbc,make)
        )
        



        ;




}







