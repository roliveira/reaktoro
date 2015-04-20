// Reaktoro is a C++ library for computational reaction modelling.
//
// Copyright (C) 2014 Allan Leal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "PyKineticSolver.hpp"

// Boost includes
#include <boost/python.hpp>
namespace py = boost::python;

// Reaktoro includes
#include <Reaktoro/Core/ChemicalState.hpp>
#include <Reaktoro/Core/ReactionSystem.hpp>
#include <Reaktoro/Core/Partition.hpp>
#include <Reaktoro/Kinetics/KineticOptions.hpp>
#include <Reaktoro/Kinetics/KineticSolver.hpp>

namespace Reaktoro {

auto export_KineticSolver() -> void
{
    auto setPartition1 = static_cast<void(KineticSolver::*)(const Partition&)>(&KineticSolver::setPartition);
    auto setPartition2 = static_cast<void(KineticSolver::*)(std::string)>(&KineticSolver::setPartition);

    auto step1 = static_cast<void(KineticSolver::*)(ChemicalState&, double&)>(&KineticSolver::step);
    auto step2 = static_cast<void(KineticSolver::*)(ChemicalState&, double&, double)>(&KineticSolver::step);

    py::class_<KineticSolver>("KineticSolver", py::no_init)
        .def(py::init<const ReactionSystem&>())
        .def("setOptions", &KineticSolver::setOptions)
        .def("setPartition", setPartition1)
        .def("setPartition", setPartition2)
        .def("initialize", &KineticSolver::initialize)
        .def("step", step1)
        .def("step", step2)
        .def("solve", &KineticSolver::solve)
        ;
}

} // namespace Reaktoro