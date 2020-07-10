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

// EnergyPlus::ZoneEquipmentManager Unit Tests

// Google Test Headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include <EnergyPlus/FluidProperties.hh>

#include <cmath>
#include <ctgmath>
#include <fstream>

#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;
using namespace ObjexxFCL;
using namespace EnergyPlus::FluidProperties;

std::ofstream static pgFile("PG.csv", std::ofstream::out);
std::ofstream static egFile("EG.csv", std::ofstream::out);
std::ofstream static waterFile("Water.csv", std::ofstream::out);

TEST_F(EnergyPlusFixture, FluidProperties_GetDensityGlycol)
{

    std::string const idf_objects = delimited_string({"FluidProperties:GlycolConcentration,", "  GLHXFluid,       !- Name",
                                                      "  PropyleneGlycol, !- Glycol Type", "  ,                !- User Defined Glycol Name",
                                                      "  0.3;             !- Glycol Concentration", " "});

    ASSERT_TRUE(process_idf(idf_objects));
    EXPECT_FALSE(has_err_output());

    int FluidIndex = 0;

    EXPECT_NEAR(1037.89, GetDensityGlycol("GLHXFLUID", -35.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(1037.89, GetDensityGlycol("GLHXFLUID", -15.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(1034.46, GetDensityGlycol("GLHXFLUID", 5.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(1030.51, GetDensityGlycol("GLHXFLUID", 15.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(1026.06, GetDensityGlycol("GLHXFLUID", 25.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(1021.09, GetDensityGlycol("GLHXFLUID", 35.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(1015.62, GetDensityGlycol("GLHXFLUID", 45.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(1003.13, GetDensityGlycol("GLHXFLUID", 65.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(988.60, GetDensityGlycol("GLHXFLUID", 85.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(972.03, GetDensityGlycol("GLHXFLUID", 105.0, FluidIndex, "UnitTest"), 0.01);
    EXPECT_NEAR(953.41, GetDensityGlycol("GLHXFLUID", 125.0, FluidIndex, "UnitTest"), 0.01);
}

TEST_F(EnergyPlusFixture, FluidProperties_GetEnthalpyGlycol)
{

    std::string const idf_objects = delimited_string(
        {
            "FluidProperties:GlycolConcentration,",
            "  PG10,            !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.1;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  PG20,            !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.2;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  PG30,            !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.3;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  PG40,            !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.4;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  PG50,            !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.5;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  PG60,            !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.6;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  PG70,            !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.7;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  PG80,            !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.8;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  PG90,            !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.9;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  EG10,            !- Name",
            "  EthyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.1;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  EG20,            !- Name",
            "  EthyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.2;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  EG30,            !- Name",
            "  EthyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.3;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  EG40,            !- Name",
            "  EthyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.4;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  EG50,            !- Name",
            "  EthyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.5;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  EG60,            !- Name",
            "  EthyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.6;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  EG70,            !- Name",
            "  EthyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.7;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  EG80,            !- Name",
            "  EthyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.8;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  EG90,            !- Name",
            "  EthyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.9;             !- Glycol Concentration",
            "",
            "FluidProperties:GlycolConcentration,",
            "  Water,           !- Name",
            "  PropyleneGlycol, !- Glycol Type",
            "  ,                !- User Defined Glycol Name",
            "  0.0;             !- Glycol Concentration",
            "",
        });

    ASSERT_TRUE(process_idf(idf_objects));
    EXPECT_FALSE(has_err_output());

    pgFile << ",0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9\n";
    egFile << ",0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9\n";
    waterFile << ",Water\n";

    std::vector<Real64> temps {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    // PG
    int pg10Idx = -1;
    int pg20Idx = -1;
    int pg30Idx = -1;
    int pg40Idx = -1;
    int pg50Idx = -1;
    int pg60Idx = -1;
    int pg70Idx = -1;
    int pg80Idx = -1;
    int pg90Idx = -1;

    for (auto &t : temps) {
        pgFile << t << ",";
        pgFile << GetSpecificHeatGlycol("PG10", t, pg10Idx, "UnitTest")  << ",";
        pgFile << GetSpecificHeatGlycol("PG20", t, pg20Idx, "UnitTest")  << ",";
        pgFile << GetSpecificHeatGlycol("PG30", t, pg30Idx, "UnitTest")  << ",";
        pgFile << GetSpecificHeatGlycol("PG40", t, pg40Idx, "UnitTest")  << ",";
        pgFile << GetSpecificHeatGlycol("PG50", t, pg50Idx, "UnitTest")  << ",";
        pgFile << GetSpecificHeatGlycol("PG60", t, pg60Idx, "UnitTest")  << ",";
        pgFile << GetSpecificHeatGlycol("PG70", t, pg70Idx, "UnitTest")  << ",";
        pgFile << GetSpecificHeatGlycol("PG80", t, pg80Idx, "UnitTest")  << ",";
        pgFile << GetSpecificHeatGlycol("PG90", t, pg90Idx, "UnitTest")  << "\n";
    }

    // EG
    int eg10Idx = -1;
    int eg20Idx = -1;
    int eg30Idx = -1;
    int eg40Idx = -1;
    int eg50Idx = -1;
    int eg60Idx = -1;
    int eg70Idx = -1;
    int eg80Idx = -1;
    int eg90Idx = -1;

    for (auto &t : temps) {
        egFile << t << ",";
        egFile << GetSpecificHeatGlycol("EG10", t, eg10Idx, "UnitTest")  << ",";
        egFile << GetSpecificHeatGlycol("EG20", t, eg20Idx, "UnitTest")  << ",";
        egFile << GetSpecificHeatGlycol("EG30", t, eg30Idx, "UnitTest")  << ",";
        egFile << GetSpecificHeatGlycol("EG40", t, eg40Idx, "UnitTest")  << ",";
        egFile << GetSpecificHeatGlycol("EG50", t, eg50Idx, "UnitTest")  << ",";
        egFile << GetSpecificHeatGlycol("EG60", t, eg60Idx, "UnitTest")  << ",";
        egFile << GetSpecificHeatGlycol("EG70", t, eg70Idx, "UnitTest")  << ",";
        egFile << GetSpecificHeatGlycol("EG80", t, eg80Idx, "UnitTest")  << ",";
        egFile << GetSpecificHeatGlycol("EG90", t, eg90Idx, "UnitTest")  << "\n";
    }

    // Water
    int idx = -1;
    for (auto &t : temps) {
        waterFile << t << "," << GetSpecificHeatGlycol("WATER", t, idx, "UnitTest") << "\n";
    }
}
