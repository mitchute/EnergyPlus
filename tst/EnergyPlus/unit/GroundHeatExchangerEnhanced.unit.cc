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

// EnergyPlus::GroundHeatExchangerEnhanced Unit Tests

// Google Test Headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/Plant/DataPlant.hh>
#include <EnergyPlus/GroundHeatExchangerEnhanced.hh>
#include <EnergyPlus/GroundTemperatureModeling/GroundTemperatureModelManager.hh>
#include <EnergyPlus/DataSizing.hh>

// Testing Headers
#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;
using namespace EnergyPlus::GroundHeatExchangerEnhanced;

TEST_F(EnergyPlusFixture, GHE_Enhanced_Good_Init_With_g_functions)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,  !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  4,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,        !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  EFT g-functions,         !- GHE:ResponseFactors EFT Object Name",
        "  BWT g-functions;         !- GHE:ResponseFactors BWT Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,  !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  5.1225E-02;              !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,        !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  EFT g-functions,  !- Name",
        "  GHE Props,  !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  BWT g-functions,  !- Name",
        "  GHE Props,  !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    EnhancedGHE::factory(state, "GHE");

    ASSERT_EQ(1u, state.dataGroundHeatExchangerEnhanced.enhancedGHE.size());

    EnhancedGHE *tstGHE = &state.dataGroundHeatExchangerEnhanced.enhancedGHE[0];

    EXPECT_EQ("GHE", tstGHE->name);
    EXPECT_EQ(0.00330000, tstGHE->designVolFlow);
    EXPECT_EQ(4, tstGHE->numBH);
    EXPECT_EQ(GroundTemperatureManager::objectType_KusudaGroundTemp, tstGHE->gtm->objectType);
    EXPECT_EQ(0.692626E+00, tstGHE->kSoil);
    EXPECT_EQ(0.234700E+07, tstGHE->rhoCpSoil);

    EXPECT_EQ("EFT G-FUNCTIONS", tstGHE->gFuncEFT.name);
    EXPECT_EQ("BWT G-FUNCTIONS", tstGHE->gFuncBWT.name);

    EXPECT_EQ(0u, tstGHE->boreholes.size());

    EXPECT_EQ(true, tstGHE->gFuncEFTExist);
    EXPECT_EQ(true, tstGHE->gFuncBWTExist);

    EXPECT_EQ(std::vector<Real64>({0.0}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({0.0})));
    EXPECT_EQ(std::vector<Real64>({0.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({0.5})));
    EXPECT_EQ(std::vector<Real64>({1.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({1.5})));
    EXPECT_EQ(std::vector<Real64>({2.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({2.5})));
    EXPECT_EQ(std::vector<Real64>({3.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({3.5})));
    EXPECT_EQ(std::vector<Real64>({-3.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({-3.5})));

    EXPECT_EQ(std::vector<Real64>({0.0}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({0.0})));
    EXPECT_EQ(std::vector<Real64>({0.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({0.5})));
    EXPECT_EQ(std::vector<Real64>({1.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({1.5})));
    EXPECT_EQ(std::vector<Real64>({2.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({2.5})));
    EXPECT_EQ(std::vector<Real64>({3.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({3.5})));
    EXPECT_EQ(std::vector<Real64>({-3.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({-3.5})));

    GHERespFactors *eft = &tstGHE->gFuncEFT;
    EXPECT_EQ("GHE PROPS", eft->props.propName);
    EXPECT_NEAR(1, eft->props.depth, 1E-1);
    EXPECT_NEAR(0.127016, eft->props.diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, eft->props.kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, eft->props.rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, eft->props.shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, eft->props.pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, eft->props.pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, eft->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, eft->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, eft->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, eft->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, eft->props.pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, eft->props.pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, eft->props.pipe.wallThickness, 1E-3);

    GHERespFactors *bwt = &tstGHE->gFuncBWT;
    EXPECT_EQ("GHE PROPS", bwt->props.propName);
    EXPECT_NEAR(1, bwt->props.depth, 1E-1);
    EXPECT_NEAR(0.127016, bwt->props.diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bwt->props.kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bwt->props.rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, bwt->props.shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bwt->props.pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bwt->props.pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bwt->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bwt->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bwt->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bwt->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bwt->props.pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bwt->props.pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bwt->props.pipe.wallThickness, 1E-3);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Good_Init_With_Array)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,  !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  4,                      !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,  !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  ,                        !- GHE:ResponseFactors EFT Object Name",
        "  ,                        !- GHE:ResponseFactors BWT Object Name",
        "  GHE-Array;               !- GHT:Vertical:Array Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,  !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  5.1225E-02;              !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,  !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:Vertical:Array,",
        "  GHE-Array,               !- Name",
        "  GHE Props,  !- GHE:Vertical:Properties Object Name",
        "  2,                       !- Number of Boreholes in X-Direction",
        "  2,                       !- Number of Boreholes in Y-Direction",
        "  5;                       !- Borehole Spacing {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    EnhancedGHE::factory(state, "GHE");

    ASSERT_EQ(1u, state.dataGroundHeatExchangerEnhanced.enhancedGHE.size());

    EnhancedGHE *tstGHE = &state.dataGroundHeatExchangerEnhanced.enhancedGHE[0];

    EXPECT_EQ("GHE", tstGHE->name);
    EXPECT_EQ(0.00330000, tstGHE->designVolFlow);
    EXPECT_EQ(4, tstGHE->numBH);
    EXPECT_EQ(GroundTemperatureManager::objectType_KusudaGroundTemp, tstGHE->gtm->objectType);
    EXPECT_EQ(0.692626E+00, tstGHE->kSoil);
    EXPECT_EQ(0.234700E+07, tstGHE->rhoCpSoil);

    EXPECT_EQ(4u, tstGHE->boreholes.size());

    EXPECT_EQ(false, tstGHE->gFuncEFTExist);
    EXPECT_EQ(false, tstGHE->gFuncBWTExist);

    GHEBorehole *bh0 = &tstGHE->boreholes[0];
    EXPECT_EQ(0.0, bh0->xLoc);
    EXPECT_EQ(0.0, bh0->yLoc);
    EXPECT_EQ("GHE PROPS", bh0->propName);
    EXPECT_NEAR(1, bh0->depth, 1E-1);
    EXPECT_NEAR(0.127016, bh0->diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bh0->kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bh0->rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, bh0->shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bh0->pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bh0->pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bh0->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh0->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bh0->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh0->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bh0->pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bh0->pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bh0->pipe.wallThickness, 1E-3);

    GHEBorehole *bh1 = &tstGHE->boreholes[1];
    EXPECT_EQ(0.0, bh1->xLoc);
    EXPECT_EQ(5.0, bh1->yLoc);
    EXPECT_EQ("GHE PROPS", bh1->propName);
    EXPECT_NEAR(1, bh1->depth, 1E-1);
    EXPECT_NEAR(0.127016, bh1->diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bh1->kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bh1->rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, bh1->shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bh1->pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bh1->pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bh1->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh1->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bh1->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh1->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bh1->pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bh1->pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bh1->pipe.wallThickness, 1E-3);

    GHEBorehole *bh2 = &tstGHE->boreholes[2];
    EXPECT_EQ(5.0, bh2->xLoc);
    EXPECT_EQ(0.0, bh2->yLoc);
    EXPECT_EQ("GHE PROPS", bh2->propName);
    EXPECT_NEAR(1, bh2->depth, 1E-1);
    EXPECT_NEAR(0.127016, bh2->diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bh2->kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bh2->rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, bh2->shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bh2->pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bh2->pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bh2->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh2->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bh2->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh2->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bh2->pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bh2->pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bh2->pipe.wallThickness, 1E-3);

    GHEBorehole *bh3 = &tstGHE->boreholes[3];
    EXPECT_EQ(5.0, bh3->xLoc);
    EXPECT_EQ(5.0, bh3->yLoc);
    EXPECT_EQ("GHE PROPS", bh3->propName);
    EXPECT_NEAR(1, bh3->depth, 1E-1);
    EXPECT_NEAR(0.127016, bh3->diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bh3->kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bh3->rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, bh3->shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bh3->pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bh3->pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bh3->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh3->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bh3->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh3->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bh3->pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bh3->pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bh3->pipe.wallThickness, 1E-3);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Good_Init_With_Boreholes)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,  !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  4,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,        !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  ,                        !- GHE:ResponseFactors EFT Object Name",
        "  ,                        !- GHE:ResponseFactors BWT Object Name",
        "  ,                        !- GHT:Vertical:Array Object Name",
        "  BH-1,                    !- GHT:Vertical:Single Object Name 1",
        "  BH-2,                    !- GHT:Vertical:Single Object Name 2",
        "  BH-3,                    !- GHT:Vertical:Single Object Name 3",
        "  BH-4;                    !- GHT:Vertical:Single Object Name 4",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,               !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  5.1225E-02;              !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,        !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:Vertical:Single,",
        "  BH-1,                    !- Name",
        "  GHE Props,               !- GHE Props",
        "  0,                       !- X Location {m}",
        "  0;                       !- Y Location {m}",
        "",
        "GroundHeatExchanger:Vertical:Single,",
        "  BH-2,                    !- Name",
        "  GHE Props,               !- GHE Props",
        "  0,                       !- X Location {m}",
        "  5;                       !- Y Location {m}",
        "",
        "GroundHeatExchanger:Vertical:Single,",
        "  BH-3,                    !- Name",
        "  GHE Props,               !- GHE Props",
        "  5,                       !- X Location {m}",
        "  0;                       !- Y Location {m}",
        "",
        "GroundHeatExchanger:Vertical:Single,",
        "  BH-4,                    !- Name",
        "  GHE Props,               !- GHE Props",
        "  5,                       !- X Location {m}",
        "  5;                       !- Y Location {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    EnhancedGHE::factory(state, "GHE");

    ASSERT_EQ(1u, state.dataGroundHeatExchangerEnhanced.enhancedGHE.size());

    EnhancedGHE *tstGHE = &state.dataGroundHeatExchangerEnhanced.enhancedGHE[0];

    EXPECT_EQ("GHE", tstGHE->name);
    EXPECT_EQ(0.00330000, tstGHE->designVolFlow);
    EXPECT_EQ(4, tstGHE->numBH);
    EXPECT_EQ(GroundTemperatureManager::objectType_KusudaGroundTemp, tstGHE->gtm->objectType);
    EXPECT_EQ(0.692626E+00, tstGHE->kSoil);
    EXPECT_EQ(0.234700E+07, tstGHE->rhoCpSoil);

    EXPECT_EQ(4u, tstGHE->boreholes.size());

    EXPECT_EQ(false, tstGHE->gFuncEFTExist);
    EXPECT_EQ(false, tstGHE->gFuncBWTExist);

    GHEBorehole *bh0 = &tstGHE->boreholes[0];
    EXPECT_EQ(0.0, bh0->xLoc);
    EXPECT_EQ(0.0, bh0->yLoc);
    EXPECT_EQ("GHE PROPS", bh0->propName);
    EXPECT_NEAR(1, bh0->depth, 1E-1);
    EXPECT_NEAR(0.127016, bh0->diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bh0->kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bh0->rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, bh0->shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bh0->pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bh0->pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bh0->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh0->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bh0->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh0->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bh0->pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bh0->pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bh0->pipe.wallThickness, 1E-3);

    GHEBorehole *bh1 = &tstGHE->boreholes[1];
    EXPECT_EQ(0.0, bh1->xLoc);
    EXPECT_EQ(5.0, bh1->yLoc);
    EXPECT_EQ("GHE PROPS", bh1->propName);
    EXPECT_NEAR(1, bh1->depth, 1E-1);
    EXPECT_NEAR(0.127016, bh1->diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bh1->kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bh1->rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, bh1->shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bh1->pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bh1->pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bh1->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh1->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bh1->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh1->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bh1->pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bh1->pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bh1->pipe.wallThickness, 1E-3);

    GHEBorehole *bh2 = &tstGHE->boreholes[2];
    EXPECT_EQ(5.0, bh2->xLoc);
    EXPECT_EQ(0.0, bh2->yLoc);
    EXPECT_EQ("GHE PROPS", bh2->propName);
    EXPECT_NEAR(1, bh2->depth, 1E-1);
    EXPECT_NEAR(0.127016, bh2->diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bh2->kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bh2->rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, bh2->shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bh2->pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bh2->pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bh2->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh2->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bh2->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh2->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bh2->pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bh2->pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bh2->pipe.wallThickness, 1E-3);

    GHEBorehole *bh3 = &tstGHE->boreholes[3];
    EXPECT_EQ(5.0, bh3->xLoc);
    EXPECT_EQ(5.0, bh3->yLoc);
    EXPECT_EQ("GHE PROPS", bh3->propName);
    EXPECT_NEAR(1, bh3->depth, 1E-1);
    EXPECT_NEAR(0.127016, bh3->diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bh3->kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bh3->rhoCpGrout, 1);
    EXPECT_NEAR(5.1225E-02, bh3->shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bh3->pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bh3->pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bh3->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh3->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bh3->pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bh3->pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bh3->pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bh3->pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bh3->pipe.wallThickness, 1E-3);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Good_Init_With_Min_Shankspace)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,  !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  4,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,  !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  EFT g-functions,  !- GHE:ResponseFactors EFT Object Name",
        "  BWT g-functions;  !- GHE:ResponseFactors BWT Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,  !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  0.001;                   !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,  !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  EFT g-functions,  !- Name",
        "  GHE Props,  !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  BWT g-functions,  !- Name",
        "  GHE Props,  !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    EnhancedGHE::factory(state, "GHE");

    ASSERT_EQ(1u, state.dataGroundHeatExchangerEnhanced.enhancedGHE.size());

    EnhancedGHE *tstGHE = &state.dataGroundHeatExchangerEnhanced.enhancedGHE[0];

    EXPECT_EQ("GHE", tstGHE->name);
    EXPECT_EQ(0.00330000, tstGHE->designVolFlow);
    EXPECT_EQ(4, tstGHE->numBH);
    EXPECT_EQ(GroundTemperatureManager::objectType_KusudaGroundTemp, tstGHE->gtm->objectType);
    EXPECT_EQ(0.692626E+00, tstGHE->kSoil);
    EXPECT_EQ(0.234700E+07, tstGHE->rhoCpSoil);

    EXPECT_EQ("EFT G-FUNCTIONS", tstGHE->gFuncEFT.name);
    EXPECT_EQ("BWT G-FUNCTIONS", tstGHE->gFuncBWT.name);

    EXPECT_EQ(0u, tstGHE->boreholes.size());

    EXPECT_EQ(true, tstGHE->gFuncEFTExist);
    EXPECT_EQ(true, tstGHE->gFuncBWTExist);

    EXPECT_EQ(std::vector<Real64>({0.0}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({0.0})));
    EXPECT_EQ(std::vector<Real64>({0.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({0.5})));
    EXPECT_EQ(std::vector<Real64>({1.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({1.5})));
    EXPECT_EQ(std::vector<Real64>({2.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({2.5})));
    EXPECT_EQ(std::vector<Real64>({3.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({3.5})));
    EXPECT_EQ(std::vector<Real64>({-3.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({-3.5})));

    EXPECT_EQ(std::vector<Real64>({0.0}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({0.0})));
    EXPECT_EQ(std::vector<Real64>({0.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({0.5})));
    EXPECT_EQ(std::vector<Real64>({1.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({1.5})));
    EXPECT_EQ(std::vector<Real64>({2.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({2.5})));
    EXPECT_EQ(std::vector<Real64>({3.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({3.5})));
    EXPECT_EQ(std::vector<Real64>({-3.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({-3.5})));

    GHERespFactors *eft = &tstGHE->gFuncEFT;
    EXPECT_EQ("GHE PROPS", eft->props.propName);
    EXPECT_NEAR(1, eft->props.depth, 1E-1);
    EXPECT_NEAR(0.127016, eft->props.diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, eft->props.kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, eft->props.rhoCpGrout, 1);
    EXPECT_NEAR(2.66667E-02, eft->props.shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, eft->props.pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, eft->props.pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, eft->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, eft->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, eft->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, eft->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, eft->props.pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, eft->props.pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, eft->props.pipe.wallThickness, 1E-3);

    GHERespFactors *bwt = &tstGHE->gFuncBWT;
    EXPECT_EQ("GHE PROPS", bwt->props.propName);
    EXPECT_NEAR(1, bwt->props.depth, 1E-1);
    EXPECT_NEAR(0.127016, bwt->props.diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bwt->props.kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bwt->props.rhoCpGrout, 1);
    EXPECT_NEAR(2.66667E-02, bwt->props.shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bwt->props.pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bwt->props.pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bwt->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bwt->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bwt->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bwt->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bwt->props.pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bwt->props.pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bwt->props.pipe.wallThickness, 1E-3);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Good_Init_With_Max_Shankspace)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,  !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  4,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,  !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  EFT g-functions,  !- GHE:ResponseFactors EFT Object Name",
        "  BWT g-functions;  !- GHE:ResponseFactors BWT Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,  !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  0.127016;                !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,  !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  EFT g-functions,  !- Name",
        "  GHE Props,  !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  BWT g-functions,  !- Name",
        "  GHE Props,  !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    EnhancedGHE::factory(state, "GHE");

    ASSERT_EQ(1u, state.dataGroundHeatExchangerEnhanced.enhancedGHE.size());

    EnhancedGHE *tstGHE = &state.dataGroundHeatExchangerEnhanced.enhancedGHE[0];

    EXPECT_EQ("GHE", tstGHE->name);
    EXPECT_EQ(0.00330000, tstGHE->designVolFlow);
    EXPECT_EQ(4, tstGHE->numBH);
    EXPECT_EQ(GroundTemperatureManager::objectType_KusudaGroundTemp, tstGHE->gtm->objectType);
    EXPECT_EQ(0.692626E+00, tstGHE->kSoil);
    EXPECT_EQ(0.234700E+07, tstGHE->rhoCpSoil);

    EXPECT_EQ("EFT G-FUNCTIONS", tstGHE->gFuncEFT.name);
    EXPECT_EQ("BWT G-FUNCTIONS", tstGHE->gFuncBWT.name);

    EXPECT_EQ(0u, tstGHE->boreholes.size());

    EXPECT_EQ(true, tstGHE->gFuncEFTExist);
    EXPECT_EQ(true, tstGHE->gFuncBWTExist);

    EXPECT_EQ(std::vector<Real64>({0.0}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({0.0})));
    EXPECT_EQ(std::vector<Real64>({0.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({0.5})));
    EXPECT_EQ(std::vector<Real64>({1.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({1.5})));
    EXPECT_EQ(std::vector<Real64>({2.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({2.5})));
    EXPECT_EQ(std::vector<Real64>({3.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({3.5})));
    EXPECT_EQ(std::vector<Real64>({-3.5}), tstGHE->gFuncEFT.g.get_values_at_target(std::vector<Real64>({-3.5})));

    EXPECT_EQ(std::vector<Real64>({0.0}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({0.0})));
    EXPECT_EQ(std::vector<Real64>({0.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({0.5})));
    EXPECT_EQ(std::vector<Real64>({1.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({1.5})));
    EXPECT_EQ(std::vector<Real64>({2.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({2.5})));
    EXPECT_EQ(std::vector<Real64>({3.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({3.5})));
    EXPECT_EQ(std::vector<Real64>({-3.5}), tstGHE->gFuncBWT.g.get_values_at_target(std::vector<Real64>({-3.5})));

    GHERespFactors *eft = &tstGHE->gFuncEFT;
    EXPECT_EQ("GHE PROPS", eft->props.propName);
    EXPECT_NEAR(1, eft->props.depth, 1E-1);
    EXPECT_NEAR(0.127016, eft->props.diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, eft->props.kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, eft->props.rhoCpGrout, 1);
    EXPECT_NEAR(7.36826E-02, eft->props.shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, eft->props.pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, eft->props.pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, eft->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, eft->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, eft->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, eft->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, eft->props.pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, eft->props.pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, eft->props.pipe.wallThickness, 1E-3);

    GHERespFactors *bwt = &tstGHE->gFuncBWT;
    EXPECT_EQ("GHE PROPS", bwt->props.propName);
    EXPECT_NEAR(1, bwt->props.depth, 1E-1);
    EXPECT_NEAR(0.127016, bwt->props.diameter, 1E-6);
    EXPECT_NEAR(0.692626E+00, bwt->props.kGrout, 1E-6);
    EXPECT_NEAR(3.90E+06, bwt->props.rhoCpGrout, 1);
    EXPECT_NEAR(7.36826E-02, bwt->props.shankSpace, 1E-2);
    EXPECT_NEAR(0.391312E+00, bwt->props.pipe.k, 1E-3);
    EXPECT_NEAR(1.542E+06, bwt->props.pipe.rhoCp, 1);
    EXPECT_NEAR(2.66667E-02, bwt->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bwt->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.66667E-02, bwt->props.pipe.outerDia, 1E-3);
    EXPECT_NEAR(1.33334E-02, bwt->props.pipe.outerRadius, 1E-3);
    EXPECT_NEAR(2.18410E-02, bwt->props.pipe.innerDia, 1E-3);
    EXPECT_NEAR(1.09205E-02, bwt->props.pipe.innerRadius, 1E-3);
    EXPECT_NEAR(2.41285E-03, bwt->props.pipe.wallThickness, 1E-3);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Bad_Props_Name_Response_Factor)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:Vertical:Properties,",
        "  Bad Props Name,          !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  0.127016;                !- U-Tube Distance {m}",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  EFT-Name,                !- Name",
        "  GHE Props,               !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    ASSERT_ANY_THROW(EnhancedGHE::factory(state, "GHE"));
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Bad_Props_Name_Array)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:Vertical:Properties,",
        "  Bad Props Name,          !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  0.127016;                !- U-Tube Distance {m}",
        "",
        "GroundHeatExchanger:Vertical:Array,",
        "  GHE-Array,               !- Name",
        "  GHE Props,               !- GHE:Vertical:Properties Object Name",
        "  2,                       !- Number of Boreholes in X-Direction",
        "  2,                       !- Number of Boreholes in Y-Direction",
        "  5;                       !- Borehole Spacing {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    ASSERT_ANY_THROW(EnhancedGHE::factory(state, "GHE"));
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Bad_Props_Name_Borehole)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:Vertical:Properties,",
        "  Bad Props Name,          !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  0.127016;                !- U-Tube Distance {m}",
        "",
        "GroundHeatExchanger:Vertical:Single,",
        "  BH-1,                   !- Name",
        "  GHE Props,   !- GHE Props",
        "  0,                       !- X Location {m}",
        "  0;                       !- Y Location {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    ASSERT_ANY_THROW(EnhancedGHE::factory(state, "GHE"));
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Bad_EWT_g_function_Name)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,                     !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  4,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,        !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  Bad g-functions name,    !- GHE:ResponseFactors EFT Object Name",
        "  BWT g-functions;         !- GHE:ResponseFactors BWT Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,               !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  5.1225E-02;              !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,        !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  EFT g-functions,         !- Name",
        "  GHE Props,               !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  BWT g-functions,         !- Name",
        "  GHE Props,               !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    ASSERT_ANY_THROW(EnhancedGHE::factory(state, "GHE"));
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Bad_BWT_g_function_Name)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,                     !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  4,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,        !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  EFT g-functions,         !- GHE:ResponseFactors EFT Object Name",
        "  Bad g-function name;     !- GHE:ResponseFactors BWT Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,               !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  5.1225E-02;              !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,        !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  EFT g-functions,         !- Name",
        "  GHE Props,               !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  BWT g-functions,         !- Name",
        "  GHE Props,               !- GHE:Vertical:Properties Object Name",
        "  0.0,                     !- g-Function Ln(T/Ts) Value 1",
        "  0.0,                     !- g-Function g Value 1",
        "  1.0,                     !- g-Function Ln(T/Ts) Value 2",
        "  1.0,                     !- g-Function g Value 2",
        "  2.0,                     !- g-Function Ln(T/Ts) Value 3",
        "  2.0,                     !- g-Function g Value 3",
        "  3.0,                     !- g-Function Ln(T/Ts) Value 4",
        "  3.0;                     !- g-Function g Value 4",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    ASSERT_ANY_THROW(EnhancedGHE::factory(state, "GHE"));
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Bad_Num_Array)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,                     !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  5,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,        !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  ,                        !- GHE:ResponseFactors EFT Object Name",
        "  ,                        !- GHE:ResponseFactors BWT Object Name",
        "  GHE-Array;               !- GHE:Vertical:Array Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,               !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  5.1225E-02;              !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,        !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:Vertical:Array,",
        "  GHE-Array,               !- Name",
        "  GHE Props,               !- GHE:Vertical:Properties Object Name",
        "  2,                       !- Number of Boreholes in X-Direction",
        "  2,                       !- Number of Boreholes in Y-Direction",
        "  5;                       !- Borehole Spacing {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    ASSERT_ANY_THROW(EnhancedGHE::factory(state, "GHE"));
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Bad_Num_BH)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,                     !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  5,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,        !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  ,                        !- GHE:ResponseFactors EFT Object Name",
        "  ,                        !- GHE:ResponseFactors BWT Object Name",
        "  ,                        !- GroundHeatExchanger:Vertical:Array Object Name",
        "  BH-1,                    !- GroundHeatExchanger:Vertical:Single Object Name 1",
        "  BH-2,                    !- GroundHeatExchanger:Vertical:Single Object Name 2",
        "  BH-3,                    !- GroundHeatExchanger:Vertical:Single Object Name 3",
        "  BH-4;                    !- GroundHeatExchanger:Vertical:Single Object Name 4",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,               !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  76.2,                    !- Borehole Length {m}",
        "  0.127016,                !- Borehole Diameter {m}",
        "  0.692626E+00,            !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.391312E+00,            !- Pipe Thermal Conductivity {W/m-K}",
        "  1.542E+06,               !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  2.66667E-02,             !- Pipe Outer Diameter {m}",
        "  2.41285E-03,             !- Pipe Thickness {m}",
        "  5.1225E-02;              !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,        !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:Vertical:Single,",
        "  BH-1,                    !- Name",
        "  GHE Props,               !- GHE Props",
        "  0,                       !- X Location {m}",
        "  0;                       !- Y Location {m}",
        "",
        "GroundHeatExchanger:Vertical:Single,",
        "  BH-2,                    !- Name",
        "  GHE Props,               !- GHE Props",
        "  0,                       !- X Location {m}",
        "  5;                       !- Y Location {m}",
        "",
        "GroundHeatExchanger:Vertical:Single,",
        "  BH-3,                    !- Name",
        "  GHE Props,               !- GHE Props",
        "  5,                       !- X Location {m}",
        "  0;                       !- Y Location {m}",
        "",
        "GroundHeatExchanger:Vertical:Single,",
        "  BH-4,                    !- Name",
        "  GHE Props,               !- GHE Props",
        "  5,                       !- X Location {m}",
        "  5;                       !- Y Location {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    ASSERT_ANY_THROW(EnhancedGHE::factory(state, "GHE"));
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Fluid_Worker_Props)
{
    int loopNum = 1;
    DataPlant::PlantLoop.allocate(1);
    DataPlant::PlantLoop(loopNum).FluidIndex = 1;
    DataPlant::PlantLoop(loopNum).FluidName = "WATER";

    // test init
    FluidWorker tst;
    tst.initialize(loopNum);
    EXPECT_EQ(tst.fluidIdx, 1);
    EXPECT_EQ(tst.fluidName, "WATER");

    static std::string const routineName = "GHE_Enhanced_Fluid_Worker_Test_Props";
    Real64 const temp = 20.0;

    // test getCp
    EXPECT_NEAR(tst.getCp(temp, routineName), 4181, 1E-01);

    // test getCond
    EXPECT_NEAR(tst.getCond(temp, routineName), 0.598, 1E-3);

    // test getVisc
    EXPECT_NEAR(tst.getVisc(temp, routineName), 1E-3, 1E-03);

    // test getRho
    EXPECT_NEAR(tst.getRho(temp, routineName), 998.2, 1E-01);

    // test getPrandlt
    EXPECT_NEAR(tst.getPrandtl(temp, routineName), 7.0, 1E-02);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Pipe_Init_Geometry)
{
    Pipe tst(1.0, 1.0, 1.0, 0.1);
    EXPECT_NEAR(tst.k, 1.0, 1E-03);
    EXPECT_NEAR(tst.rhoCp, 1.0, 1E-03);
    EXPECT_NEAR(tst.outerDia, 1.0, 1E-03);
    EXPECT_NEAR(tst.wallThickness, 0.1, 1E-03);
    EXPECT_NEAR(tst.innerDia, 0.8, 1E-03);
    EXPECT_NEAR(tst.outerRadius, 0.5, 1E-03);
    EXPECT_NEAR(tst.innerRadius, 0.4, 1E-03);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Pipe_mdotToRe)
{
    int loopNum = 1;
    DataPlant::PlantLoop.allocate(1);
    DataPlant::PlantLoop(loopNum).FluidIndex = 1;
    DataPlant::PlantLoop(loopNum).FluidName = "WATER";

    // init fluid
    FluidWorker fluid;
    fluid.initialize(loopNum);

    // setup pipe
    Pipe tst(1.0, 1.0, 1.0, 0.1);
    tst.fluid = fluid;

    Real64 const mdot = 10;
    Real64 const temp = 20.0;
    EXPECT_NEAR(tst.mdotToRe(mdot, temp), 15890, 1.0);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Pipe_smoothingFunc)
{
    Real64 tol = 1E-3;
    EXPECT_NEAR(GroundHeatExchangerEnhanced::smoothingFunc(-8, 0, 1), 0.0, tol);
    EXPECT_NEAR(GroundHeatExchangerEnhanced::smoothingFunc(8, 0, 1), 1.0, tol);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Pipe_friction_factor)
{
    Pipe tst(0.389, 1.77E+06, 0.0267, 0.00243);

    Real64 reynoldsNum = 0;
    Real64 const tol = 0.000001;

    // laminar tests
    reynoldsNum = 100;
    EXPECT_NEAR(tst.frictionFactor(reynoldsNum), 64.0 / reynoldsNum, tol);

    reynoldsNum = 1000;
    EXPECT_NEAR(tst.frictionFactor(reynoldsNum), 64.0 / reynoldsNum, tol);

    reynoldsNum = 1400;
    EXPECT_NEAR(tst.frictionFactor(reynoldsNum), 64.0 / reynoldsNum, tol);

    // transitional tests
    reynoldsNum = 2000;
    EXPECT_NEAR(tst.frictionFactor(reynoldsNum), 0.034003503, tol);

    reynoldsNum = 3000;
    EXPECT_NEAR(tst.frictionFactor(reynoldsNum), 0.033446219, tol);

    reynoldsNum = 4000;
    EXPECT_NEAR(tst.frictionFactor(reynoldsNum), 0.03895358, tol);

    // turbulent tests
    reynoldsNum = 5000;
    EXPECT_NEAR(tst.frictionFactor(reynoldsNum), pow(0.79 * log(reynoldsNum) - 1.64, -2.0), tol);

    reynoldsNum = 15000;
    EXPECT_NEAR(tst.frictionFactor(reynoldsNum), pow(0.79 * log(reynoldsNum) - 1.64, -2.0), tol);

    reynoldsNum = 25000;
    EXPECT_NEAR(tst.frictionFactor(reynoldsNum), pow(0.79 * log(reynoldsNum) - 1.64, -2.0), tol);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Pipe_convection_resist)
{
    int loopNum = 1;
    DataPlant::PlantLoop.allocate(1);
    DataPlant::PlantLoop(loopNum).FluidIndex = 1;
    DataPlant::PlantLoop(loopNum).FluidName = "WATER";

    // init fluid
    FluidWorker fluid;
    fluid.initialize(loopNum);

    Pipe tst(0.4, 1.805E+06, 0.0334, 0.00325);
    tst.fluid = fluid;

    Real64 const tol = 1E-06;

    Real64 const temperature = 20;
    Real64 mdot = 0.0;

    // Laminar
    Real64 resisLam = tst.calcConvResist(mdot, temperature);
    EXPECT_NEAR(resisLam, 0.132652, tol);

    // Transitional
    mdot = 0.07;
    Real64 resitTrans = tst.calcConvResist(mdot, temperature);
    EXPECT_NEAR(resitTrans, 0.026448, tol);

    // Turbulent
    mdot = 2.0;
    Real64 resistTurb = tst.calcConvResist(mdot, temperature);
    EXPECT_NEAR(resistTurb, 0.000932, tol);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Pipe_conduction_resist)
{
    Pipe tst(0.4, 1.805E+06, 0.0334, 0.00325);

    Real64 const tol = 1E-06;

    Real64 condResist = tst.calcCondResist();
    EXPECT_NEAR(condResist, 0.0861146, tol);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Pipe_total_resist)
{
    int loopNum = 1;
    DataPlant::PlantLoop.allocate(1);
    DataPlant::PlantLoop(loopNum).FluidIndex = 1;
    DataPlant::PlantLoop(loopNum).FluidName = "WATER";

    // init fluid
    FluidWorker fluid;
    fluid.initialize(loopNum);

    Pipe tst(0.4, 1.805E+06, 0.0334, 0.00325);
    tst.fluid = fluid;

    Real64 const tol = 1E-06;

    Real64 const temperature = 20;
    Real64 mdot = 0.0;

    // Laminar
    Real64 totResist = tst.calcResistance(mdot, temperature);
    EXPECT_NEAR(totResist, 0.132652 + 0.0861146, tol);

    // Transitional
    mdot = 0.07;
    totResist = tst.calcResistance(mdot, temperature);
    EXPECT_NEAR(totResist, 0.026448 + 0.0861146, tol);

    // Turbulent
    mdot = 2.0;
    totResist = tst.calcResistance(mdot, temperature);
    EXPECT_NEAR(totResist, 0.000932 + 0.0861146, tol);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Borehole_tot_internal_resist)
{
    Real64 const tol = 1E-5;

    {
        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 0.6, 0.389, 0.032, 0.00243);
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcTotIntResist(0.05), 0.32365, tol);
    }

    {
        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 1.2, 0.389,0.032, 0.00243);
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcTotIntResist(0.05), 0.23126, tol);
    }

    {
        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 1.8, 0.389,0.032, 0.00243);
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcTotIntResist(0.05), 0.19830, tol);
    }

    {
        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 2.4, 0.389,0.032, 0.00243);
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcTotIntResist(0.05), 0.18070, tol);
    }

    {
        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 3.0, 0.389,0.032, 0.00243);
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcTotIntResist(0.05), 0.16947, tol);
    }

    {
        int loopNum = 1;
        DataPlant::PlantLoop.allocate(1);
        DataPlant::PlantLoop(loopNum).FluidIndex = 1;
        DataPlant::PlantLoop(loopNum).FluidName = "WATER";

        // init fluid
        FluidWorker fluid;
        fluid.initialize(loopNum);

        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 3.0, 0.389,0.032, 0.00243);
        tst.pipe.fluid = fluid;
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcTotIntResist(20, 0.1), 0.20799, tol);
    }
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Borehole_grout_resist)
{
    Real64 const tol = 1E-5;

    {
        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 0.6, 0.389,0.032, 0.00243);
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcGroutResist(0.05), 0.17701, tol);
    }

    {
        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 1.2, 0.389,0.032, 0.00243);
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcGroutResist(0.05), 0.09211, tol);
    }

    {
        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 1.8, 0.389,0.032, 0.00243);
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcGroutResist(0.05), 0.06329, tol);
    }

    {
        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 2.4, 0.389,0.032, 0.00243);
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcGroutResist(0.05), 0.04861, tol);
    }

    {
        int loopNum = 1;
        DataPlant::PlantLoop.allocate(1);
        DataPlant::PlantLoop(loopNum).FluidIndex = 1;
        DataPlant::PlantLoop(loopNum).FluidName = "WATER";

        // init fluid
        FluidWorker fluid;
        fluid.initialize(loopNum);

        GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 3.0, 0.389, 0.032, 0.00243);
        tst.pipe.fluid = fluid;
        EXPECT_NEAR(tst.theta1, 0.33333, tol);
        EXPECT_NEAR(tst.theta2, 3.0, tol);
        EXPECT_NEAR(tst.calcGroutResist(20, 0.1), 0.04068, tol);
    }
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Borehole_direct_coupling_resist)
{
    Real64 const tol = 1E-5;

    int loopNum = 1;
    DataPlant::PlantLoop.allocate(1);
    DataPlant::PlantLoop(loopNum).FluidIndex = 1;
    DataPlant::PlantLoop(loopNum).FluidName = "WATER";

    // init fluid
    FluidWorker fluid;
    fluid.initialize(loopNum);

    GHEBorehole tst(0.096, 0.032, 4.0, 8E+05, 3.0, 0.389, 0.032, 0.00243);
    tst.pipe.fluid = fluid;
    EXPECT_NEAR(tst.calcDirectCouplingResist(20, 0.1), 0.68937, tol);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_Borehole_short_timestep_g_functions)
{
    Real64 const tol = 1E-2;

    int loopNum = 1;
    DataPlant::PlantLoop.allocate(1);
    DataPlant::PlantLoop(loopNum).FluidIndex = 1;
    DataPlant::PlantLoop(loopNum).FluidName = "WATER";

    // init fluid
    FluidWorker fluid;
    fluid.initialize(loopNum);

    GHEBorehole tst;
    tst.pipe.outerDia = 0.02670;
    tst.pipe.wallThickness = 0.00243;
    tst.pipe.k = 0.4;
    tst.pipe.rhoCp = 1.5853E+06;
    tst.diameter = 0.109982;
    tst.kGrout = 2.4;
    tst.rhoCpGrout = 3.9E+06;
    tst.shankSpace = 0.04556;
    tst.length = 100;
    tst.initialize(2.432, 2.432E+06);
    tst.pipe.fluid = fluid;

    Real64 mdot = 0.1;
    Real64 temperature = 20.0;
    tst.calcShortTimestepGFunctions(mdot, temperature);

    EXPECT_NEAR(tst.gSTS.back(), 2.28, tol);
    EXPECT_NEAR(tst.lnttsSTS.back(), -8.99, tol);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_long_timestep_g_functions)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,  !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  4,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,        !- Undisturbed Ground Temperature Model Name",
        "  2.423,                   !- Ground Thermal Conductivity {W/m-K}",
        "  2.343E+06,               !- Ground Thermal Heat Capacity {J/m3-K}",
        "  ,                        !- GHE:ResponseFactors EFT Object Name",
        "  ,                        !- GHE:ResponseFactors BWT Object Name",
        "  GHE-Array;               !- GHT:Vertical:Array Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,  !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100.0,                   !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1.77E+06,                !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,  !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:Vertical:Array,",
        "  GHE-Array,               !- Name",
        "  GHE Props,  !- GHE:Vertical:Properties Object Name",
        "  2,                       !- Number of Boreholes in X-Direction",
        "  2,                       !- Number of Boreholes in Y-Direction",
        "  5;                       !- Borehole Spacing {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    EnhancedGHE::factory(state, "GHE");

    ASSERT_EQ(1u, state.dataGroundHeatExchangerEnhanced.enhancedGHE.size());

    auto &tstGHE(state.dataGroundHeatExchangerEnhanced.enhancedGHE[0]);

    DataEnvironment::MaxNumberSimYears = 1;
    DataPlant::TotNumLoops = 1;
    DataPlant::PlantLoop.allocate(1);
    auto &loop(DataPlant::PlantLoop(1));
    loop.LoopSide.allocate(2);
    auto &loopside(DataPlant::PlantLoop(1).LoopSide(1));
    loopside.TotalBranches = 1;
    loopside.Branch.allocate(1);
    auto &loopsidebranch(DataPlant::PlantLoop(1).LoopSide(1).Branch(1));
    loopsidebranch.TotalComponents = 1;
    loopsidebranch.Comp.allocate(1);

    DataPlant::PlantLoop(1).Name = "ChilledWaterLoop";
    DataPlant::PlantLoop(1).FluidIndex = 1;
    DataPlant::PlantLoop(1).PlantSizNum = 1;
    DataPlant::PlantLoop(1).FluidName = "WATER";
    DataPlant::PlantLoop(1).LoopSide(1).Branch(1).Comp(1).Name = tstGHE.name;
    DataPlant::PlantLoop(1).LoopSide(1).Branch(1).Comp(1).TypeOf_Num = DataPlant::TypeOf_GrndHtExchgSystem;
    DataPlant::PlantLoop(1).LoopSide(1).Branch(1).Comp(1).NodeNumIn = tstGHE.inletNode;
    DataPlant::PlantLoop(1).LoopSide(1).Branch(1).Comp(1).NodeNumOut = tstGHE.outletNode;

    DataSizing::PlantSizData.allocate(1);
    DataSizing::PlantSizData(1).DesVolFlowRate = 0.0033;
    DataSizing::PlantSizData(1).DeltaT = 5.0;

    DataPlant::PlantFirstSizesOkayToFinalize = true;
    DataPlant::PlantFirstSizesOkayToReport = true;
    DataPlant::PlantFinalSizesOkayToReport = true;

    tstGHE.gFuncEFTExist = true;

    PlantLocation loc(0, 0, 0, 0);

    tstGHE.onInitLoopEquip(state, loc);

    Real64 const tol = 1E-2;

    EXPECT_NEAR(tstGHE.gLTS[0], 2.54, tol);
    EXPECT_NEAR(tstGHE.gLTS[1], 2.79, tol);
    EXPECT_NEAR(tstGHE.gLTS[2], 3.03, tol);
    EXPECT_NEAR(tstGHE.gLTS[3], 3.28, tol);
    EXPECT_NEAR(tstGHE.gLTS[4], 3.53, tol);
    EXPECT_NEAR(tstGHE.gLTS[5], 3.81, tol);
    EXPECT_NEAR(tstGHE.gLTS[6], 4.14, tol);
    EXPECT_NEAR(tstGHE.gLTS[7], 4.58, tol);
    EXPECT_NEAR(tstGHE.gLTS[8], 5.13, tol);
    EXPECT_NEAR(tstGHE.gLTS[9], 5.79, tol);
    EXPECT_NEAR(tstGHE.gLTS[10], 6.53, tol);
}

TEST_F(EnergyPlusFixture, GHE_Enhanced_merge_bwt_g_functions)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  GHE,  !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  4,                       !- Number of Boreholes",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  GHE Ground Temps,        !- Undisturbed Ground Temperature Model Name",
        "  2.423,                   !- Ground Thermal Conductivity {W/m-K}",
        "  2.343E+06,               !- Ground Thermal Heat Capacity {J/m3-K}",
        "  ,                        !- GHE:ResponseFactors EFT Object Name",
        "  ,                        !- GHE:ResponseFactors BWT Object Name",
        "  GHE-Array;               !- GHT:Vertical:Array Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  GHE Props,  !- Name",
        "  1,                       !- Depth of Top of Borehole {m}",
        "  100.0,                   !- Borehole Length {m}",
        "  0.109982,                !- Borehole Diameter {m}",
        "  0.744,                   !- Grout Thermal Conductivity {W/m-K}",
        "  3.90E+06,                !- Grout Thermal Heat Capacity {J/m3-K}",
        "  0.389,                   !- Pipe Thermal Conductivity {W/m-K}",
        "  1.77E+06,                !- Pipe Thermal Heat Capacity {J/m3-K}",
        "  0.0267,                  !- Pipe Outer Diameter {m}",
        "  0.00243,                 !- Pipe Thickness {m}",
        "  0.04556;                 !- U-Tube Distance {m}",
        "",
        "Site:GroundTemperature:Undisturbed:KusudaAchenbach,",
        "  GHE Ground Temps,  !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:Vertical:Array,",
        "  GHE-Array,               !- Name",
        "  GHE Props,  !- GHE:Vertical:Properties Object Name",
        "  2,                       !- Number of Boreholes in X-Direction",
        "  2,                       !- Number of Boreholes in Y-Direction",
        "  5;                       !- Borehole Spacing {m}",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    EnhancedGHE::factory(state, "GHE");

    ASSERT_EQ(1u, state.dataGroundHeatExchangerEnhanced.enhancedGHE.size());

    auto &tstGHE(state.dataGroundHeatExchangerEnhanced.enhancedGHE[0]);

    tstGHE.aveBH.gSTS = std::vector<Real64>{1, 2, 3, 4, 5};
    tstGHE.aveBH.lnttsSTS = std::vector<Real64>{1, 2, 3, 4, 5};
    tstGHE.gLTS = std::vector<Real64>{6, 7, 8, 9, 10};
    tstGHE.lnttsLTS = std::vector<Real64>{6, 7, 8, 9, 10};

    tstGHE.mergeLTSandSTSgFunctions();

    Real64 tol = 1E-3;

    EXPECT_NEAR(tstGHE.getBWTgFunc(-1.0), -1.0, tol);
    EXPECT_NEAR(tstGHE.getBWTgFunc(1.0), 1.0, tol);
    EXPECT_NEAR(tstGHE.getBWTgFunc(2.5), 2.5, tol);
    EXPECT_NEAR(tstGHE.getBWTgFunc(8.0), 8.0, tol);
    EXPECT_NEAR(tstGHE.getBWTgFunc(9.5), 9.5, tol);
    EXPECT_NEAR(tstGHE.getBWTgFunc(11.0), 11.0, tol);
}