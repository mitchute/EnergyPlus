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
#include <EnergyPlus/Plant/DataPlant.hh>
#include <EnergyPlus/FluidProperties.hh>
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
            if (UtilityRoutines::SameString(ghe.name, objectName)) {
                return &ghe;
            }
        }

        ShowFatalError("Ground Heat Exchanger Factory: Error getting inputs for GHX named: " + objectName);
        return nullptr; // LCOV_EXCL_LINE
    }

    void getGHEInput()
    {
        // module object names
        std::string const propsModObjName = "GroundHeatExchanger:Vertical:Properties";
        std::string const respFactModObjName = "GroundHeatExchanger:ResponseFactors";
        std::string const arrModObjName = "GroundHeatExchanger:Vertical:Array";
        std::string const bhModObjName = "GroundHeatExchanger:Vertical:Single";

        // num objects
        int const numProps = inputProcessor->getNumObjectsFound("GroundHeatExchanger:Vertical:Properties");
        int const numResponseFactors = inputProcessor->getNumObjectsFound("GroundHeatExchanger:ResponseFactors");
        int const numArray = inputProcessor->getNumObjectsFound("GroundHeatExchanger:Vertical:Array");
        int const numSingleBHs = inputProcessor->getNumObjectsFound("GroundHeatExchanger:Vertical:Single");
        int const numGHE = inputProcessor->getNumObjectsFound("GroundHeatExchanger:System");

        // temporary vectors
        std::vector<BoreholeProps> propsVect;
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

            // create new instance
            BoreholeProps newProp;

            // populate values
            newProp.propName = UtilityRoutines::MakeUPPERCase(DataIPShortCuts::cAlphaArgs(1));
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
            } else if (newProp.shankSpace > (newProp.diameter - 2.0 * newProp.pipe.outerDia)) {
                ShowWarningError("Borehole shank spacing is invalid.");
                ShowWarningError("U-tube spacing is reference from the u-tube pipe center.");
                ShowWarningError("Shank spacing is set to the maximum possible value.");
                newProp.shankSpace = newProp.diameter - 2.0 * newProp.pipe.outerDia;
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

            // Build out new instance
            GHERespFactors newRF;
            newRF.name = UtilityRoutines::MakeUPPERCase(DataIPShortCuts::cAlphaArgs(1));

            // get props instance
            bool propsFound = false;
            for (auto &prop : propsVect) {
                if (UtilityRoutines::SameString(DataIPShortCuts::cAlphaArgs(2), prop.propName)) {
                    newRF.props = prop;
                    propsFound = true;
                    break;
                }
            }

            if (!propsFound) {
                ShowSevereError( propsModObjName + " object, name: " + DataIPShortCuts::cAlphaArgs(2) + " not found");
                errorsFound = true;
            }

            newRF.maxSimYears = DataEnvironment::MaxNumberSimYears;

            // get number of g-function pairs
            int numPairs = 0;
            if (numNumbers % 2 == 0) {
                numPairs = numNumbers / 2;
            } else {
                errorsFound = true;
                ShowSevereError("Errors found processing response factor input for " + DataIPShortCuts::cCurrentModuleObject +
                                " object name: " + newRF.name);
                ShowSevereError("Uneven number of g-function pairs");
            }

            if (errorsFound) {
                ShowFatalError("Errors occurred during inputs for object: " + DataIPShortCuts::cCurrentModuleObject + " name: " + newRF.name);
            }

            // load g-function-LnTTs pairs into temporary arrays
            std::vector<std::vector<Real64>> LnTTs = {{}};
            std::vector<std::vector<Real64>> gFn = {{}};

            int indexNum = 1;
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
            bool errorsFound = false;

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

            // build out new instance
            GHEArray newArray;
            newArray.name = UtilityRoutines::MakeUPPERCase(DataIPShortCuts::cAlphaArgs(1));

            // find correct properties instance
            BoreholeProps props;
            bool propsFound = false;
            for (auto &thisProp : propsVect) {
                if (UtilityRoutines::SameString(DataIPShortCuts::cAlphaArgs(2), thisProp.propName)) {
                    props = thisProp;
                    propsFound = true;
                    break;
                }
            }

            if (!propsFound) {
                ShowSevereError( propsModObjName + " object, name: " + DataIPShortCuts::cAlphaArgs(2) + " not found");
                errorsFound = true;
            }

            if (errorsFound) {
                ShowFatalError("Errors occurred during inputs for object: " + DataIPShortCuts::cCurrentModuleObject + " name: " + newArray.name);
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
            bool errorsFound = false;

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

            // create new instance
            GHEBorehole newBH;
            newBH.name = UtilityRoutines::MakeUPPERCase(DataIPShortCuts::cAlphaArgs(1));

            // get right props object
            BoreholeProps props;
            bool propsFound = false;
            for (const auto& thisProp : propsVect) {
                if (UtilityRoutines::SameString(DataIPShortCuts::cAlphaArgs(2), thisProp.propName)) {
                    props = thisProp;
                    propsFound = true;
                    break;
                }
            }

            if (!propsFound) {
                ShowSevereError( propsModObjName + " object, name: " + DataIPShortCuts::cAlphaArgs(2) + " not found");
                errorsFound = true;
            }

            if (errorsFound) {
                ShowFatalError("Errors occurred during inputs for object: " + DataIPShortCuts::cCurrentModuleObject + " name: " + newBH.name);
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

            // Build out new instance
            EnhancedGHE newGHE;
            newGHE.name = UtilityRoutines::MakeUPPERCase(DataIPShortCuts::cAlphaArgs(1));

            // Inlet node
            newGHE.inletNode = NodeInputManager::GetOnlySingleNode(DataIPShortCuts::cAlphaArgs(2),
                                                                       errorsFound,
                                                                       DataIPShortCuts::cCurrentModuleObject,
                                                                       DataIPShortCuts::cAlphaArgs(1),
                                                                       DataLoopNode::NodeType_Water,
                                                                       DataLoopNode::NodeConnectionType_Inlet,
                                                                       1,
                                                                       DataLoopNode::ObjectIsNotParent);

            // Outlet node
            newGHE.outletNode = NodeInputManager::GetOnlySingleNode(DataIPShortCuts::cAlphaArgs(3),
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
            newGHE.designVolFlow = DataIPShortCuts::rNumericArgs(1);
            PlantUtilities::RegisterPlantCompDesignFlow(newGHE.inletNode, newGHE.designVolFlow);

            // Number of boreholes
            newGHE.numBH = DataIPShortCuts::rNumericArgs(2);

            // Soil
            newGHE.kSoil = DataIPShortCuts::rNumericArgs(3);
            newGHE.rhoCpSoil = DataIPShortCuts::rNumericArgs(4);

            // gb values - Exiting fluid temperature response factors
            if (!DataIPShortCuts::lAlphaFieldBlanks(6)) {
                // Response factors come from IDF object
                for (auto &thisRF : respFactorsVect) {
                    if (UtilityRoutines::SameString(DataIPShortCuts::cAlphaArgs(6), thisRF.name)) {
                        newGHE.gFuncEFT = thisRF;
                        newGHE.gFuncEFTExist = true;
                        break;
                    }
                }

                if (!newGHE.gFuncEFTExist) {
                    ShowSevereError(respFactModObjName + " object, name :" + DataIPShortCuts::cAlphaArgs(6) + " not found");
                    errorsFound = true;
                }
            }

            // g values - Borehole wall temperature response factors
            if (!DataIPShortCuts::lAlphaFieldBlanks(7)) {
                // Response factors come from IDF object
                for (auto &thisRF : respFactorsVect) {
                    if (UtilityRoutines::SameString(DataIPShortCuts::cAlphaArgs(7), thisRF.name)) {
                        newGHE.gFuncBWT = thisRF;
                        newGHE.gFuncBWTExist = true;
                        break;
                    }
                }

                if (!newGHE.gFuncBWTExist) {
                    ShowSevereError(respFactModObjName + " object, name :" + DataIPShortCuts::cAlphaArgs(7) + " not found");
                    errorsFound = true;
                }
            }

            // error checking variables
            bool initFromArrObj = false;
            bool initFromBHObjs = false;
            std::string arrName;

            // either g or gb were not found, so need to generate g-functions
            if ((DataIPShortCuts::lAlphaFieldBlanks(6) && !newGHE.gFuncBWTExist) | (DataIPShortCuts::lAlphaFieldBlanks(7) && !newGHE.gFuncEFTExist)) {
                if (!DataIPShortCuts::lAlphaFieldBlanks(8)) {
                    // borehole instances from array object
                    initFromArrObj = true;

                    std::string arrObjName;
                    for (auto &arr : arraysVect) {
                        if (UtilityRoutines::SameString(DataIPShortCuts::cAlphaArgs(8), arr.name)) {
                            newGHE.boreholes = arr.boreholes;
                            arrObjName = arr.name;
                            break;
                        }
                    }

                } else {
                    // borehole instances from single borehole objects
                    initFromBHObjs = true;

                    // get single boreholes
                    int numPreviousAlphaFields = 7;
                    int numBH = numAlphas - numPreviousAlphaFields;

                    int index = 9;
                    for (int bhNum = 1; bhNum < numBH; ++bhNum) {
                        bool bhFound = false;
                        for (auto it = boreholesVect.begin(); it < boreholesVect.end(); it++) {
                            auto &bh = *it;
                            if (UtilityRoutines::SameString(DataIPShortCuts::cAlphaArgs(index), bh.name)) {
                                // save this instance
                                newGHE.boreholes.push_back(bh);
                                bhFound = true;

                                // delete to prevent later reuse
                                boreholesVect.erase(it);
                                break;
                            }
                        }

                        if (!bhFound) {
                            ShowSevereError("Borehole \"" + DataIPShortCuts::cAlphaArgs(index) + "\" not found");
                            errorsFound = true;
                        }

                        ++index;
                    }
                }

                if (newGHE.boreholes.empty()) {
                    ShowSevereError("Problems getting inputs for " + DataIPShortCuts::cCurrentModuleObject + " object, name: " + newGHE.name);
                    ShowSevereError("Model requires both " + respFactModObjName + " objects, or");
                    ShowSevereError("boreholes to be described using " + bhModObjName + " or " + arrModObjName + " objects");
                    errorsFound = true;
                } else if (newGHE.numBH != (int)newGHE.boreholes.size()) {
                    if (initFromArrObj) {
                        ShowSevereError("Number of boreholes specified does not match number specified in " + arrModObjName + " object, name: " + arrName);
                    } else if (initFromBHObjs) {
                        ShowSevereError("Number of boreholes specified does not match number specified from " + bhModObjName + " objects");
                    } else {
                        assert(false); // LCOV_EXCL_LINE
                    }
                    errorsFound = true;
                }
            }

            if (errorsFound) {
                ShowFatalError("Errors occurred during inputs for object: " + DataIPShortCuts::cCurrentModuleObject + " name: " + newGHE.name);
            }

            // Initialize ground temperature model and get pointer reference
            // Do this last because it calls getObjectItem
            newGHE.gtm = GroundTemperatureManager::GetGroundTempModelAndInit(DataIPShortCuts::cAlphaArgs(4), DataIPShortCuts::cAlphaArgs(5));

            // Save object
            enhancedGHE.push_back(newGHE);
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

    void EnhancedGHE::onInitLoopEquip(const PlantLocation &EP_UNUSED(calledFromLocation))
    {
        if (this->oneTimeInit) {
            this->setupOutputVars();

            // populate location on the plant loop
            bool errorsFound = false;
            PlantUtilities::ScanPlantLoopsForObject(this->name, DataPlant::TypeOf_GrndHtExchgSystem, this->loopNum, this->loopSideNum, this->branchNum, this->compNum, errorsFound, _, _, _, _, _);

            // error if not found
            if (errorsFound) {
                ShowFatalError("Program terminated due to previous conditions.");
            }

            // setup fluid worker
            this->fluid.initialize(this->loopNum);

            // create an "average" borehole
            this->aveBH;
            for (auto &bh : this->boreholes) {
                this->aveBH.depth += bh.depth;
                this->aveBH.length += bh.length;
                this->aveBH.diameter += bh.diameter;
                this->aveBH.kGrout += bh.kGrout;
                this->aveBH.rhoCpGrout += bh.rhoCpGrout;
                this->aveBH.shankSpace += bh.shankSpace;
                this->aveBH.pipe.k += bh.pipe.k;
                this->aveBH.pipe.rhoCp += bh.pipe.rhoCp;
                this->aveBH.pipe.outerDia += bh.pipe.outerDia;
                this->aveBH.pipe.wallThickness += bh.pipe.wallThickness;
            }

            // setup pipe
            this->aveBH.pipe.initGeometry();
            this->aveBH.pipe.fluid = this->fluid;

            // check for EFT g-functions
            if (!this->gFuncEFTExist) {
            	this->generateEFTgFunc();
            	this->gFuncEFTExist = true;
            }

            // check for BWT g-functions
            if (!this->gFuncBWTExist) {
            	this->generateBWTgFunc();
            	this->gFuncBWTExist = true;
            }

            this->oneTimeInit = false;
        }
    }

    void EnhancedGHE::generateEFTgFunc()
    {

    }

    void EnhancedGHE::generateBWTgFunc()
    {
    }

    void EnhancedGHE::simulate(const PlantLocation &EP_UNUSED(calledFromLocation), bool EP_UNUSED(FirstHVACIteration), Real64 &EP_UNUSED(CurLoad), bool EP_UNUSED(RunFlag))
    {
    }

    void FluidWorker::initialize(int _loopNum)
    {
        this->fluidName = DataPlant::PlantLoop(_loopNum).FluidName;
        this->loopNum = _loopNum;
        this->fluidIdx = DataPlant::PlantLoop(_loopNum).FluidIndex;
    }

    Real64 FluidWorker::getCp(Real64 const &temperature, const std::string &routineName)
    {
        return FluidProperties::GetSpecificHeatGlycol(this->fluidName, temperature, this->fluidIdx, routineName);
    }

    Real64 FluidWorker::getCond(Real64 const &temperature, const std::string &routineName)
    {
        return FluidProperties::GetConductivityGlycol(this->fluidName, temperature, this->fluidIdx, routineName);
    }

    Real64 FluidWorker::getVisc(Real64 const &temperature, const std::string &routineName)
    {
        return FluidProperties::GetViscosityGlycol(this->fluidName, temperature, this->fluidIdx, routineName);
    }

    Real64 FluidWorker::getRho(Real64 const &temperature, const std::string &routineName)
    {
        return FluidProperties::GetDensityGlycol(this->fluidName, temperature, this->fluidIdx, routineName);
    }

    Real64 FluidWorker::getPrandtl(Real64 const &temperature, const std::string &routineName)
    {
        Real64 cp = this->getCp(temperature, routineName);
        Real64 mu = this->getVisc(temperature, routineName);
        Real64 k = this->getCond(temperature, routineName);

        return cp * mu / k;
    }

    void Pipe::initGeometry()
    {
        this->innerDia = this->outerDia - 2 * this->wallThickness;
        this->innerRadius = this->innerDia / 2.0;
        this->outerRadius = this->outerDia / 2.0;
    }

    Real64 Pipe::mdotToRe(Real64 flowRate, Real64 temperature)
    {
        static std::string const routineName("Pipe::mdotToRe");
        Real64 mu = this->fluid.getVisc(temperature, routineName);
        return 4.0 * flowRate / (mu * DataGlobals::Pi * this->innerDia);
    }

} // namespace GroundHeatExchangerEnhanced

} // namespace EnergyPlus
