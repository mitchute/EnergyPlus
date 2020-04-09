// EnergyPlus, Copyright (c) 1996-2020, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef GroundHeatExchangerEnhanced_hh_INCLUDED
#define GroundHeatExchangerEnhanced_hh_INCLUDED

// third-party
#include <btwxt.h>

// EnergyPlus Headers
#include <EnergyPlus/DataGlobals.hh>
#include <EnergyPlus/GroundTemperatureModeling/BaseGroundTemperatureModel.hh>
#include <EnergyPlus/EnergyPlus.hh>
#include <EnergyPlus/GroundTemperatureModeling/GroundTemperatureModelManager.hh>
#include <EnergyPlus/PlantComponent.hh>

namespace EnergyPlus {

namespace GroundHeatExchangerEnhanced {

struct FluidWorker
{
    // Members
    std::string fluidName;
    int loopNum;
    int fluidIdx;

    // Default constructor
    FluidWorker() : loopNum(0), fluidIdx(0)
    {
    }

    // Copy constructor
    FluidWorker(const FluidWorker &r) {
        fluidName = r.fluidName;
        loopNum = r.loopNum;
        fluidIdx = r.fluidIdx;
    }

    // Default destructor
    ~FluidWorker() = default;

    // Member methods
    void initialize(int _loopNum);
    Real64 getCp(Real64 const &temperature, const std::string &routineName);
    Real64 getCond(Real64 const &temperature, const std::string &routineName);
    Real64 getVisc(Real64 const &temperature, const std::string &routineName);
    Real64 getRho(Real64 const &temperature, const std::string &routineName);
    Real64 getPrandtl(Real64 const &temperature, const std::string &routineName);
};

    struct Pipe
    {
        Real64 k;
        Real64 rhoCp;
        Real64 innerDia;
        Real64 outerDia;
        Real64 innerRadius;
        Real64 outerRadius;
        Real64 wallThickness;
        FluidWorker fluid;
        Real64 friction;
        Real64 convResist;

        // default constructor
        Pipe() : k(0.0), rhoCp(0.0), innerDia(0.0), outerDia(0.0), innerRadius(0.0), outerRadius(0.0), wallThickness(0.0),
                 friction(0.0), convResist(0.0)
        {
        }

        // member constructor
        Pipe(Real64 const &_k, Real64 const &_rhoCp, Real64 const& _outerDia, Real64 const &_wallThickness)
        {
            k = _k; rhoCp = _rhoCp; outerDia = _outerDia;  wallThickness = _wallThickness; innerDia = 0.0; innerRadius = 0.0;
            outerRadius = 0.0; friction = 0.0; convResist = 0.0;
            initGeometry();
        }

        // copy constructor
        Pipe(Pipe const &r) {
            k = r.k; rhoCp = r.rhoCp; innerDia = r.innerDia; outerDia = r.outerDia; innerRadius = r.innerRadius;
            outerRadius = r.outerRadius; wallThickness = r.wallThickness; fluid = r.fluid; friction = r.friction;
            convResist = r.convResist;
        }

        // default destructor
        ~Pipe() = default;

        // member methods
        void initGeometry();
        Real64 mdotToRe(Real64 flowRate, Real64 temperature);
        static Real64 laminarFrictionFactor(Real64 Re);
        static Real64 turbulentFrictionFactor(Real64 Re);
        Real64 frictionFactor(Real64 Re);
        static Real64 laminarNusselt();
        Real64 turbulentNusselt(Real64 Re, Real64 temperature);
        Real64 convectionResistance(Real64 flowRate, Real64 temperature);
//        Real64 calcConductionResistance();
//        Real64 calcResistance(Real64 flowRate, Real64 temperature);
    };

    struct BoreholeProps
    {
        // members
        std::string propName;
        Real64 depth;
        Real64 length;
        Real64 diameter;
        Real64 kGrout;
        Real64 rhoCpGrout;
        Real64 shankSpace;
        Pipe pipe;

        // default constructor
        BoreholeProps() : depth(0.0), length(0.0), diameter(0.0), kGrout(0.0), rhoCpGrout(0.0), shankSpace(0.0)
        {
        }

        // copy constructor
        BoreholeProps(BoreholeProps const &r)
        {
            propName = r.propName; depth = r.depth; length = r.length; diameter = r.diameter; kGrout = r.kGrout; rhoCpGrout = r.rhoCpGrout;
            shankSpace = r.shankSpace; pipe = r.pipe;
        }

        // default destructor
        ~BoreholeProps() = default;
    };

    struct GHEBorehole : BoreholeProps
    {
        // members
        std::string name;
        Real64 xLoc;
        Real64 yLoc;

        // default constructor
        GHEBorehole() : xLoc(0.0), yLoc(0.0)
        {
        }

        // copy constructor
        GHEBorehole(GHEBorehole const &r) : BoreholeProps(r)
        {
            name = r.name; xLoc = r.xLoc; yLoc = r.yLoc;
        };

        // default destructor
        ~GHEBorehole() = default;
    };

    struct GHEArray
    {
        std::string name;
        std::vector<GHEBorehole> boreholes;

        // default constructor
        GHEArray() = default;

        // default destructor
        ~GHEArray() = default;
    };

    struct GHERespFactors
    {
        // members
        std::string name;
        BoreholeProps props;
        Btwxt::RegularGridInterpolator g;
        int maxSimYears;

        // default constructor
        GHERespFactors() : maxSimYears(0)
        {
        }

        // copy constructor
        GHERespFactors(GHERespFactors const &r) {
            name = r.name; props = r.props; g = r.g; maxSimYears = r.maxSimYears;
        }

        // default destructor
        ~GHERespFactors() = default;
    };

    struct EnhancedGHE : PlantComponent
    {
        // members
        bool oneTimeInit;
        std::string name;
        int inletNode;
        int outletNode;
        Real64 designVolFlow;
        int numBH;
        Real64 kSoil;
        Real64 rhoCpSoil;
        std::shared_ptr<BaseGroundTempsModel> gtm;
        Real64 aveBHWallTemp;
        Real64 heatRateToSoil;
        Real64 inletTemp;
        Real64 outletTemp;
        Real64 massFlowRate;
        Real64 aveFluidTemp;
        Real64 farFieldGroundTemp;
        bool gFuncEFTExist;
        bool gFuncBWTExist;
        GHERespFactors gFuncEFT;
        GHERespFactors gFuncBWT;
        std::vector<GHEBorehole> boreholes;
        GHEBorehole aveBH;
        int loopNum;
        int loopSideNum;
        int branchNum;
        int compNum;
        FluidWorker fluid;

        // default constructor
        EnhancedGHE() : oneTimeInit(true), inletNode(0), outletNode(0), designVolFlow(0.0), numBH(0), kSoil(0.0), rhoCpSoil(0.0), aveBHWallTemp(0.0),
                        heatRateToSoil(0.0), inletTemp(0.0), outletTemp(0.0), massFlowRate(0.0), aveFluidTemp(0.0), farFieldGroundTemp(0.0),
                        gFuncEFTExist(false), gFuncBWTExist(false), loopNum(0), loopSideNum(0), branchNum(0), compNum(0)
        {
        }

        // default destructor
        ~EnhancedGHE() override = default;

        // member methods
        static PlantComponent *factory(std::string const &objectName);
        void setupOutputVars();
        void simulate(const PlantLocation &calledFromLocation, bool FirstHVACIteration, Real64 &CurLoad, bool RunFlag) override;
        void onInitLoopEquip(const PlantLocation &calledFromLocation) override;
        void generateEFTgFunc();
        void generateBWTgFunc();
    };

    void clear_state();
    void getGHEInput();
    Real64 smoothingFunc(Real64 const &x, Real64 const &a, Real64 const &b);

    extern std::vector<EnhancedGHE> enhancedGHE;

} // namespace GroundHeatExchangerEnhanced

} // namespace EnergyPlus

#endif
