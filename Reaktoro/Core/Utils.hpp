// Reaktoro is a unified framework for modeling chemically reactive systems.
//
// Copyright (C) 2014-2015 Allan Leal
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

#pragma once

// C++ includes
#include <string>
#include <vector>

// Reaktoro includes
#include <Reaktoro/Common/Index.hpp>
#include <Reaktoro/Common/Matrix.hpp>
#include <Reaktoro/Common/ScalarTypes.hpp>

namespace Reaktoro {

/// Return the names of the entries in a container.
template<typename NamedValues>
auto names(const NamedValues& values) -> std::vector<std::string>;

/// Return the electrical charges of all species in a list of species
template<typename ChargedValues>
auto charges(const ChargedValues& values) -> Vector;

/// Return the molar masses of all species in a list of species (in units of kg/mol)
template<typename SpeciesValues>
auto molarMasses(const SpeciesValues& species) -> Vector;

/// Return the index of an entry in a container.
template<typename NamedValues>
auto index(const std::string& name, const NamedValues& values) -> Index;

/// Return the index of an entry in a container.
template<typename NamedValue, typename NamedValues>
auto index(const NamedValue& value, const NamedValues& values) -> Index;

/// Return the index of the first entry in a container of named values with any of the given names.
template<typename Names, typename NamedValues>
auto indexAny(const Names& names, const NamedValues& values) -> Index;

/// Return the indices of some entries in a container.
template<typename NamedValues>
auto indices(const std::vector<std::string>& names, const NamedValues& values) -> Indices;

/// Return the indices of some entries in a container.
template<typename NamedValues>
auto indices(const NamedValues& subvalues, const NamedValues& values) -> Indices;

/// Return true if a named value is in a set of values.
template<typename NamedValues>
auto contained(const std::string& name, const NamedValues& values) -> bool;

/// Return the molar fractions of the species.
auto molarFractions(const Eigen::Ref<const Vector>& n) -> ChemicalVector;

} // namespace Reaktoro

#include <Reaktoro/Core/Utils.hxx>
