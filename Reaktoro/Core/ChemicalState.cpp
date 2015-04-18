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

#include "ChemicalState.hpp"

// C++ includes
#include <iomanip>
#include <iostream>

// Reaktoro includes
#include <Reaktoro/Common/Exception.hpp>
#include <Reaktoro/Common/StringUtils.hpp>
#include <Reaktoro/Common/Units.hpp>
#include <Reaktoro/Core/ChemicalSystem.hpp>
#include <Reaktoro/Core/Utils.hpp>
#include <Reaktoro/Thermodynamics/Water/WaterConstants.hpp>

namespace Reaktoro {
namespace {

auto errorNonAmountOrMassUnits(std::string units) -> void
{
    Exception exception;
    exception.error << "Cannot set the amount of the species.";
    exception.reason << "The provided units `" << units << "` is not convertible to units of amount or mass (e.g., mol and kg).";
    RaiseError(exception);
}

} // namespace

struct ChemicalState::Impl
{
    /// The chemical system instance
    ChemicalSystem system;

    /// The temperature state of the chemical system (in units of K)
    double T = 298.15;

    /// The pressure state of the chemical system (in units of Pa)
    double P = 1.0e+05;

    /// The molar amounts of the chemical species
    Vector n;

    /// The Lagrange multipliers with respect to the balance constraints (in units of J/mol)
    Vector y;

    /// The Lagrange multipliers with respect to the bound constraints of the species (in units of J/mol)
    Vector z;

    /// Construct a default ChemicalState::Impl instance
    Impl()
    {}

    /// Construct a custom ChemicalState::Impl instance
    Impl(const ChemicalSystem& system)
    : system(system)
    {
        n = zeros(system.numSpecies());
        y = zeros(system.numElements());
        z = zeros(system.numSpecies());
    }
};

ChemicalState::ChemicalState()
: pimpl(new Impl())
{}

ChemicalState::ChemicalState(const ChemicalSystem& system)
: pimpl(new Impl(system))
{}

ChemicalState::ChemicalState(const ChemicalState& other)
: pimpl(new Impl(*other.pimpl))
{}

ChemicalState::~ChemicalState()
{}

auto ChemicalState::operator=(ChemicalState other) -> ChemicalState&
{
    pimpl = std::move(other.pimpl);
    return *this;
}

auto ChemicalState::setTemperature(double val) -> void
{
    Assert(val > 0.0, "Cannot set temperature of the chemical state with a non-positive value.", "");
    pimpl->T = val;
}

auto ChemicalState::setTemperature(double val, std::string units) -> void
{
    setTemperature(units::convert(val, units, "kelvin"));
}

auto ChemicalState::setPressure(double val) -> void
{
    Assert(val > 0.0, "Cannot set pressure of the chemical state with a non-positive value.", "");
    pimpl->P = val;
}

auto ChemicalState::setPressure(double val, std::string units) -> void
{
    setPressure(units::convert(val, units, "pascal"));
}

auto ChemicalState::setSpeciesAmounts(double val) -> void
{
    Assert(val >= 0.0,
        "Cannot set the molar amounts of the species.",
        "The given molar abount is negative.");
    pimpl->n.fill(val);
}

auto ChemicalState::setSpeciesAmounts(const Vector& n) -> void
{
    Assert(n.rows() == system().numSpecies(),
        "Cannot set the molar amounts of the species.",
        "The dimension of the molar abundance vector "
        "is different than the number of species.");
    pimpl->n = n;
}

auto ChemicalState::setSpeciesAmounts(const Vector& n, const Indices& indices) -> void
{
    Assert(n.rows() == indices.size(),
        "Cannot set the molar amounts of the species with given indices.",
        "The dimension of the molar abundance vector "
        "is different than the number of indices.");
    rows(pimpl->n, indices) = n;
}

auto ChemicalState::setSpeciesAmount(Index index, double amount) -> void
{
    const unsigned num_species = system().species().size();
    Assert(amount >= 0.0,
        "Cannot set the molar amount of the species.",
        "The given molar amount is negative.");
    Assert(index < num_species,
        "Cannot set the molar amount of the species.",
        "The given index is out-of-range.");
    pimpl->n[index] = amount;
}

auto ChemicalState::setSpeciesAmount(std::string species, double amount) -> void
{
    const Index index = system().indexSpeciesWithError(species);
    setSpeciesAmount(index, amount);
}

auto ChemicalState::setSpeciesAmount(Index index, double amount, std::string units) -> void
{
    if(units::convertible(units, "mol"))
        setSpeciesAmount(index, units::convert(amount, units, "mol"));
    else if(units::convertible(units, "kg"))
    {
        const double molar_mass = system().species(index).molarMass();
        setSpeciesAmount(index, units::convert(amount, units, "kg")/molar_mass);
    }
    else errorNonAmountOrMassUnits(units);
}

auto ChemicalState::setSpeciesAmount(std::string species, double amount, std::string units) -> void
{
    const Index index = system().indexSpeciesWithError(species);
    setSpeciesAmount(index, amount, units);
}

auto ChemicalState::setElementPotentials(const Vector& y) -> void
{
    pimpl->y = y;
}

auto ChemicalState::setSpeciesPotentials(const Vector& z) -> void
{
    pimpl->z = z;
}

auto ChemicalState::setVolume(double volume) -> void
{
    Assert(volume >= 0.0, "Cannot set the volume of the chemical state.", "The given volume is negative.");
    const double T = temperature();
    const double P = pressure();
    const Vector& n = speciesAmounts();
    const Vector v = system().phaseVolumes(T, P, n).val;
    const double vtotal = sum(v);
    const double scalar = (vtotal != 0.0) ? volume/vtotal : 0.0;
    scaleSpeciesAmounts(scalar);
}

auto ChemicalState::setPhaseVolume(Index index, double volume) -> void
{
    Assert(volume >= 0.0, "Cannot set the volume of the phase.", "The given volume is negative.");
    Assert(index < system().numPhases(), "Cannot set the volume of the phase.", "The given phase index is out of range.");
    const double T = temperature();
    const double P = pressure();
    const Vector& n = speciesAmounts();
    const Vector v = system().phaseVolumes(T, P, n).val;
    const double scalar = (v[index] != 0.0) ? volume/v[index] : 0.0;
    scaleSpeciesAmountsInPhase(index, scalar);
}

auto ChemicalState::setPhaseVolume(std::string name, double volume) -> void
{
    const Index index = system().indexPhase(name);
    setPhaseVolume(index, volume);
}

auto ChemicalState::scaleSpeciesAmounts(double scalar) -> void
{
    Assert(scalar >= 0.0, "Cannot scale the molar amounts of the species.", "The given scalar is negative.");
    for(unsigned i = 0; i < pimpl->n.rows(); ++i)
        pimpl->n[i] *= scalar;
}

auto ChemicalState::scaleSpeciesAmountsInPhase(Index index, double scalar) -> void
{
    Assert(scalar >= 0.0, "Cannot scale the molar amounts of the species.", "The given scalar `" + std::to_string(scalar) << "` is negative.");
    Assert(index < system().numPhases(), "Cannot set the volume of the phase.", "The given phase index is out of range.");
    const Index start = system().indexFirstSpeciesInPhase(index);
    const Index size = system().numSpeciesInPhase(index);
    for(unsigned i = 0; i < size; ++i)
        pimpl->n[start + i] *= scalar;
}

auto ChemicalState::system() const -> const ChemicalSystem&
{
    return pimpl->system;
}

auto ChemicalState::temperature() const -> double
{
    return pimpl->T;
}

auto ChemicalState::pressure() const -> double
{
    return pimpl->P;
}

auto ChemicalState::speciesAmounts() const -> const Vector&
{
    return pimpl->n;
}

auto ChemicalState::elementPotentials() const -> const Vector&
{
    return pimpl->y;
}

auto ChemicalState::speciesPotentials() const -> const Vector&
{
    return pimpl->z;
}

auto ChemicalState::speciesAmount(Index index) const -> double
{
    Assert(index < system().numSpecies(),
        "Cannot get the molar amount of the species.",
        "The given index is out-of-range.");
    return pimpl->n[index];
}

auto ChemicalState::speciesAmount(std::string name) const -> double
{
    return speciesAmount(system().indexSpeciesWithError(name));
}

auto ChemicalState::speciesAmount(Index ispecies, std::string units) const -> double
{
    return units::convert(speciesAmount(ispecies), "mol", units);
}

auto ChemicalState::speciesAmount(std::string species, std::string units) const -> double
{
    return units::convert(speciesAmount(species), "mol", units);
}

auto ChemicalState::elementAmounts() const -> Vector
{
    return system().elementAmounts(speciesAmounts());
}

auto ChemicalState::elementAmountsInPhase(Index iphase) const -> Vector
{
    return system().elementAmountsInPhase(iphase, speciesAmounts());
}

auto ChemicalState::elementAmountsInSpecies(const Indices& ispecies) const -> Vector
{
    return system().elementAmountsInSpecies(ispecies, speciesAmounts());
}

auto ChemicalState::elementAmount(Index ielement) const -> double
{
    return system().elementAmount(ielement, speciesAmounts());
}

auto ChemicalState::elementAmount(std::string element) const -> double
{
    return elementAmount(system().indexElementWithError(element));
}

auto ChemicalState::elementAmount(Index index, std::string units) const -> double
{
    return units::convert(elementAmount(index), "mol", units);
}

auto ChemicalState::elementAmount(std::string name, std::string units) const -> double
{
    return units::convert(elementAmount(name), "mol", units);
}

auto ChemicalState::elementAmountInPhase(Index ielement, Index iphase) const -> double
{
    return system().elementAmountInPhase(ielement, iphase, speciesAmounts());
}

auto ChemicalState::elementAmountInPhase(std::string element, std::string phase) const -> double
{
    const unsigned ielement = system().indexElementWithError(element);
    const unsigned iphase = system().indexPhaseWithError(phase);
    return elementAmountInPhase(ielement, iphase);
}

auto ChemicalState::elementAmountInPhase(Index ielement, Index iphase, std::string units) const -> double
{
    return units::convert(elementAmountInPhase(ielement, iphase), "mol", units);
}

auto ChemicalState::elementAmountInPhase(std::string element, std::string phase, std::string units) const -> double
{
    return units::convert(elementAmountInPhase(element, phase), "mol", units);
}

auto ChemicalState::elementAmountInSpecies(Index ielement, const Indices& ispecies) const -> double
{
    return system().elementAmountInSpecies(ielement, ispecies, speciesAmounts());
}

auto ChemicalState::elementAmountInSpecies(Index ielement, const Indices& ispecies, std::string units) const -> double
{
    return units::convert(elementAmountInSpecies(ielement, ispecies), "mol", units);
}

auto operator<<(std::ostream& out, const ChemicalState& state) -> std::ostream&
{
    const ChemicalSystem& system = state.system();
    const double& T = state.temperature();
    const double& P = state.pressure();
    const Vector& n = state.speciesAmounts();
    const Vector u0 = system.standardGibbsEnergies(T, P).val;
    const Vector u  = system.chemicalPotentials(T, P, n).val;
    const Vector a  = system.activities(T, P, n).val;

    out << std::setw(10) << std::left << "Index";
    out << std::setw(20) << std::left << "Species";
    out << std::setw(20) << std::left << "Moles";
    out << std::setw(20) << std::left << "Activity";
    out << std::setw(20) << std::left << "GibbsEnergy";
    out << std::setw(20) << std::left << "ChemicalPotential";
    out << std::endl;
    for(unsigned i = 0; i < system.numSpecies(); ++i)
    {
        out << std::setw(10) << std::left << i;
        out << std::setw(20) << std::left << system.species(i).name();
        out << std::setw(20) << std::left << n[i];
        out << std::setw(20) << std::left << a[i];
        out << std::setw(20) << std::left << u0[i];
        out << std::setw(20) << std::left << u[i];
        out << std::endl;
    }
    return out;
}

auto operator+(const ChemicalState& l, const ChemicalState& r) -> ChemicalState
{
    const Vector& nl = l.speciesAmounts();
    const Vector& nr = r.speciesAmounts();
    ChemicalState res = l;
    res.setSpeciesAmounts(nl + nr);
    return res;
}

auto operator*(double scalar, const ChemicalState& state) -> ChemicalState
{
    ChemicalState res = state;
    res.scaleSpeciesAmounts(scalar);
    return res;
}

auto operator*(const ChemicalState& state, double scalar) -> ChemicalState
{
    return scalar*state;
}

auto extract(const ChemicalState& state, std::string str) -> double
{
    auto words = split(str, ":");

    std::string quantity = words[0];
    std::string units = words.size() > 1 ? words[1] : "";

    const auto& T = state.temperature();
    const auto& P = state.pressure();
    const auto& n = state.speciesAmounts();
    const auto& a = state.system().activities(T, P, n);

    if(quantity[0] == 'n')
    {
        units = units.empty() ? "mol" : units;
        std::string species = split(quantity, "[]").back();
        const double ni = state.speciesAmount(species, units);
        return ni;
    }
    if(quantity[0] == 'b')
    {
        units = units.empty() ? "mol" : units;
        auto names = split(quantity, "[]");
        std::string element = names[1];
        std::string phase = names.size() > 2 ? names[2] : "";
        const double bi = phase.empty() ?
            state.elementAmount(element, units) :
            state.elementAmountInPhase(element, phase, units);
        return bi;
    }
    if(quantity[0] == 'm')
    {
        units = units.empty() ? "molal" : units;
        std::string species = split(quantity, "[]").back();
        const double nH2O = state.speciesAmount("H2O(l)");
        const double ni = state.speciesAmount(species);
        const double mi = ni/(nH2O * waterMolarMass);
        return units::convert(mi, "molal", units);
    }
    if(quantity[0] == 'a')
    {
        std::string species = split(quantity, "[]").back();
        Index index = state.system().indexSpecies(species);
        return a.val[index];
    }
    if(quantity == "pH")
    {
        const Index iH = state.system().indexSpecies("H+");
        const double aH = a.val[iH];
        return -std::log10(aH);
    }

    RuntimeError("Cannot extract the quantity from the ChemicalState instance.",
        "The provided string `" + str + "` does not represent a valid quantity.");

    return 0.0;
}

} // namespace Reaktoro