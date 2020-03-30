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

// C++ Headers

// ObjexxFCL Headers

// JSON Headers

// EnergyPlus Headers
#include <EnergyPlus/BranchNodeConnections.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataIPShortCuts.hh>
#include <EnergyPlus/GroundHeatExchangerEnhanced.hh>
#include <EnergyPlus/InputProcessing/InputProcessor.hh>
#include <EnergyPlus/NodeInputManager.hh>
#include <EnergyPlus/OutputProcessor.hh>
#include <EnergyPlus/PlantUtilities.hh>
#include <EnergyPlus/UtilityRoutines.hh>

namespace EnergyPlus {

namespace GroundHeatExchangerEnhanced {

    bool getInput(true);

    std::vector<GroundHeatExchangerEnhanced::EnhancedGHE> enhancedGHE;

    void clear_state()
    {
        getInput = true;
        enhancedGHE.clear();
    }

    PlantComponent *EnhancedGHE::factory(std::string const &objectName)
    {
        if (getInput) {
            getGHEInput();
            getInput = false;
        }

        for (auto &ghe : enhancedGHE) {
            if (ghe.name == objectName) {
                return &ghe;
            }
        }

        // If we didn't find it, fatal
        ShowFatalError("Ground Heat Exchanger Factory: Error getting inputs for GHX named: " + objectName);
        // Shut up the compiler
        return nullptr;
    }

    void getGHEInput()
    {
        int const numGHE = inputProcessor->getNumObjectsFound("GroundHeatExchanger:System");
        int const numArray = inputProcessor->getNumObjectsFound("GroundHeatExchanger:Vertical:Array");
        int const numProps = inputProcessor->getNumObjectsFound("GroundHeatExchanger:Vertical:Properties");
        int const numResponseFactors = inputProcessor->getNumObjectsFound("GroundHeatExchanger:ResponseFactors");
        int const numSingleBHs = inputProcessor->getNumObjectsFound("GroundHeatExchanger:Vertical:Single");

        // TODO: cleanup error handling

        // temporary vectors
        std::vector<GHEProps> propsVect;
        std::vector<GHERespFactors> respFactorsVect;
        std::vector<GHEArray> arraysVect;
        std::vector<GHEBorehole> boreholesVect;

        DataIPShortCuts::cCurrentModuleObject = "GroundHeatExchanger:Vertical:Properties";

        for (int propNum = 1; propNum <= numProps; ++propNum) {

            // just a few vars to pass in and out to GetObjectItem
            int ioStatus;
            int numAlphas;
            int numNumbers;

            // get the input data and store it in the Shortcuts structures
            inputProcessor->getObjectItem(DataIPShortCuts::cCurrentModuleObject,
                                          propNum,
                                          DataIPShortCuts::cAlphaArgs,
                                          numAlphas,
                                          DataIPShortCuts::rNumericArgs,
                                          numNumbers,
                                          ioStatus,
                                          DataIPShortCuts::lNumericFieldBlanks,
                                          DataIPShortCuts::lAlphaFieldBlanks,
                                          DataIPShortCuts::cAlphaFieldNames,
                                          DataIPShortCuts::cNumericFieldNames);

            // the input processor validates the numeric inputs based on the IDD definition
            // still validate the name to make sure there aren't any duplicates or blanks
            // blanks are easy: fatal if blank
            if (DataIPShortCuts::lAlphaFieldBlanks(1)) {
                ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject + " object: Name cannot be blank");
            }

            // we just need to loop over the existing vector elements to check for duplicates since we haven't add this one yet
            for (auto &prop : propsVect) {
                if (DataIPShortCuts::cAlphaArgs(1) == prop.propName) {
                    ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject +
                                   " object: Duplicate name found: " + prop.propName);
                }
            }

            // create new instance
            GHEProps newProp;

            // populate values
            newProp.propName = DataIPShortCuts::cAlphaArgs(1);
            newProp.depth = DataIPShortCuts::rNumericArgs(1);
            newProp.length = DataIPShortCuts::rNumericArgs(2);
            newProp.diameter = DataIPShortCuts::rNumericArgs(3);
            newProp.kGrout = DataIPShortCuts::rNumericArgs(4);
            newProp.rhoCpGrout = DataIPShortCuts::rNumericArgs(5);

            // construct pipe instance
            newProp.pipe = Pipe(DataIPShortCuts::rNumericArgs(6),
                                DataIPShortCuts::rNumericArgs(7),
                                DataIPShortCuts::rNumericArgs(8),
                                DataIPShortCuts::rNumericArgs(9));

            newProp.shankSpace = DataIPShortCuts::rNumericArgs(10);

            // check for invalid shank spacing
            if (newProp.shankSpace < newProp.pipe.outerDia) {
                ShowWarningError("Borehole shank spacing is invalid.");
                ShowWarningError("U-tube spacing is reference from the u-tube pipe center.");
                ShowWarningError("Shank spacing is set to minimum possible value.");
                newProp.shankSpace = newProp.pipe.outerDia;
            } else if (newProp.shankSpace > (newProp.diameter - newProp.pipe.outerDia)) {
                ShowWarningError("Borehole shank spacing is invalid.");
                ShowWarningError("U-tube spacing is reference from the u-tube pipe center.");
                ShowWarningError("Shank spacing is set to the maximum possible value.");
            }

            // save instance
            propsVect.push_back(newProp);
        }

        DataIPShortCuts::cCurrentModuleObject = "GroundHeatExchanger:ResponseFactors";

        for (int rfNum = 1; rfNum <= numResponseFactors; ++rfNum) {

            // just a few vars to pass in and out to GetObjectItem
            int ioStatus;
            int numAlphas;
            int numNumbers;
            bool errorsFound = false;

            // get the input data and store it in the Shortcuts structures
            inputProcessor->getObjectItem(DataIPShortCuts::cCurrentModuleObject,
                                          rfNum,
                                          DataIPShortCuts::cAlphaArgs,
                                          numAlphas,
                                          DataIPShortCuts::rNumericArgs,
                                          numNumbers,
                                          ioStatus,
                                          DataIPShortCuts::lNumericFieldBlanks,
                                          DataIPShortCuts::lAlphaFieldBlanks,
                                          DataIPShortCuts::cAlphaFieldNames,
                                          DataIPShortCuts::cNumericFieldNames);

            // the input processor validates the numeric inputs based on the IDD definition
            // still validate the name to make sure there aren't any duplicates or blanks
            // blanks are easy: fatal if blank
            if (DataIPShortCuts::lAlphaFieldBlanks(1)) {
                ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject + " object: Name cannot be blank");
            }

            // we just need to loop over the existing vector elements to check for duplicates since we haven't added this one yet
            for (auto &rf : respFactorsVect) {
                if (DataIPShortCuts::cAlphaArgs(1) == rf.name) {
                    ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject +
                                   " object: Duplicate name found: " + rf.name);
                }
            }

            // Build out new instance and add it to the vector
            GHERespFactors newRF;

            newRF.name = DataIPShortCuts::cAlphaArgs(1);

            for (auto &prop : propsVect) {
                if (prop.propName == DataIPShortCuts::cAlphaArgs(2)) {
                    newRF.props = prop;
                }
            }

            newRF.numBH = DataIPShortCuts::rNumericArgs(1);
            newRF.maxSimYears = DataEnvironment::MaxNumberSimYears;

            // get number of g-function pairs
            int numPreviousNumFields = 1;
            int numPairs = 0;
            if ((numNumbers - numPreviousNumFields) % 2 == 0) {
                numPairs = (numNumbers - numPreviousNumFields) / 2;
            } else {
                errorsFound = true;
                ShowSevereError("Errors found processing response factor input for Response Factor= " + newRF.name);
                ShowSevereError("Uneven number of g-function pairs");
            }

            if (errorsFound) {
                ShowFatalError("Errors occurred during input processing.");
            }


            // load g-function-LnTTs pairs into temporary arrays
            std::vector<std::vector<Real64>> LnTTs = {{}};
            std::vector<std::vector<Real64>> gFn = {{}};

            int indexNum = 2;
            for (int pairNum = 1; pairNum <= numPairs; ++pairNum) {
                LnTTs.front().push_back(DataIPShortCuts::rNumericArgs(indexNum));
                gFn.front().push_back(DataIPShortCuts::rNumericArgs(indexNum + 1));
                indexNum += 2;
            }

            // setup table/interpolator object
            Btwxt::GriddedData gFnGridData(LnTTs, gFn);
            gFnGridData.set_axis_interp_method(0, Btwxt::Method::LINEAR);
            gFnGridData.set_axis_extrap_method(0, Btwxt::Method::LINEAR);
            newRF.g = Btwxt::RegularGridInterpolator(gFnGridData);

            // save instance
            respFactorsVect.push_back(newRF);
        }

        DataIPShortCuts::cCurrentModuleObject = "GroundHeatExchanger:Vertical:Array";

        for (int arrayNum = 1; arrayNum <= numArray; ++arrayNum) {

            // just a few vars to pass in and out to GetObjectItem
            int ioStatus;
            int numAlphas;
            int numNumbers;

            // get the input data and store it in the Shortcuts structures
            inputProcessor->getObjectItem(DataIPShortCuts::cCurrentModuleObject,
                                          arrayNum,
                                          DataIPShortCuts::cAlphaArgs,
                                          numAlphas,
                                          DataIPShortCuts::rNumericArgs,
                                          numNumbers,
                                          ioStatus,
                                          DataIPShortCuts::lNumericFieldBlanks,
                                          DataIPShortCuts::lAlphaFieldBlanks,
                                          DataIPShortCuts::cAlphaFieldNames,
                                          DataIPShortCuts::cNumericFieldNames);

            // the input processor validates the numeric inputs based on the IDD definition
            // still validate the name to make sure there aren't any duplicates or blanks
            // blanks are easy: fatal if blank
            if (DataIPShortCuts::lAlphaFieldBlanks(1)) {
                ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject + " object: Name cannot be blank");
            }

            // we just need to loop over the existing vector elements to check for duplicates since we haven't add this one yet
            for (auto &array : arraysVect) {
                if (DataIPShortCuts::cAlphaArgs(1) == array.name) {
                    ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject +
                                   " object: Duplicate name found: " + array.name);
                }
            }

            // build out new instance
            GHEArray newArray;
            newArray.name = DataIPShortCuts::cAlphaArgs(1);

            // find correct properties instance
            GHEProps props;
            for (auto &thisProp : propsVect) {
                if (thisProp.propName == DataIPShortCuts::cAlphaArgs(2)) {
                    props = thisProp;
                    break;
                }
            }

            // build out boreholes
            int numBHinXDirection = DataIPShortCuts::rNumericArgs(1);
            int numBHinYDirection = DataIPShortCuts::rNumericArgs(2);
            Real64 bhSpacing = DataIPShortCuts::rNumericArgs(3);

            for (int i = 0; i < numBHinXDirection; ++i) {
                for (int j = 0; j < numBHinYDirection; ++j) {
                    // new instance
                    GHEBorehole bh;

                    // populate values
                    bh.xLoc = i * bhSpacing;
                    bh.yLoc = j * bhSpacing;
                    bh.propName = props.propName;
                    bh.depth = props.depth;
                    bh.length = props.length;
                    bh.diameter = props.diameter;
                    bh.kGrout = props.kGrout;
                    bh.rhoCpGrout = props.rhoCpGrout;
                    bh.shankSpace = props.shankSpace;
                    bh.pipe = props.pipe;

                    // save bh instance
                    newArray.boreholes.push_back(bh);
                }
            }

            // save array instance
            arraysVect.push_back(newArray);
        }

        DataIPShortCuts::cCurrentModuleObject = "GroundHeatExchanger:Vertical:Single";

        for (int bhNum = 1; bhNum <= numSingleBHs; ++bhNum) {

            // just a few vars to pass in and out to GetObjectItem
            int ioStatus;
            int numAlphas;
            int numNumbers;

            // get the input data and store it in the Shortcuts structures
            inputProcessor->getObjectItem(DataIPShortCuts::cCurrentModuleObject,
                                          bhNum,
                                          DataIPShortCuts::cAlphaArgs,
                                          numAlphas,
                                          DataIPShortCuts::rNumericArgs,
                                          numNumbers,
                                          ioStatus,
                                          DataIPShortCuts::lNumericFieldBlanks,
                                          DataIPShortCuts::lAlphaFieldBlanks,
                                          DataIPShortCuts::cAlphaFieldNames,
                                          DataIPShortCuts::cNumericFieldNames);

            // the input processor validates the numeric inputs based on the IDD definition
            // still validate the name to make sure there aren't any duplicates or blanks
            // blanks are easy: fatal if blank
            if (DataIPShortCuts::lAlphaFieldBlanks(1)) {
                ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject + " object: Name cannot be blank");
            }

            // we just need to loop over the existing vector elements to check for duplicates since we haven't add this one yet
            for (auto &bh : boreholesVect) {
                if (DataIPShortCuts::cAlphaArgs(1) == bh.name) {
                    ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject +
                                   " object: Duplicate name found: " + bh.name);
                }
            }

            // create new instance
            GHEBorehole newBH;

            newBH.name = DataIPShortCuts::cAlphaArgs(1);

            // get right props object
            GHEProps props;
            for (const auto& thisProp : propsVect) {
                if (thisProp.propName == DataIPShortCuts::cAlphaArgs(2)) {
                    props = thisProp;
                }
            }

            // populate values
            newBH.xLoc = DataIPShortCuts::rNumericArgs(1);
            newBH.yLoc = DataIPShortCuts::rNumericArgs(2);
            newBH.propName = props.propName;
            newBH.depth = props.depth;
            newBH.length = props.length;
            newBH.diameter = props.diameter;
            newBH.kGrout = props.kGrout;
            newBH.rhoCpGrout = props.rhoCpGrout;
            newBH.shankSpace = props.shankSpace;
            newBH.pipe = props.pipe;

            // save instance
            boreholesVect.push_back(newBH);
        }

        DataIPShortCuts::cCurrentModuleObject = "GroundHeatExchanger:System";

        for (int GHENum = 1; GHENum <= numGHE; ++GHENum) {
            // just a few vars to pass in and out to GetObjectItem
            int ioStatus;
            int numAlphas;
            int numNumbers;
            bool errorsFound = false;

            // get the input data and store it in the Shortcuts structures
            inputProcessor->getObjectItem(DataIPShortCuts::cCurrentModuleObject,
                                          GHENum,
                                          DataIPShortCuts::cAlphaArgs,
                                          numAlphas,
                                          DataIPShortCuts::rNumericArgs,
                                          numNumbers,
                                          ioStatus,
                                          DataIPShortCuts::lNumericFieldBlanks,
                                          DataIPShortCuts::lAlphaFieldBlanks,
                                          DataIPShortCuts::cAlphaFieldNames,
                                          DataIPShortCuts::cNumericFieldNames);

            // the input processor validates the numeric inputs based on the IDD definition
            // still validate the name to make sure there aren't any duplicates or blanks
            // blanks are easy: fatal if blank
            if (DataIPShortCuts::lAlphaFieldBlanks(1)) {
                ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject + " object: Name cannot be blank");
            }

            // we just need to loop over the existing vector elements to check for duplicates since we haven't add this one yet
            for (auto &ghe : enhancedGHE) {
                if (DataIPShortCuts::cAlphaArgs(1) == ghe.name) {
                    ShowFatalError("Invalid input for " + DataIPShortCuts::cCurrentModuleObject +
                                   " object: Duplicate name found: " + ghe.name);
                }
            }

            // Build out new instance
            EnhancedGHE thisGHE;

            // Object name
            thisGHE.name = DataIPShortCuts::cAlphaArgs(1);

            // Inlet node
            thisGHE.inletNode = NodeInputManager::GetOnlySingleNode(DataIPShortCuts::cAlphaArgs(2),
                                                                       errorsFound,
                                                                       DataIPShortCuts::cCurrentModuleObject,
                                                                       DataIPShortCuts::cAlphaArgs(1),
                                                                       DataLoopNode::NodeType_Water,
                                                                       DataLoopNode::NodeConnectionType_Inlet,
                                                                       1,
                                                                       DataLoopNode::ObjectIsNotParent);

            // Outlet node
            thisGHE.outletNode = NodeInputManager::GetOnlySingleNode(DataIPShortCuts::cAlphaArgs(3),
                                                                        errorsFound,
                                                                        DataIPShortCuts::cCurrentModuleObject,
                                                                        DataIPShortCuts::cAlphaArgs(1),
                                                                        DataLoopNode::NodeType_Water,
                                                                        DataLoopNode::NodeConnectionType_Outlet,
                                                                        1,
                                                                        DataLoopNode::ObjectIsNotParent);

            BranchNodeConnections::TestCompSet(DataIPShortCuts::cCurrentModuleObject,
                                               DataIPShortCuts::cAlphaArgs(1),
                                               DataIPShortCuts::cAlphaArgs(2),
                                               DataIPShortCuts::cAlphaArgs(3),
                                               "Condenser Water Nodes");

            // Design flow rate
            thisGHE.designVolFlow = DataIPShortCuts::rNumericArgs(1);
            PlantUtilities::RegisterPlantCompDesignFlow(thisGHE.inletNode, thisGHE.designVolFlow);

            // Soil
            thisGHE.kSoil = DataIPShortCuts::rNumericArgs(2);
            thisGHE.rhoCpSoil = DataIPShortCuts::rNumericArgs(3);

            // gb values - Exiting fluid temperature response factors
            if (!DataIPShortCuts::lAlphaFieldBlanks(6)) {
                // Response factors come from IDF object
                for (auto &thisRF : respFactorsVect) {
                    if (UtilityRoutines::SameString(thisRF.name, DataIPShortCuts::cAlphaArgs(6))) {
                        thisGHE.gFuncEFT = thisRF;
                        thisGHE.gFuncEFTExist = true;
                    }
                }

                if (!thisGHE.gFuncEFTExist) {
                    ShowSevereError("GroundHeatExchanger:ResponseFactors object name \"" + DataIPShortCuts::cAlphaArgs(6) + "\" not found");
                }
            }

            // g values - Borehole wall temperature response factors
            if (!DataIPShortCuts::lAlphaFieldBlanks(7)) {
                // Response factors come from IDF object
                for (auto &thisRF : respFactorsVect) {
                    if (UtilityRoutines::SameString(thisRF.name, DataIPShortCuts::cAlphaArgs(7))) {
                        thisGHE.gFuncBWT = thisRF;
                        thisGHE.gFuncBWTExist = true;
                    }
                }

                if (!thisGHE.gFuncBWTExist) {
                    ShowSevereError("GroundHeatExchanger:ResponseFactors object name \"" + DataIPShortCuts::cAlphaArgs(7) + "\" not found");
                }
            }

            // either g or gb were not found, so need to generate g-functions
            if (!thisGHE.gFuncBWTExist | !thisGHE.gFuncEFTExist) {
                if (!DataIPShortCuts::lAlphaFieldBlanks(8)) {
                    // borehole instances from array object
                    for (auto &arr : arraysVect) {
                        if (arr.name == DataIPShortCuts::cAlphaArgs(8)) {
                            thisGHE.boreholes = arr.boreholes;
                        }
                    }
                } else {
                    // borehole instances from single borehole objects

                    // get single boreholes
                    int numPreviousAlphaFields = 7;
                    int numBH = numAlphas - numPreviousAlphaFields;

                    int index = 8;
                    for (int bhNum = 1; bhNum < numBH; ++bhNum) {
                        bool bhFound = false;
                        for (auto it = boreholesVect.begin(); it < boreholesVect.end(); it++) {
                            auto &bh = *it;
                            if (bh.name == DataIPShortCuts::cAlphaArgs(index)) {
                                // save this instance
                                thisGHE.boreholes.push_back(bh);
                                bhFound = true;

                                // delete to prevent later reuse
                                boreholesVect.erase(it);
                            }
                        }

                        if (!bhFound) {
                            ShowSevereError("Borehole \"" + DataIPShortCuts::cAlphaArgs(index) + "\" not found");
                        }

                        ++index;
                    }
                }
            }

            if (thisGHE.boreholes.empty()) {
                ShowFatalError("There were problems getting borholes");
            }

            // Initialize ground temperature model and get pointer reference
            // Do this last because it calls getObjectItem
            thisGHE.gtm = GroundTemperatureManager::GetGroundTempModelAndInit(DataIPShortCuts::cAlphaArgs(4), DataIPShortCuts::cAlphaArgs(5));

            // Save object
            enhancedGHE.push_back(thisGHE);
        }

    }

    void EnhancedGHE::setupOutputVars()
    {
        SetupOutputVariable("Ground Heat Exchanger Average Borehole Temperature", OutputProcessor::Unit::C, this->aveBHWallTemp, "System", "Average", this->name);
        SetupOutputVariable("Ground Heat Exchanger Heat Transfer Rate", OutputProcessor::Unit::W, this->heatRateToSoil, "System", "Average", this->name);
        SetupOutputVariable("Ground Heat Exchanger Inlet Temperature", OutputProcessor::Unit::C, this->inletTemp, "System", "Average", this->name);
        SetupOutputVariable("Ground Heat Exchanger Outlet Temperature", OutputProcessor::Unit::C, this->outletTemp, "System", "Average", this->name);
        SetupOutputVariable("Ground Heat Exchanger Mass Flow Rate", OutputProcessor::Unit::kg_s, this->massFlowRate, "System", "Average", this->name);
        SetupOutputVariable("Ground Heat Exchanger Average Fluid Temperature", OutputProcessor::Unit::C, this->aveFluidTemp, "System", "Average", this->name);
        SetupOutputVariable("Ground Heat Exchanger Farfield Ground Temperature",OutputProcessor::Unit::C, this->farFieldGroundTemp,"System","Average", this->name);
    }

    void EnhancedGHE::simulate(const PlantLocation &EP_UNUSED(calledFromLocation), bool EP_UNUSED(FirstHVACIteration), Real64 &EP_UNUSED(CurLoad), bool EP_UNUSED(RunFlag))
    {
    }

    void EnhancedGHE::getDesignCapacities(const PlantLocation &EP_UNUSED(calledFromLocation), Real64 &EP_UNUSED(MaxLoad), Real64 &EP_UNUSED(MinLoad), Real64 &EP_UNUSED(OptLoad))
    {
    }

    void EnhancedGHE::getDesignTemperatures(Real64 &EP_UNUSED(TempDesCondIn), Real64 &EP_UNUSED(TempDesEvapOut))
    {
    }

    void EnhancedGHE::getSizingFactor(Real64 &EP_UNUSED(sizFac))
    {
    }

    void EnhancedGHE::onInitLoopEquip(const PlantLocation &EP_UNUSED(calledFromLocation))
    {
        if (this->oneTimeInit) {
            this->setupOutputVars();
        }
    }

} // namespace GroundHeatExchangerEnhanced

} // namespace EnergyPlus
