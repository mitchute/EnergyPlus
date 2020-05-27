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

#include <EnergyPlus/api/TypeDefs.h>
#include <EnergyPlus/AirLoopHVACDOAS.hh>
#include <EnergyPlus/DataAirLoop.hh>
#include <EnergyPlus/DataSizing.hh>

namespace EnergyPlus {

    enum class AutoSizingType {
        AutoCalculate,
        HeatingAirflowUASizing,
        Unknown
    };

    enum class AutoSizingResultType {
        NoError,
        ErrorType1
    };

    struct CommonFlags {
        bool sysSizingRunDone = false;
        bool zoneSizingRunDone = false;
        int curSysNum = 0;
        int curOASysNum = 0;
        int curZoneEqNum = 0;
        int curDuctType = 0;
        int numPrimaryAirSys = 0;
        int numSysSizInput = 0;
        bool doSystemSizing = false;
        int numZoneSizingInput = 0;
        bool doZoneSizing = false;
        std::string compType = "";
        std::string compName = "";
        bool printWarningFlag = false;
    };

    struct BaseSizer {
        AutoSizingType sizingType = AutoSizingType::Unknown;
        Real64 originalValue = 0.0;
        Real64 autoSizedValue = 0.0;
        bool wasAutoSized = false;
        CommonFlags baseFlags;
        bool hardSizeNoDesignRun = false;
        bool sizingDesRunThisAirSys = false;
        bool sizingDesRunThisZone = false;
        bool sizingDesValueFromParent = false;
        bool airLoopSysFlag = false;
        bool oaSysFlag = false;

        Array1D<DataSizing::SystemSizingInputData> sysSizingInputData;
        Array1D<DataSizing::ZoneSizingInputData> zoneSizingInput;
        Array1D<DataSizing::ZoneEqSizingData> unitarySysEqSizing;
        Array1D<DataSizing::ZoneEqSizingData> oaSysEqSizing;
        Array1D<DataSizing::ZoneEqSizingData> zoneEqSizing;
        Array1D<DataAirLoop::OutsideAirSysProps> outsideAirSys;
        std::vector<AirLoopHVACDOAS::AirLoopDOAS> airloopDOAS;

        void preSize(CommonFlags & flags, Real64 originalValue);

        virtual AutoSizingResultType size(Real64 originalValue) = 0;
    };

}
