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
#include "GroundHeatExchangerEnhanced.hh"
#include <EnergyPlus/BranchNodeConnections.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataIPShortCuts.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/FluidProperties.hh>
#include <EnergyPlus/InputProcessing/InputProcessor.hh>
#include <EnergyPlus/NodeInputManager.hh>
#include <EnergyPlus/OutputProcessor.hh>
#include <EnergyPlus/Plant/DataPlant.hh>
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

    Real64 smoothingFunc(Real64 const &x, Real64 const &a, Real64 const &b)
    {
        //  Sigmoid smoothing function
        //
        //  https://en.wikipedia.org/wiki/Sigmoid_function
        //
        //  param x: independent variable
        //  param a: fitting parameter 1
        //  param b: fitting parameter 2
        //  @return float between 0-1

        return 1 / (1 + std::exp(-(x - a) / b));
    }

    std::vector<Real64> solveTDM(std::vector<Real64> a, std::vector<Real64> b, std::vector<Real64> c, std::vector<Real64> d)
    {
        // Tri-diagonal matrix solver

        // This solver expects the ghost points at a(0) and c(n) to be present

        // a(0) = 0
        // c(n) = 0

        // len(a) = len(b) = len(c) = len(d)

        // Adapted from: https://en.wikibooks.org/wiki/Algorithm_Implementation/Linear_Algebra/Tridiagonal_matrix_algorithm#C++

        // param a: west diagonal vector from coefficient matrix
        // param b: center diagonal vector from coefficient matrix
        // param c: east diagonal vector from coefficient matrix
        // param d: column vector
        // returns solution vector

        u_int n = d.size() - 1;

        c[0] /= b[0];
        d[0] /= b[0];

        for (u_int i = 1; i < n; ++i) {
            c[i] /= b[i] - a[i] * c[i - 1];
            d[i] = (d[i] - a[i] * d[i - 1]) / (b[i] - a[i] * c[i - 1]);
        }

        d[n] = (d[n] - a[n] * d[n - 1]) / (b[n] - a[n] * c[n - 1]);

        for (int i = n; i-- > 0;) {
            d[i] -= c[i] * d[i + 1];
        }

        return d;
    }

    inline bool isEven(unsigned const &x) {return !(x % 2);}

    PlantComponent *EnhancedGHE::factory(EnergyPlusData &state, std::string const &objectName)
    {
        if (getInput) {
            getGHEInput(state);
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

    void getGHEInput(EnergyPlusData &state)
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
            newGHE.alphaSoil = newGHE.kSoil / newGHE.rhoCpSoil;

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
            newGHE.gtm = GroundTemperatureManager::GetGroundTempModelAndInit(state, DataIPShortCuts::cAlphaArgs(4), DataIPShortCuts::cAlphaArgs(5));

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

    void EnhancedGHE::onInitLoopEquip(EnergyPlusData &state, const PlantLocation &EP_UNUSED(calledFromLocation))
    {
        if (this->oneTimeInit) {
            this->setupOutputVars();

            // populate location on the plant loop
            bool errorsFound = false;
            PlantUtilities::ScanPlantLoopsForObject(state.dataBranchInputManager, this->name, DataPlant::TypeOf_GrndHtExchgSystem, this->loopNum, this->loopSideNum, this->branchNum, this->compNum, errorsFound, _, _, _, _, _);

            // error if not found
            if (errorsFound) {
                ShowFatalError("Program terminated due to previous conditions.");
            }

            // setup fluid worker
            this->fluid.initialize(this->loopNum);

            // create an "average" borehole
            for (auto &bh : this->boreholes)
            {
                this->aveBH.depth += bh.depth / this->numBH;
                this->aveBH.length += bh.length / this->numBH;
                this->aveBH.diameter += bh.diameter / this->numBH;
                this->aveBH.kGrout += bh.kGrout / this->numBH;
                this->aveBH.rhoCpGrout += bh.rhoCpGrout / this->numBH;
                this->aveBH.shankSpace += bh.shankSpace / this->numBH;
                this->aveBH.pipe.k += bh.pipe.k / this->numBH;
                this->aveBH.pipe.rhoCp += bh.pipe.rhoCp / this->numBH;
                this->aveBH.pipe.outerDia += bh.pipe.outerDia / this->numBH;
                this->aveBH.pipe.wallThickness += bh.pipe.wallThickness / this->numBH;
            }

            // setup borehole
            this->aveBH.initialize(this->kSoil, this->rhoCpSoil);
            this->aveBH.pipe.fluid = this->fluid;

            // check for EFT g-functions
            if (!this->gFuncEFTExist) {
            	this->generateEFTgFunc();
            	this->gFuncEFTExist = true;
            }

            // check for BWT g-functions
            if (!this->gFuncBWTExist) {

                // setup boreholes
                for (auto &bh: this->boreholes) {
                    bh.initialize(this->kSoil, this->rhoCpSoil);
                    bh.pipe.fluid = this->fluid;
                }

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
        std::string const routineName = "EnhancedGHE::generateBWTgFunc";

        // short timestep borehole wall g-functions
        Real64 volFlowPerBH = this->designVolFlow / this->numBH;
        Real64 temperature = 20.0;
        Real64 rho = this->fluid.getRho(temperature, routineName);
        Real64 mdot = volFlowPerBH * rho;
        this->aveBH.calcShortTimestepGFunctions(mdot, temperature);

        // long timestep borehole wall g-functions
        this->calcLongTimeStepGFunctions();

    }

    void EnhancedGHE::simulate(EnergyPlusData &EP_UNUSED(state), const PlantLocation &EP_UNUSED(calledFromLocation), bool EP_UNUSED(FirstHVACIteration), Real64 &EP_UNUSED(CurLoad), bool EP_UNUSED(RunFlag))
    {
    }

    void EnhancedGHE::calcLongTimeStepGFunctions()
    {
        int const numDaysInYear(365);

        // Minimum simulation time for which finite line source method is applicable
        Real64 const lntts_min_for_long_timestep = -8.5;

        Real64 const t_s = pow_2(this->aveBH.length) / (9 * this->alphaSoil);
        Real64 const lnttsStepSize = 0.5;

        // only setup g-functions for the max possible number of sim years
        Real64 const maxSimYears = DataEnvironment::MaxNumberSimYears;

        this->lnttsLTS.push_back(lntts_min_for_long_timestep);
        std::vector<Real64> time(1, std::exp(lntts_min_for_long_timestep) * t_s);

        // Setup Ln(t/ts) values
        while (true) {
            Real64 maxPossibleSimTime = exp(this->lnttsLTS.back()) * t_s;
            if (maxPossibleSimTime < maxSimYears * numDaysInYear * DataGlobals::HoursInDay * DataGlobals::SecInHour) {
                this->lnttsLTS.push_back(this->lnttsLTS.back() + lnttsStepSize);
                time.push_back(std::exp(this->lnttsLTS.back()) * t_s);
            } else {
                break;
            }
        }

        this->gLTS = std::vector<Real64> (this->lnttsLTS.size(), 0.0);

        Real64 totalTubeLength = 0;
        for (const auto& bh : this->boreholes) {
            totalTubeLength += bh.length;
        }

        DisplayString("Initializing GroundHeatExchanger:System: " + name);

        // Calculate the g-functions
        for (size_t lntts_index = 0; lntts_index != this->lnttsLTS.size(); ++lntts_index) {
            for (auto &bh_i : this->boreholes) {
                Real64 sum_T_ji = 0;
                for (auto &bh_j : this->boreholes) {
                    sum_T_ji += doubleIntegral(bh_i, bh_j, time[lntts_index]);
                }
                this->gLTS[lntts_index] += sum_T_ji;
            }
            this->gLTS[lntts_index] /= (2 * totalTubeLength);

            // std::stringstream ss;
            // ss << std::fixed << std::setprecision(1) << float(lntts_index) / myRespFactors->LNTTS.size() * 100;

            // DisplayString("...progress: " + ss.str() + "%");
        }
    }

    Real64 EnhancedGHE::calcResponse(std::vector<Real64> const &dists, Real64 const &currTime) const
    {
        Real64 pointToPointResponse = erfc(dists[0] / (2 * sqrt(this->alphaSoil * currTime))) / dists[0];
        Real64 pointToReflectedResponse = erfc(dists[1] / (2 * sqrt(this->alphaSoil * currTime))) / dists[1];

        return pointToPointResponse - pointToReflectedResponse;
    }

    std::vector<Real64> EnhancedGHE::distances(PointLoc const &point_i, PointLoc const &point_j)
    {
        std::vector<Real64> sumVals;

        // Calculate the distance between points
        sumVals.push_back(pow_2(point_i.x - point_j.x));
        sumVals.push_back(pow_2(point_i.y - point_j.y));
        sumVals.push_back(pow_2(point_i.z - point_j.z));

        Real64 sumTot = 0;
        std::vector<Real64> retVals;
        std::for_each(sumVals.begin(), sumVals.end(), [&](Real64 n) { sumTot += n; });
        retVals.push_back(std::sqrt(sumTot));

        // Calculate distance to mirror point
        sumVals.pop_back();
        sumVals.push_back(pow_2(point_i.z - (-point_j.z)));

        sumTot = 0;
        std::for_each(sumVals.begin(), sumVals.end(), [&](Real64 n) { sumTot += n; });
        retVals.push_back(std::sqrt(sumTot));

        return retVals;
    }

    Real64 EnhancedGHE::integral(const PointLoc &point_i, const GHEBorehole &bh_j, Real64 const &currTime) const
    {
        // This code could be optimized in a number of ways.
        // The first, most simple way would be to precompute the distances from point i to point j,
        // then store them for reuse. The second, more intensive method would be to break the calcResponse
        // calls out into four different parts. The first point, last point, odd points, and even points.
        // Then multiply the odd/even points by their respective coefficient for the Simpson's method.
        // After that, all points are summed together and divided by 3.

        Real64 sum_f = 0;
        unsigned index = 0;
        unsigned lastIndex_j = bh_j.pointLocations_j.size() - 1;
        for (auto &point_j : bh_j.pointLocations_j) {
            std::vector<Real64> dists = distances(point_i, point_j);
            Real64 const f = this->calcResponse(dists, currTime);

            // Integrate using Simpson's
            if (index == 0 || index == lastIndex_j) {
                sum_f += f;
            } else if (isEven(index)) {
                sum_f += 2 * f;
            } else {
                sum_f += 4 * f;
            }

            ++index;
        }

        return (bh_j.dl_j / 3.0) * sum_f;
    }

    Real64 EnhancedGHE::doubleIntegral(const GHEBorehole &bh_i, const GHEBorehole &bh_j, Real64 const &currTime) const
    {
        // Similar optimizations as discussed above could happen here
        if (&bh_i == &bh_j) {

            Real64 sum_f = 0;
            unsigned index = 0;
            unsigned lastIndex = bh_i.pointLocations_ii.size() - 1;
            for (auto &thisPoint : bh_i.pointLocations_ii) {

                Real64 f = this->integral(thisPoint, bh_j, currTime);

                // Integrate using Simpson's
                if (index == 0 || index == lastIndex) {
                    sum_f += f;
                } else if (isEven(index)) {
                    sum_f += 2 * f;
                } else {
                    sum_f += 4 * f;
                }

                ++index;
            }

            return (bh_i.dl_ii / 3.0) * sum_f;

        } else {

            Real64 sum_f = 0;
            unsigned index = 0;
            unsigned const lastIndex = bh_i.pointLocations_i.size() - 1;
            for (auto &thisPoint : bh_i.pointLocations_i) {

                Real64 f = integral(thisPoint, bh_j, currTime);

                // Integrate using Simpson's
                if (index == 0 || index == lastIndex) {
                    sum_f += f;
                } else if (isEven(index)) {
                    sum_f += 2 * f;
                } else {
                    sum_f += 4 * f;
                }

                ++index;
            }

            return (bh_i.dl_i / 3.0) * sum_f;
        }
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

    void Pipe::initialize()
    {
        this->innerDia = this->outerDia - 2 * this->wallThickness;
        this->innerRadius = this->innerDia / 2.0;
        this->outerRadius = this->outerDia / 2.0;
    }

    Real64 Pipe::mdotToRe(Real64 const &flowRate, Real64 const &temperature)
    {
        static std::string const routineName("Pipe::mdotToRe");
        Real64 mu = this->fluid.getVisc(temperature, routineName);
        return 4.0 * flowRate / (mu * DataGlobals::Pi * this->innerDia);
    }

    Real64 Pipe::laminarFrictionFactor(Real64 const &Re)
    {
        // laminar friction factor

        // param Re: Reynolds number
        // returns: friction factor

        return 64 / Re;
    }

    Real64 Pipe::turbulentFrictionFactor(Real64 const &Re)
    {
        // turbulent friction factor

        // Petukhov, B. S. (1970). Advances in Heat Transfer, volume 6, chapter Heat transfer and
        // friction in turbulent pipe flow with variable physical properties, pages 503–564.
        // Academic Press, Inc., New York, NY.

        // param Re: Reynolds number
        // returns: friction factor

        return std::pow(0.79 * std::log(Re) - 1.64, -2.0);
    }

    Real64 Pipe::frictionFactor(Real64 const &Re)
    {
        // smooth pipe friction factor

        // param Re: Reynolds number
        // returns: friction factor

        Real64 lowRe = 1500;
        Real64 highRe = 5000;

        if (Re < lowRe) {
            this->friction = laminarFrictionFactor(Re);
        } else if (lowRe <= Re && Re < highRe) {
            Real64 fLow = laminarFrictionFactor(Re);
            Real64 fHigh = turbulentFrictionFactor(Re);
            Real64 sigma = smoothingFunc(Re, 3000, 450);
            this->friction = (1 - sigma) * fLow + sigma * fHigh;
        } else
            this->friction = turbulentFrictionFactor(Re);
        return this->friction;
    }

    Real64 Pipe::laminarNusselt()
    {
        // laminar Nusselt number for smooth pipes
        // mean(4.36, 3.66)

        // returns Nusselt number

        return 4.01;
    }

    Real64 Pipe::turbulentNusselt(Real64 const &Re, Real64 const &temperature)
    {
        // turbulent Nusselt number

        // Gnielinski, V. 1976. 'New equations for heat and mass transfer in turbulent pipe and channel flow.'
        // International Chemical Engineering 16(1976), pp. 359-368.

        // param Re: Reynolds number
        // param temperature: temperature, C
        // returns: Nusselt number

        static std::string const routineName("Pipe::turbulentNusselt");

        // friction factor
        Real64 f = this->frictionFactor(Re);

        // Prandtl number
        Real64 Pr = this->fluid.getPrandtl(temperature, routineName);

        return (f / 8) * (Re - 1000) * Pr / (1 + 12.7 * std::pow(f / 8, 0.5) * (std::pow(Pr, 2.0 / 3.0) - 1));
    }

    Real64 Pipe::calcConvResist(Real64 const &flowRate, Real64 const &temperature)
    {
    // Calculates the convection resistance using Gnielinski and Petukhov, in [k/(W/m)]

    // Gnielinski, V. 1976. 'New equations for heat and mass transfer in turbulent pipe and channel flow.'
    // International Chemical Engineering 16(1976), pp. 359-368.

    // param flow_rate: mass flow rate, kg/s
    // param temperature: temperature, C
    // returns: convection resistance, K/(W/m)

    static std::string const routineName("Pipe::calcConvResist");

    Real64 lowRe = 2000;
    Real64 highRe = 4000;

    Real64 Re = this->mdotToRe(flowRate, temperature);
    Real64 Nu = 0;

    if (Re < lowRe) {
        Nu = Pipe::laminarNusselt();
    } else if (lowRe <= Re && Re < highRe) {
        Real64 NuLow = laminarNusselt();
        Real64 NuHigh = turbulentNusselt(Re, temperature);
        Real64 sigma = smoothingFunc(Re, 3000, 150);
        Nu = (1 - sigma) * NuLow + sigma * NuHigh;
    } else
        Nu = Pipe::turbulentNusselt(Re, temperature);

    this->convResist = 1 / (Nu * DataGlobals::Pi * this->fluid.getCond(temperature, routineName));
    return this->convResist;
    }

    Real64 Pipe::calcCondResist()
    {
        // Calculates the thermal resistance of a pipe, in [K/(W/m)].

        // Javed, S. and Spitler, J.D. 2017. 'Accuracy of borehole thermal resistance calculation methods
        // for grouted single U-tube ground heat exchangers.' Applied Energy. 187: 790-806.

        // returns: conduction resistance, K/(W/m)
        this->condResist = std::log(this->outerDia / this->innerDia) / (2 * DataGlobals::Pi * this->k);
        return this->condResist;
    }

    Real64 Pipe::calcResistance(Real64 const &flowRate, Real64 const &temperature)
    {
        // Calculates the combined conduction and convection pipe resistance

        // Javed, S. and Spitler, J.D. 2017. 'Accuracy of borehole thermal resistance calculation methods
        // for grouted single U-tube ground heat exchangers.' Applied Energy. 187: 790-806.

        // Equation 3

        // param flowRate: mass flow rate, kg/s
        // param temperature: temperature, C
        // returns: pipe resistance, K/(W/m)

        this->totResist = this->calcConvResist(flowRate, temperature) + this->calcCondResist();
        return this->totResist;
    }

    void GHEBorehole::initialize(Real64 const &_kSoil, Real64 const &_rhoCpSoil)
    {
        // setup pipe
        this->pipe.initialize();

        // soil props
        this->kSoil = _kSoil;
        this->rhoCpSoil = _rhoCpSoil;

        // bh resistance
        this->c0 = 2.0 * DataGlobals::Pi * this->kSoil;
        this->radius = this->diameter / 2.0;
        this->theta1 = this->shankSpace / (2 * this->radius);
        this->theta2 = this->radius / this->pipe.outerRadius;
        this->theta3 = 1 / (2 * this->theta1 * this->theta2);
        this->sigma = (this->kGrout - this->kSoil) / (this->kGrout + this->kSoil);

        // point to point setup for g-functions

        // Using Simpson's rule the number of points (n+1) must be odd, therefore an even number of panels is required
        // Starting from i = 0 to i <= NumPanels produces an odd number of points
        int constexpr numPanels_i = 50;
        int constexpr numPanels_ii = 50;
        int constexpr numPanels_j = 560;

        this->dl_i = this->length / numPanels_i;
        for (int i = 0; i <= numPanels_i; ++i) {
            PointLoc newPoint;
            newPoint.x = this->xLoc;
            newPoint.y = this->yLoc;
            newPoint.z = this->depth + (i * this->dl_i);
            this->pointLocations_i.push_back(newPoint);
        }

        this->dl_ii = this->length / numPanels_ii;
        for (int i = 0; i <= numPanels_ii; ++i) {
            PointLoc newPoint;
            // For case when bh is being compared to itself, shift points by 1 radius in the horizontal plane
            newPoint.x = this->xLoc + (this->diameter / 2.0) / sqrt(2.0);
            newPoint.y = this->yLoc + (this->diameter / 2.0) / (-sqrt(2.0));
            newPoint.z = this->depth + (i * this->dl_ii);
            this->pointLocations_ii.push_back(newPoint);
        }

        this->dl_j = this->length / numPanels_j;
        for (int i = 0; i <= numPanels_j; ++i) {
            PointLoc newPoint;
            newPoint.x = this->xLoc;
            newPoint.y = this->yLoc;
            newPoint.z = this->depth + (i * this->dl_j);
            this->pointLocations_j.push_back(newPoint);
        }

    }

    Real64 GHEBorehole::calcTotIntResistWorker(Real64 const &beta)
    {
        // Calculates the total internal thermal resistance of the borehole using the first-order multipole method.

        // Javed, S. & Spitler, J.D. 2016. 'Accuracy of Borehole Thermal Resistance Calculation Methods
        // for Grouted Single U-tube Ground Heat Exchangers.' Applied Energy. 187:790-806.

        // Equation 26

        Real64 final_term_1 = log(pow(1 + pow_2(this->theta1), this->sigma) / (this->theta3 * pow(1 - pow_2(this->theta1), this->sigma)));
        Real64 num_term_2 = pow_2(this->theta3) * pow_2(1 - pow_4(this->theta1) + 4 * this->sigma * pow_2(this->theta1));
        Real64 den_term_2_pt_1 = (1 + beta) / (1 - beta) * pow_2(1 - pow_4(this->theta1));
        Real64 den_term_2_pt_2 = pow_2(this->theta3) * pow_2(1 - pow_4(this->theta1));
        Real64 den_term_2_pt_3 = 8 * this->sigma * pow_2(this->theta1) * pow_2(this->theta3) * (1 + pow_4(this->theta1));
        Real64 den_term_2 = den_term_2_pt_1 - den_term_2_pt_2 + den_term_2_pt_3;
        Real64 final_term_2 = num_term_2 / den_term_2;

        return (1 / (DataGlobals::Pi * this->kGrout)) * (beta + final_term_1 - final_term_2);
    }

    Real64 GHEBorehole::calcTotIntResist(Real64 const &pipeResist)
    {
        // Calculates the total internal thermal resistance of the borehole using the first-order multipole method.

        // Javed, S. & Spitler, J.D. 2016. 'Accuracy of Borehole Thermal Resistance Calculation Methods
        // for Grouted Single U-tube Ground Heat Exchangers.' Applied Energy. 187:790-806.

        // Equation 26

        Real64 beta = 2 * DataGlobals::Pi * this->kGrout * pipeResist;
        return this->calcTotIntResistWorker(beta);
    }

    Real64 GHEBorehole::calcTotIntResist(Real64 const &flowRate, Real64 const &temperature)
    {
        // Calculates the total internal thermal resistance of the borehole using the first-order multipole method.

        // Javed, S. & Spitler, J.D. 2016. 'Accuracy of Borehole Thermal Resistance Calculation Methods
        // for Grouted Single U-tube Ground Heat Exchangers.' Applied Energy. 187:790-806.

        // Equation 26

        Real64 beta = 2 * DataGlobals::Pi * this->kGrout * this->pipe.calcResistance(flowRate, temperature);
        return this->calcTotIntResistWorker(beta);
    }

    Real64 GHEBorehole::calcAveResistWorker(Real64 const &beta)
    {
        // Calculates the average thermal resistance of the borehole using the first-order multipole method.

        // Javed, S. & Spitler, J.D. 2016. 'Accuracy of Borehole Thermal Resistance Calculation Methods
        // for Grouted Single U-tube Ground Heat Exchangers.' Applied Energy.187:790-806.

        // Equation 13

        Real64 const final_term_1 = log(this->theta2 / (2 * this->theta1 * pow(1 - pow_4(this->theta1), this->sigma)));
        Real64 const num_final_term_2 = pow_2(this->theta3) * pow_2(1 - (4 * this->sigma * pow_4(this->theta1)) / (1 - pow_4(this->theta1)));
        Real64 const den_final_term_2_pt_1 = (1 + beta) / (1 - beta);
        Real64 const den_final_term_2_pt_2 = pow_2(this->theta3) * (1 + (16 * this->sigma * pow_4(this->theta1)) / pow_2(1 - pow_4(this->theta1)));
        Real64 const den_final_term_2 = den_final_term_2_pt_1 + den_final_term_2_pt_2;
        Real64 const final_term_2 = num_final_term_2 / den_final_term_2;

        return (1 / (4 * DataGlobals::Pi * this->kGrout)) * (beta + final_term_1 - final_term_2);
    }

    Real64 GHEBorehole::calcAveResist(Real64 const &pipeResist)
    {
        // Calculates the average thermal resistance of the borehole using the first-order multipole method.

        // Javed, S. & Spitler, J.D. 2016. 'Accuracy of Borehole Thermal Resistance Calculation Methods
        // for Grouted Single U-tube Ground Heat Exchangers.' Applied Energy.187:790-806.

        // Equation 13

        Real64 const beta = 2 * DataGlobals::Pi * this->kGrout * pipeResist;
        return this->calcAveResistWorker(beta);
    }

    Real64 GHEBorehole::calcAveResist(Real64 const &flowRate, Real64 const &temperature)
    {
        // Calculates the average thermal resistance of the borehole using the first-order multipole method.

        // Javed, S. & Spitler, J.D. 2016. 'Accuracy of Borehole Thermal Resistance Calculation Methods
        // for Grouted Single U-tube Ground Heat Exchangers.' Applied Energy.187:790-806.

        // Equation 13

        Real64 const beta = 2 * DataGlobals::Pi * this->kGrout * this->pipe.calcResistance(flowRate, temperature);
        return this->calcAveResistWorker(beta);
    }

    Real64 GHEBorehole::calcGroutResist(Real64 const &pipeResist)
    {
        // Calculates grout resistance. Use for validation.

        // Javed, S. & Spitler, J.D. 2016. 'Accuracy of Borehole Thermal Resistance Calculation Methods
        // for Grouted Single U-tube Ground Heat Exchangers.' Applied Energy. 187:790-806.

        // Equation 3

        return this->calcAveResist(pipeResist) - pipeResist / 2.0;
    }

    Real64 GHEBorehole::calcGroutResist(Real64 const &flowRate, Real64 const &temperature)
    {
        // Calculates grout resistance. Use for validation.

        // Javed, S. & Spitler, J.D. 2016. 'Accuracy of Borehole Thermal Resistance Calculation Methods
        // for Grouted Single U-tube Ground Heat Exchangers.' Applied Energy. 187:790-806.

        // Equation 3

        return this->calcAveResist(flowRate, temperature) - this->pipe.calcResistance(flowRate, temperature) / 2.0;
    }

    Real64 GHEBorehole::calcDirectCouplingResist(const Real64 &flowRate, const Real64 &temperature)
    {
        Real64 ra = this->calcTotIntResist(flowRate, temperature);
        Real64 rb = this->calcAveResist(flowRate, temperature);
        Real64 r12 = (4.0 * ra * rb) / (4.0 * rb - ra);

        // reset high if negative
        // may need further investigation
        if (r12 < 0.0) {
            r12 = 70;
        }

        return r12;
    }

    void GHEBorehole::calcShortTimestepGFunctions(Real64 const &flowRate, Real64 const &temperature)
    {
        // X. Xu and Jeffrey D. Spitler. 2006. 'Modeling of Vertical Ground Loop Heat Exchangers
        // with Variable Convective Resistance and Thermal Mass of the Fluid.' in Proceedings of
        // the 10th International Conference on Thermal Energy Storage-EcoStock. Pomona, NJ, May 31-June 2.

        static std::string const routineName("GHEBorehole::calcShortTimestepGFunctions");

        enum class CellType
        {
            FLUID,
            CONVECTION,
            PIPE,
            GROUT,
            SOIL
        };

        struct Cell
        {
            // Members
            CellType type;
            Real64 radius_center;
            Real64 radius_outer;
            Real64 radius_inner;
            Real64 thickness;
            Real64 vol;
            Real64 conductivity;
            Real64 rhoCp;
            Real64 temperature;
            Real64 temperature_prev_ts;

            // Default constructor
            Cell()
                : type(), radius_center(0.0), radius_outer(0.0), radius_inner(0.0), thickness(0.0), vol(0.0), conductivity(0.0), rhoCp(0.0),
                  temperature(0.0), temperature_prev_ts(0.0)
            {
            }

            // Destructor
            ~Cell() = default;
        };

        // vector to hold 1-D cells
        std::vector<Cell> Cells;

        // setup pipe, convection, and fluid layer geometries
        int const num_pipe_cells = 4;
        int const num_conv_cells = 1;
        int const num_fluid_cells = 3;
        Real64 const pcf_cell_thickness = this->pipe.wallThickness / num_pipe_cells;
        Real64 const radius_pipe_out = std::sqrt(2) * this->pipe.outerRadius;
        Real64 const radius_pipe_in = radius_pipe_out - this->pipe.wallThickness;
        Real64 const radius_conv = radius_pipe_in - num_conv_cells * pcf_cell_thickness;
        Real64 const radius_fluid = radius_conv - (num_fluid_cells - 0.5) * pcf_cell_thickness; // accounts for half thickness of boundary cell

        // setup grout layer geometry
        int const num_grout_cells = 27;
        Real64 const radius_grout = this->radius;
        Real64 const grout_cell_thickness = (radius_grout - radius_pipe_out) / num_grout_cells;

        // setup soil layer geometry
        int const num_soil_cells = 500;
        Real64 const radius_soil = 10;
        Real64 const soil_cell_thickness = (radius_soil - radius_grout) / num_soil_cells;

        // calculate equivalent thermal resistance between borehole wall and fluid
        Real64 bhResistance = this->calcAveResist(flowRate, temperature);
        Real64 bhConvectionResistance = this->pipe.calcConvResist(flowRate, temperature);
        Real64 bh_equivalent_resistance_tube_grout = bhResistance - bhConvectionResistance / 2.0;
        Real64 bh_equivalent_resistance_convection = bhResistance - bh_equivalent_resistance_tube_grout;

        Real64 initial_temperature = temperature;
        Real64 cpFluid_init = this->pipe.fluid.getCp(temperature, routineName);
        Real64 fluidDensity_init = this->pipe.fluid.getRho(temperature, routineName);

        // initialize the fluid cells
        for (int i = 0; i < num_fluid_cells; ++i) {
            Cell thisCell;
            thisCell.type = CellType::FLUID;
            thisCell.thickness = pcf_cell_thickness;
            thisCell.radius_center = radius_fluid + i * thisCell.thickness;

            // boundary cell is only half thickness
            if (i == 0) {
                thisCell.radius_inner = thisCell.radius_center;
            } else {
                thisCell.radius_inner = thisCell.radius_center - thisCell.thickness / 2.0;
            }

            thisCell.radius_outer = thisCell.radius_center + thisCell.thickness / 2.0;
            thisCell.conductivity = 200;
            thisCell.rhoCp = 2.0 * cpFluid_init * fluidDensity_init * pow_2(this->pipe.innerRadius) / (pow_2(radius_conv) - pow_2(radius_fluid));
            Cells.push_back(thisCell);
        }

        // initialize the convection cells
        for (int i = 0; i < num_conv_cells; ++i) {
            Cell thisCell;
            thisCell.thickness = pcf_cell_thickness;
            thisCell.radius_inner = radius_conv + i * thisCell.thickness;
            thisCell.radius_center = thisCell.radius_inner + thisCell.thickness / 2.0;
            thisCell.radius_outer = thisCell.radius_inner + thisCell.thickness;
            thisCell.conductivity = log(radius_pipe_in / radius_conv) / (2 * DataGlobals::Pi * bh_equivalent_resistance_convection);
            thisCell.rhoCp = 1;
            Cells.push_back(thisCell);
        }

        // initialize pipe cells
        for (int i = 0; i < num_pipe_cells; ++i) {
            Cell thisCell;
            thisCell.type = CellType::PIPE;
            thisCell.thickness = pcf_cell_thickness;
            thisCell.radius_inner = radius_pipe_in + i * thisCell.thickness;
            thisCell.radius_center = thisCell.radius_inner + thisCell.thickness / 2.0;
            thisCell.radius_outer = thisCell.radius_inner + thisCell.thickness;
            thisCell.conductivity = log(radius_grout / radius_pipe_in) / (2 * DataGlobals::Pi * bh_equivalent_resistance_tube_grout);
            thisCell.rhoCp = this->pipe.rhoCp;
            Cells.push_back(thisCell);
        }

        // initialize grout cells
        for (int i = 0; i < num_grout_cells; ++i) {
            Cell thisCell;
            thisCell.type = CellType::GROUT;
            thisCell.thickness = grout_cell_thickness;
            thisCell.radius_inner = radius_pipe_out + i * thisCell.thickness;
            thisCell.radius_center = thisCell.radius_inner + thisCell.thickness / 2.0;
            thisCell.radius_outer = thisCell.radius_inner + thisCell.thickness;
            thisCell.conductivity = log(radius_grout / radius_pipe_in) / (2 * DataGlobals::Pi * bh_equivalent_resistance_tube_grout);
            thisCell.rhoCp = this->rhoCpGrout;
            Cells.push_back(thisCell);
        }

        // initialize soil cells
        for (int i = 0; i < num_soil_cells; ++i) {
            Cell thisCell;
            thisCell.type = CellType::SOIL;
            thisCell.thickness = soil_cell_thickness;
            thisCell.radius_inner = radius_grout + i * thisCell.thickness;
            thisCell.radius_center = thisCell.radius_inner + thisCell.thickness / 2.0;
            thisCell.radius_outer = thisCell.radius_inner + thisCell.thickness;
            thisCell.conductivity = this->kSoil;
            thisCell.rhoCp = this->rhoCpSoil;
            Cells.push_back(thisCell);
        }

        // other non-geometric specific setup
        for (auto &thisCell : Cells) {
            thisCell.vol = DataGlobals::Pi * (pow_2(thisCell.radius_outer) - pow_2(thisCell.radius_inner));
            thisCell.temperature = initial_temperature;
        }

        // set upper limit of time for the short time-step g-function calcs so there is some overlap
        Real64 const lntts_max_for_short_timestep = -9.0;
        Real64 const alphaSoil = this->kSoil / this->rhoCpSoil;
        Real64 const t_s = pow_2(this->length) / (9.0 * alphaSoil);

        Real64 const time_step = 120;
        Real64 const time_max_for_short_timestep = exp(lntts_max_for_short_timestep) * t_s;
        Real64 total_time = 0;

        Real64 const heat_flux = 40.0;

        // time step loop
        while (total_time < time_max_for_short_timestep) {

            for (auto &thisCell : Cells) {
                thisCell.temperature_prev_ts = thisCell.temperature;
            }

            std::vector<Real64> a;
            std::vector<Real64> b;
            std::vector<Real64> c;
            std::vector<Real64> d;

            // setup tdma matrices
            int num_cells = Cells.size();
            for (int cell_index = 0; cell_index < num_cells; ++cell_index) {
                if (cell_index == 0) {
                    // heat flux BC

                    auto &thisCell = Cells[cell_index];
                    auto &eastCell = Cells[cell_index + 1];

                    Real64 FE1 = log(thisCell.radius_outer / thisCell.radius_center) / (2 * DataGlobals::Pi * thisCell.conductivity);
                    Real64 FE2 = log(eastCell.radius_center / eastCell.radius_inner) / (2 * DataGlobals::Pi * eastCell.conductivity);
                    Real64 AE = 1 / (FE1 + FE2);

                    Real64 AD = thisCell.rhoCp * thisCell.vol / time_step;

                    a.push_back(0);
                    b.push_back(-AE / AD - 1);
                    c.push_back(AE / AD);
                    d.push_back(-thisCell.temperature_prev_ts - heat_flux / AD);

                } else if (cell_index == num_cells - 1) {
                    // const ground temp bc

                    auto &thisCell = Cells[cell_index];

                    a.push_back(0);
                    b.push_back(1);
                    c.push_back(0);
                    d.push_back(thisCell.temperature_prev_ts);

                } else {
                    // all other cells

                    auto &westCell = Cells[cell_index - 1];
                    auto &thisCell = Cells[cell_index];
                    auto &eastCell = Cells[cell_index + 1];

                    Real64 FE1 = log(thisCell.radius_outer / thisCell.radius_center) / (2 * DataGlobals::Pi * thisCell.conductivity);
                    Real64 FE2 = log(eastCell.radius_center / eastCell.radius_inner) / (2 * DataGlobals::Pi * eastCell.conductivity);
                    Real64 AE = 1 / (FE1 + FE2);

                    Real64 FW1 = log(westCell.radius_outer / westCell.radius_center) / (2 * DataGlobals::Pi * westCell.conductivity);
                    Real64 FW2 = log(thisCell.radius_center / thisCell.radius_inner) / (2 * DataGlobals::Pi * thisCell.conductivity);
                    Real64 AW = -1 / (FW1 + FW2);

                    Real64 AD = thisCell.rhoCp * thisCell.vol / time_step;

                    a.push_back(-AW / AD);
                    b.push_back(AW / AD - AE / AD - 1);
                    c.push_back(AE / AD);
                    d.push_back(-thisCell.temperature_prev_ts);
                }
            } // end tdma setup

            // solve for new temperatures
            std::vector<Real64> new_temps = solveTDM(a, b, c, d);

            for (int cell_index = 0; cell_index < num_cells; ++cell_index) {
                Cells[cell_index].temperature = new_temps[cell_index];
            }

            // calculate bh wall temp
            Real64 T_bhWall = 0.0;
            for (int cell_index = 0; cell_index < num_cells; ++cell_index) {
                auto &leftCell = Cells[cell_index];
                auto &rightCell = Cells[cell_index + 1];

                if (leftCell.type == CellType::GROUT && rightCell.type == CellType::SOIL) {

                    Real64 left_conductance = 2 * DataGlobals::Pi * leftCell.conductivity / log(leftCell.radius_outer / leftCell.radius_inner);
                    Real64 right_conductance = 2 * DataGlobals::Pi * rightCell.conductivity / log(rightCell.radius_center / leftCell.radius_inner);

                    T_bhWall = (left_conductance * leftCell.temperature + right_conductance * rightCell.temperature) /
                               (left_conductance + right_conductance);
                    break;
                }
            }

            total_time += time_step;

            // computed at BH wall
            this->lnttsSTS.push_back(log(total_time / t_s));
            this->gSTS.push_back(this->c0 * ((T_bhWall - initial_temperature) / heat_flux));

            // uncomment to compute at center of BH
            // g.push_back(this->c0 * ((Cells[0].temperature - initial_temperature) / heat_flux - bhResistance));

        } // end timestep loop
    }

} // namespace GroundHeatExchangerEnhanced

} // namespace EnergyPlus
