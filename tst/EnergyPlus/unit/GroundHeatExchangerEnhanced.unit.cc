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
#include <EnergyPlus/GroundHeatExchangerEnhanced.hh>
#include <EnergyPlus/GroundTemperatureModeling/GroundTemperatureModelManager.hh>

// Testing Headers
#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;
using namespace EnergyPlus::GroundHeatExchangerEnhanced;

TEST_F(EnergyPlusFixture, GHE_Enhanced_Good_Init_With_g_functions)
{
    std::string const idf_objects = delimited_string({
        "GroundHeatExchanger:System,",
        "  Vertical Ground Heat Exchanger,  !- Name",
        "  GHE Inlet Node,          !- Inlet Node Name",
        "  GHE Outlet Node,         !- Outlet Node Name",
        "  0.00330000,              !- Design Flow Rate {m3/s}",
        "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
        "  Vertical Ground Heat Exchanger Ground Temps,  !- Undisturbed Ground Temperature Model Name",
        "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
        "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
        "  Exiting Fluid Temperature g-functions,  !- GHE:ResponseFactors Exiting Fluid Temperature Object Name",
        "  Borehole Wall Temperature g-functions;  !- GHE:ResponseFactors Borehole Wall Temperature Object Name",
        "",
        "GroundHeatExchanger:Vertical:Properties,",
        "  Vertical Ground Heat Exchanger Props,  !- Name",
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
        "  Vertical Ground Heat Exchanger Ground Temps,  !- Name",
        "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
        "  920,                     !- Soil Density {kg/m3}",
        "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
        "  13.375,                  !- Average Soil Surface Temperature {C}",
        "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
        "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
        "",
        "GroundHeatExchanger:ResponseFactors,",
        "  Exiting Fluid Temperature g-functions,  !- Name",
        "  Vertical Ground Heat Exchanger Props,  !- GHE:Vertical:Properties Object Name",
        "  10,                      !- Number of Boreholes",
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
        "  Borehole Wall Temperature g-functions,  !- Name",
        "  Vertical Ground Heat Exchanger Props,  !- GHE:Vertical:Properties Object Name",
        "  10,                      !- Number of Boreholes",
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

    EnhancedGHE::factory("VERTICAL GROUND HEAT EXCHANGER");

    ASSERT_EQ(1u, enhancedGHE.size());

    EnhancedGHE *tstGHE = &enhancedGHE[0];

    EXPECT_EQ("VERTICAL GROUND HEAT EXCHANGER", tstGHE->name);
    EXPECT_EQ(0.00330000, tstGHE->designVolFlow);
    EXPECT_EQ(GroundTemperatureManager::objectType_KusudaGroundTemp, tstGHE->gtm->objectType);
    EXPECT_EQ(0.692626E+00, tstGHE->kSoil);
    EXPECT_EQ(0.234700E+07, tstGHE->rhoCpSoil);

    EXPECT_EQ("EXITING FLUID TEMPERATURE G-FUNCTIONS", tstGHE->gFuncEFT.name);
    EXPECT_EQ("BOREHOLE WALL TEMPERATURE G-FUNCTIONS", tstGHE->gFuncBWT.name);

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
    EXPECT_EQ("VERTICAL GROUND HEAT EXCHANGER PROPS", eft->props.propName);
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
    EXPECT_EQ("VERTICAL GROUND HEAT EXCHANGER PROPS", bwt->props.propName);
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
         "  Vertical Ground Heat Exchanger,  !- Name",
         "  GHE Inlet Node,          !- Inlet Node Name",
         "  GHE Outlet Node,         !- Outlet Node Name",
         "  0.00330000,              !- Design Flow Rate {m3/s}",
         "  Site:GroundTemperature:Undisturbed:KusudaAchenbach,  !- Undisturbed Ground Temperature Model Type",
         "  Vertical Ground Heat Exchanger Ground Temps,  !- Undisturbed Ground Temperature Model Name",
         "  0.692626E+00,            !- Ground Thermal Conductivity {W/m-K}",
         "  0.234700E+07,            !- Ground Thermal Heat Capacity {J/m3-K}",
         "  ,                        !- GHE:ResponseFactors Exiting Fluid Temperature Object Name",
         "  ,                        !- GHE:ResponseFactors Borehole Wall Temperature Object Name",
         "  GHE-Array;               !- GroundHeatExchanger:Vertical:Array Object Name",
         "",
         "GroundHeatExchanger:Vertical:Properties,",
         "  Vertical Ground Heat Exchanger Props,  !- Name",
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
         "  Vertical Ground Heat Exchanger Ground Temps,  !- Name",
         "  0.692626E+00,            !- Soil Thermal Conductivity {W/m-K}",
         "  920,                     !- Soil Density {kg/m3}",
         "  2551.09,                 !- Soil Specific Heat {J/kg-K}",
         "  13.375,                  !- Average Soil Surface Temperature {C}",
         "  3.2,                     !- Average Amplitude of Surface Temperature {deltaC}",
         "  8;                       !- Phase Shift of Minimum Surface Temperature {days}",
         "",
         "GroundHeatExchanger:Vertical:Array,",
         "  GHE-Array,               !- Name",
         "  Vertical Ground Heat Exchanger Props,  !- GHE:Vertical:Properties Object Name",
         "  2,                       !- Number of Boreholes in X-Direction",
         "  2,                       !- Number of Boreholes in Y-Direction",
         "  5;                       !- Borehole Spacing {m}",
     });

    ASSERT_TRUE(process_idf(idf_objects));

    EnhancedGHE::factory("VERTICAL GROUND HEAT EXCHANGER");

    ASSERT_EQ(1u, enhancedGHE.size());

    EnhancedGHE *tstGHE = &enhancedGHE[0];

    EXPECT_EQ("VERTICAL GROUND HEAT EXCHANGER", tstGHE->name);
    EXPECT_EQ(0.00330000, tstGHE->designVolFlow);
    EXPECT_EQ(GroundTemperatureManager::objectType_KusudaGroundTemp, tstGHE->gtm->objectType);
    EXPECT_EQ(0.692626E+00, tstGHE->kSoil);
    EXPECT_EQ(0.234700E+07, tstGHE->rhoCpSoil);

    EXPECT_EQ(4u, tstGHE->boreholes.size());

    EXPECT_EQ(false, tstGHE->gFuncEFTExist);
    EXPECT_EQ(false, tstGHE->gFuncBWTExist);

    GHEBorehole *bh0 = &tstGHE->boreholes[0];
    EXPECT_EQ(0.0, bh0->xLoc);
    EXPECT_EQ(0.0, bh0->yLoc);
    EXPECT_EQ("VERTICAL GROUND HEAT EXCHANGER PROPS", bh0->propName);
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
    EXPECT_EQ("VERTICAL GROUND HEAT EXCHANGER PROPS", bh1->propName);
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
    EXPECT_EQ("VERTICAL GROUND HEAT EXCHANGER PROPS", bh2->propName);
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
    EXPECT_EQ("VERTICAL GROUND HEAT EXCHANGER PROPS", bh3->propName);
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
