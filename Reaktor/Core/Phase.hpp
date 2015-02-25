// Reaktor is a C++ library for computational reaction modelling.
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

#pragma once

// C++ includes
#include <memory>
#include <string>

// Reaktor includes
#include <Reaktor/Core/Species.hpp>

namespace Reaktor {

// Forward declarations
class Species;

/// The type that defines the attributes of a Phase instance
/// @see Phase
/// @ingroup Core
struct PhaseData
{
    /// The name of the phase
    std::string name;

    /// The list of Species instances defining the phase
    std::vector<Species> species;
};

/// A type used to define a phase and its attributes.
/// @see ChemicalSystem, Element, Species
/// @ingroup Core
class Phase
{
public:
    /// Construct a default Phase instance
    Phase();

    /// Construct a custom Phase instance with all its attributes
    Phase(const PhaseData& data);

    /// Construct a custom Phase instance with all its attributes
    Phase(std::string name, std::vector<Species> species);

    /// Get the number of elements in the phase
    auto numElements() const -> unsigned;

    /// Get the number of species in the phase
    auto numSpecies() const -> unsigned;

    /// Get the name of the phase
    auto name() const -> const std::string&;

    /// Get the elements of the phase
    auto elements() const -> const std::vector<Element>&;

    /// Get the species of the phase
    auto species() const -> const std::vector<Species>&;

private:
    struct Impl;

    std::shared_ptr<Impl> pimpl;
};

/// Compare two Phase instances for less than
auto operator<(const Phase& lhs, const Phase& rhs) -> bool;

/// Compare two Phase instances for equality
auto operator==(const Phase& lhs, const Phase& rhs) -> bool;

/// Return a list of species (in order of appearance) in a list of phases
auto collectSpecies(const std::vector<Phase>& phases) -> std::vector<Species>;

} // namespace Reaktor
