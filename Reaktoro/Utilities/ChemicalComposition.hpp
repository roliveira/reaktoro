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

// Reaktoro includes
#include <Reaktoro/Common/ChemicalScalar.hpp>
#include <Reaktoro/Common/Matrix.hpp>

namespace Reaktoro {

// Forward declarations
class ChemicalState;
class ChemicalSystem;
class EquilibriumProblem;
class Partition;

/// A type that contains the values of a scalar field and its derivatives.
class ChemicalComposition
{
public:
    /// Construct a default ChemicalComposition instance.
    ChemicalComposition();

    /// Construct a custom ChemicalComposition instance.
    ChemicalComposition(const ChemicalSystem& system);

    /// Construct a copy of a ChemicalComposition instance.
    ChemicalComposition(const ChemicalComposition& other);

    /// Destroy this instance.
    virtual ~ChemicalComposition();

    /// Construct a copy of a ChemicalComposition instance.
    auto operator=(ChemicalComposition other) -> ChemicalComposition&;

    /// Return the chemical system.
    auto system() const -> const ChemicalSystem&;

    /// Return the partition of the chemical system.
    auto partition() const -> const Partition&;

    /// Set the partition of the chemical system.
    auto setPartition(const Partition& partition) -> void;

    /// Set the temperature for the equilibrium calculation.
    /// @param value The temperature value.
    /// @param units The temperature units.
    auto setTemperature(double value, std::string units) -> void;

    /// Set the pressure for the equilibrium calculation.
    /// @param value The pressure value.
    /// @param units The pressure units.
    auto setPressure(double value, std::string units) -> void;

    /// Set the composition of the aqueous phase using molalities of compounds.
    /// The compounds and their molalities are separated by semicollon.
    /// The following describes how to set the composition of an aqueous phase
    /// with 1 molal of NaCl and 1 mmolal MgCl2:
    /// ~~~
    /// ChemicalComposition composition(system);
    /// composition.aqueous("1 molal NaCl; 1 mmolal MgCl2");
    /// ~~~
    auto setAqueousFluid(std::string molalities) -> void;

    /// Set the composition of the gaseous phase using molar fractions of compounds.
    /// The compounds and their molar fractions are separated by semicollon.
    /// The following describes how to set the composition of a gas phase
    /// with 70% N2, 20% O2, and 10% CO2 (molar percentage):
    /// ~~~
    /// ChemicalComposition composition(system);
    /// composition.gaseous("0.70 N2; 0.20 O2; 0.10 CO2");
    /// ~~~
    auto setGaseousFluid(std::string molarfractions) -> void;

    /// Set the volume fractions of the solid phases.
    /// The composition of the solid part of the system is defined using
    /// volume fractions of each solid phase. The volume fraction of a solid
    /// phase is defined as the volume of that phase divided by total solid volume.
    /// The following describes how to set the volume fractions of solid
    /// phases `Calcite` and `Quartz`.
    /// ~~~
    /// ChemicalComposition composition(system);
    /// composition.solid("0.10 Calcite; 0.90 Quartz");
    /// ~~~
    auto setSolid(std::string volumefractions) -> void;

    /// Set the saturation of the aqueous fluid.
    /// The saturation of the aqueous fluid is defined as the ratio
    /// of its volume and the total fluid volume.
    auto setAqueousSaturation(double value) -> void;

    /// Set the saturation of the gaseous fluid.
    /// The saturation of the gaseous fluid is defined as the ratio
    /// of its volume and the total fluid volume.
    auto setGaseousSaturation(double value) -> void;

    /// Set the porosity of the solid matrix.
    /// The porosity is defined as the total fluid volume divided by total volume.
    auto setPorosity(double value) -> void;

    /// Convert this ChemicalComposition instance into an EquilibriumProblem instance.
    /// This conversion is needed to calculate the equilibrium state of both fluid and
    /// solid phases using their given compositions and volume conditions.
    /// Note that the calculated equilibrium state will satisfy the given fluid phase
    /// saturations and solid matrix porosity. The internal equilibrium composition of
    /// each phase might differ from those provided.
    /// For example, assume the aqueous and gaseous phases are set as:
    /// ~~~
    /// composition.setAqueousFluid("1 molal NaCl");
    /// composition.setGaseousFluid("0.95 CO2; 0.05 O2");
    /// ChemicalState state = equilibrate(composition);
    /// ~~~
    /// When both phases are equilibrated, enouth amount of gas with prescribed
    /// composition will be added in the system to satisfy the saturation of the
    /// gaseous phase. As a result, the aqueous phase will become saturated with
    /// both CO2 and O2. Thus, its final composition will contain a saturated
    /// molality of CO2 and O2 in addition to NaCl.
    operator EquilibriumProblem();

private:
    struct Impl;

    std::unique_ptr<Impl> pimpl;
};

} // namespace Reaktoro
