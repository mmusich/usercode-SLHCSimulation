// -*- C++ -*-
//
// Package:    GeneralHLTOffline
// Class:      GeneralHLTOffline
// 
/**\class GeneralHLTOffline

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jason Michael Slaunwhite,512 1-008,`+41227670494,
//         Created:  Fri Aug  5 10:34:47 CEST 2011
// $Id: OccupancyPlotter.cc,v 1.10 2011/12/19 17:44:16 abrinke1 Exp $
//
//

// system include files
#include <memory>

// user include files
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "TMath.h"

#include "TStyle.h"
//
// class declaration
//

using namespace edm;
using namespace trigger;
using std::vector;
using std::string;


class GeneralHLTOffline : public edm::EDAnalyzer {
   public:
      explicit GeneralHLTOffline(const edm::ParameterSet&);
      ~GeneralHLTOffline();

  //static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void setupHltMatrix(std::string, int);
  virtual void fillHltMatrix(std::string, std::string, double, double, bool);

      // ----------member data ---------------------------


  bool debugPrint;
  bool outputPrint;

  std::string plotDirectoryName;

  DQMStore * dbe;

  HLTConfigProvider hltConfig_;

  vector< vector<string> > PDsVectorPathsVector;
  
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
GeneralHLTOffline::GeneralHLTOffline(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed

  debugPrint = false;
  outputPrint = false;

  if (debugPrint) std::cout << "Inside Constructor" << std::endl;

  plotDirectoryName = iConfig.getUntrackedParameter<std::string>("dirname", "HLT/General");

  if (debugPrint) std::cout << "Got plot dirname = " << plotDirectoryName << std::endl;


}


GeneralHLTOffline::~GeneralHLTOffline()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
GeneralHLTOffline::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using std::string;

   if (debugPrint) std::cout << "Inside analyze" << std::endl;

    // Access Trigger Results
   edm::Handle<edm::TriggerResults> triggerResults;
   iEvent.getByLabel(InputTag("TriggerResults","", "HLT"), triggerResults);
   
   if (!triggerResults.isValid()) {
     if (debugPrint) std::cout << "Trigger results not valid" << std::endl;
     return; }

    if (debugPrint) std::cout << "Found triggerResults" << std::endl;

   edm::Handle<trigger::TriggerEvent>         aodTriggerEvent;   
   iEvent.getByLabel(InputTag("hltTriggerSummaryAOD", "", "HLT"), aodTriggerEvent);
   
   if ( !aodTriggerEvent.isValid() ) { 
     if (debugPrint) std::cout << "No AOD trigger summary found! Returning..."; 
     return; 
   }

   const TriggerObjectCollection objects = aodTriggerEvent->getObjects();


   vector<string> datasetNames =  hltConfig_.streamContent("A");
// Loop over PDs
   for (unsigned int iPD = 0; iPD < datasetNames.size(); iPD++) { 

     //     if (datasetNames[iPD] != "SingleMu" && datasetNames[iPD] != "SingleElectron" && datasetNames[iPD] != "Jet") continue;  

     unsigned int keyTracker[1000]; // Array to eliminate double counts by tracking what Keys have already been fired
   for(unsigned int irreproduceableIterator = 0; irreproduceableIterator < 1000; irreproduceableIterator++) {
     keyTracker[irreproduceableIterator] = 1001;
   }
// Loop over Paths in each PD
   for (unsigned int iPath = 0; iPath < PDsVectorPathsVector[iPD].size(); iPath++) { //Andrew - where does PDsVectorPathsVector get defined?
     
     std::string pathName = PDsVectorPathsVector[iPD][iPath];

     if (debugPrint) std::cout << "Looking at path " << pathName << std::endl;
     
     unsigned int index = hltConfig_.triggerIndex(pathName);
     
     if (debugPrint) std::cout << "Index = " << index << " triggerResults->size() = " << triggerResults->size() << std::endl;

     if (index < triggerResults->size()) {
       if(triggerResults->accept(index)) {

         if (debugPrint) std::cout << "We fired path " << pathName << std::endl;

         // look up module labels for this path

         vector<std::string> modulesThisPath = hltConfig_.moduleLabels(pathName);

         if (debugPrint) std::cout << "Looping over module labels " << std::endl;

         // Loop backward through module names
         for ( int iModule = (modulesThisPath.size()-1); iModule >= 0; iModule--) {

           if (debugPrint) std::cout << "Module name is " << modulesThisPath[iModule] << std::endl;

           // check to see if you have savetags information
           if (hltConfig_.saveTags(modulesThisPath[iModule])) {

             if (debugPrint) std::cout << "For path " << pathName << " this module " << modulesThisPath[iModule] <<" is a saveTags module of type " << hltConfig_.moduleType(modulesThisPath[iModule]) << std::endl;

	     if (hltConfig_.moduleType(modulesThisPath[iModule]) == "HLTLevel1GTSeed") break;

             InputTag moduleWhoseResultsWeWant(modulesThisPath[iModule], "", "HLT");

             unsigned int indexOfModuleInAodTriggerEvent = aodTriggerEvent->filterIndex(moduleWhoseResultsWeWant);

             if ( indexOfModuleInAodTriggerEvent < aodTriggerEvent->sizeFilters() ) {
               const Keys &keys = aodTriggerEvent->filterKeys( indexOfModuleInAodTriggerEvent );
               if (debugPrint) std::cout << "Got Keys for index " << indexOfModuleInAodTriggerEvent <<", size of keys is " << keys.size() << std::endl;
               
               for ( size_t iKey = 0; iKey < keys.size(); iKey++ ) {
                 TriggerObject foundObject = objects[keys[iKey]];
		 bool first_count = false;

		 if(keyTracker[iKey] != iKey) first_count = true;
		 	
		 if (debugPrint || outputPrint) std::cout << "This object has id (pt, eta, phi) = "
						          << " " << foundObject.id() << " " 
							  << std::setw(10) << foundObject.pt()
							  << ", " << std::setw(10) << foundObject.eta() 
							  << ", " << std::setw(10) << foundObject.phi()
							  << "    for path = " << std::setw(20) << pathName
							  << " module " << std::setw(40) << modulesThisPath[iModule]
							  << " iKey " << iKey << std::endl;

		 fillHltMatrix(datasetNames[iPD],pathName,foundObject.eta(),foundObject.phi(),first_count);
		      
		 keyTracker[iKey] = iKey;
		 
               }// end for each key               
             }// end if filter in aodTriggerEvent
	     

             // OK, we found the last module. No need to look at the others.
             // get out of the loop

             break;
           }// end if saveTags
         }//end Loop backward through module names   
       }// end if(triggerResults->accept(index))
     }// end if (index < triggerResults->size())
   }// end Loop over Paths in each PD
   }//end Loop over PDs


   

}


// ------------ method called once each job just before starting event loop  ------------
void 
GeneralHLTOffline::beginJob()
{

  if (debugPrint) std::cout << "Inside begin job" << std::endl; 

  dbe = Service<DQMStore>().operator->();

  if (dbe) {

    dbe->setCurrentFolder(plotDirectoryName);

  }

}

// ------------ method called once each job just after ending the event loop  ------------
void 
GeneralHLTOffline::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
GeneralHLTOffline::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{

  if (debugPrint) std::cout << "Inside beginRun" << std::endl;

  bool changed = true;
  if (hltConfig_.init(iRun, iSetup, "HLT", changed)) {
    if(debugPrint)
      if(debugPrint) std::cout << "HLT config with process name " 
                << "HLT" << " successfully extracted" << std::endl;
  } else {
    if (debugPrint)
      if (debugPrint) std::cout << "Warning, didn't find process HLT" << std::endl;
  }

  vector<string> datasetNames =  hltConfig_.streamContent("A");
  for (unsigned int i=0;i<datasetNames.size();i++) {

    if (debugPrint) std::cout << "This is dataset " << datasetNames[i] <<std::endl;

    vector<string> datasetPaths = hltConfig_.datasetContent(datasetNames[i]);

    if (debugPrint) std::cout << "datasetPaths.size() = " << datasetPaths.size() << std::endl;

    PDsVectorPathsVector.push_back(datasetPaths);

    if (debugPrint) std::cout <<"Found PD: " << datasetNames[i]  << std::endl;     
    setupHltMatrix(datasetNames[i],i);   

  }// end of loop over dataset names

}// end of beginRun

// ------------ method called when ending the processing of a run  ------------
void GeneralHLTOffline::endRun(edm::Run const&, edm::EventSetup const&)
{
}

void GeneralHLTOffline::setupHltMatrix(std::string label, int iPD) {

std::string h_name;
std::string h_title;
std::string h_name_1dEta;
std::string h_name_1dPhi;
std::string h_title_1dEta;
std::string h_title_1dPhi;
std::string h_name_1dEtaPath;
std::string h_name_1dPhiPath;
std::string h_title_1dEtaPath;
std::string h_title_1dPhiPath;
std::string pathName;
std::string PD_Folder;
std::string Path_Folder;

PD_Folder = TString("HLT/GeneralHLTOffline");
if (label != "SingleMu" && label != "SingleElectron" && label != "Jet")  PD_Folder = TString("HLT/GeneralHLTOffline/"+label); 

dbe->setCurrentFolder(PD_Folder.c_str());

h_name = "HLT_"+label+"_EtaVsPhi";
h_title = "HLT_"+label+"_EtaVsPhi";
h_name_1dEta = "HLT_"+label+"_1dEta";
h_name_1dPhi = "HLT_"+label+"_1dPhi";
h_title_1dEta = label+" Occupancy Vs Eta";
h_title_1dPhi = label+" Occupancy Vs Phi";
//Int_t numBinsEta = 12;
//Int_t numBinsPhi = 8;
Int_t numBinsEta = 30;
Int_t numBinsPhi = 34;
Int_t numBinsEtaFine = 60;
Int_t numBinsPhiFine = 66;
Double_t EtaMax = 2.610;
Double_t PhiMax = 17.0*TMath::Pi()/16.0;
Double_t PhiMaxFine = 33.0*TMath::Pi()/32.0;

  //Double_t eta_bins[] = {-2.610,-2.349,-2.088,-1.827,-1.740,-1.653,-1.566,-1.479,-1.392,-1.305,-1.044,-0.783,-0.522,-0.261,0,0.261,0.522,0.783,1.044,1.305,1.392,1.479,1.566,1.653,1.740,1.827,2.088,2.349,2.610}; //Has narrower bins in Barrel/Endcap border region
//Double_t eta_bins[] = {-2.610,-2.175,-1.740,-1.305,-0.870,-0.435,0,0.435,0.870,1.305,1.740,2.175,2.610};
//Double_t phi_bins[] = {-TMath::Pi(),-3*TMath::Pi()/4,-TMath::Pi()/2,-TMath::Pi()/4,0,TMath::Pi()/4,TMath::Pi()/2,3*TMath::Pi()/4,TMath::Pi()};

 TH2F * hist_EtaVsPhi = new TH2F(h_name.c_str(),h_title.c_str(),numBinsEta,-EtaMax,EtaMax,numBinsPhi,-PhiMax,PhiMax);
 TH1F * hist_1dEta = new TH1F(h_name_1dEta.c_str(),h_title_1dEta.c_str(),numBinsEtaFine,-EtaMax,EtaMax);
 TH1F * hist_1dPhi = new TH1F(h_name_1dPhi.c_str(),h_title_1dPhi.c_str(),numBinsPhiFine,-PhiMaxFine,PhiMaxFine);

 hist_EtaVsPhi->SetMinimum(0);
 hist_1dEta->SetMinimum(0);
 hist_1dPhi->SetMinimum(0);

 // Do not comment out these pointers since it is the booking that is the work here. 
 MonitorElement * ME_EtaVsPhi = dbe->book2D(h_name.c_str(),hist_EtaVsPhi);
 MonitorElement * ME_1dEta = dbe->book1D(h_name_1dEta.c_str(),hist_1dEta);
 MonitorElement * ME_1dPhi = dbe->book1D(h_name_1dPhi.c_str(),hist_1dPhi);

  for (unsigned int iPath = 0; iPath < PDsVectorPathsVector[iPD].size(); iPath++) { 
    pathName = PDsVectorPathsVector[iPD][iPath];
    h_name_1dEtaPath = "HLT_"+pathName+"_1dEta";
    h_name_1dPhiPath = "HLT_"+pathName+"_1dPhi";
    h_title_1dEtaPath = pathName+" Occupancy Vs Eta";
    h_title_1dPhiPath = pathName+"Occupancy Vs Phi";
    Path_Folder = TString("HLT/GeneralHLTOffline/"+label+"/Paths");
    dbe->setCurrentFolder(Path_Folder.c_str());

    // Do not comment out these pointers since it is the booking that is the work here. 
     MonitorElement * ME_1dEta = dbe->book1D(h_name_1dEtaPath.c_str(),h_title_1dEtaPath.c_str(),numBinsEtaFine,-EtaMax,EtaMax);
     MonitorElement * ME_1dPhi = dbe->book1D(h_name_1dPhiPath.c_str(),h_title_1dPhiPath.c_str(),numBinsPhiFine,-PhiMaxFine,PhiMaxFine);
  
    if (debugPrint) std::cout << "book1D for " << pathName << std::endl;
  }
   
 if (debugPrint) std::cout << "Success setupHltMatrix( " << label << " , " << iPD << " )" << std::cout;
} //End setupHltMatrix

void GeneralHLTOffline::fillHltMatrix(std::string label, std::string path,double Eta, double Phi, bool first_count) {

  if (debugPrint) std::cout << "Inside fillHltMatrix( " << label << " , " << path << " ) " << std::endl;

  std::string fullPathToME;
  std::string fullPathToME1dEta;
  std::string fullPathToME1dPhi;
  std::string fullPathToME1dEtaPath;
  std::string fullPathToME1dPhiPath;

 fullPathToME = "HLT/GeneralHLTOffline/HLT_"+label+"_EtaVsPhi"; 
 fullPathToME1dEta = "HLT/GeneralHLTOffline/HLT_"+label+"_1dEta";
 fullPathToME1dPhi = "HLT/GeneralHLTOffline/HLT_"+label+"_1dPhi";

if (label != "SingleMu" && label != "SingleElectron" && label != "Jet") {
 fullPathToME = "HLT/GeneralHLTOffline/"+label+"/HLT_"+label+"_EtaVsPhi"; 
 fullPathToME1dEta = "HLT/GeneralHLTOffline/"+label+"/HLT_"+label+"_1dEta";
 fullPathToME1dPhi = "HLT/GeneralHLTOffline/"+label+"/HLT_"+label+"_1dPhi";
 }
 
 fullPathToME1dEtaPath = "HLT/GeneralHLTOffline/"+label+"/Paths/HLT_"+path+"_1dEta";
 fullPathToME1dPhiPath = "HLT/GeneralHLTOffline/"+label+"/Paths/HLT_"+path+"_1dPhi";

  if (debugPrint) std::cout << "fullPathToME = " << std::endl;

  MonitorElement * ME_2d = dbe->get(fullPathToME);
  MonitorElement * ME_1dEta = dbe->get(fullPathToME1dEta);
  MonitorElement * ME_1dPhi = dbe->get(fullPathToME1dPhi);  
  MonitorElement * ME_1dEtaPath = dbe->get(fullPathToME1dEtaPath);
  MonitorElement * ME_1dPhiPath = dbe->get(fullPathToME1dPhiPath);

  if (debugPrint) std::cout << "MonitorElement * " << std::endl;

  TH2F * hist_2d = ME_2d->getTH2F();
  TH1F * hist_1dEta = ME_1dEta->getTH1F();
  TH1F * hist_1dPhi = ME_1dPhi->getTH1F();
  TH1F * hist_1dEtaPath = ME_1dEtaPath->getTH1F();
  TH1F * hist_1dPhiPath = ME_1dPhiPath->getTH1F();

  if (debugPrint) std::cout << "TH2F *" << std::endl;

  //int i=2;
  //if (Eta>1.305 && Eta<1.872) i=0;
  //if (Eta<-1.305 && Eta>-1.872) i=0;
  //for (int ii=i; ii<3; ++ii) hist_2d->Fill(Eta,Phi); //Scales narrow bins in Barrel/Endcap border region

  if(first_count) {
    hist_1dEta->Fill(Eta);
    hist_1dPhi->Fill(Phi); 
    hist_2d->Fill(Eta,Phi); }
    hist_1dEtaPath->Fill(Eta); 
    hist_1dPhiPath->Fill(Phi);

 if (debugPrint) std::cout << "hist->Fill" << std::endl;

} //End fillHltMatrix

// ------------ method called when starting to processes a luminosity block  ------------
void GeneralHLTOffline::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
GeneralHLTOffline::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
// void
// GeneralHLTOffline::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
//   //The following says we do not know what parameters are allowed so do no validation
//   // Please change this to state exactly what you do use, even if it is no parameters
//   edm::ParameterSetDescription desc;
//   desc.setUnknown();
//   descriptions.addDefault(desc);
// }

//define this as a plug-in for online (not offline ?)
DEFINE_FWK_MODULE(GeneralHLTOffline);