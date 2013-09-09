// -*- C++ -*-
//
// Package:    TightMuonProducer
// Class:      TightMuonProducer
// 
/**\class TightMuonProducer TightMuonProducer.cc AuuCode/TightMuonProducer/src/TightMuonProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Ernesto Migliore,13 2-017,+41227672059,
//         Created:  Wed Jun 20 12:27:19 CEST 2012
// $Id$
//
//


// system include files
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
//
// class declaration
//

class TightMuonProducer : public edm::EDProducer {
public:
  explicit TightMuonProducer(const edm::ParameterSet&);
  ~TightMuonProducer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:
  virtual void beginJob() ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  virtual void beginRun(edm::Run&, edm::EventSetup const&);
  virtual void endRun(edm::Run&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
  
  // ----------member data ---------------------------
  edm::InputTag muonCollectionTag_;
  edm::InputTag vertexCollectionTag_;
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
TightMuonProducer::TightMuonProducer(const edm::ParameterSet& iConfig):
muonCollectionTag_(iConfig.getParameter<edm::InputTag>("muonSrc")),
vertexCollectionTag_(iConfig.getParameter<edm::InputTag>("vertexSrc"))
{
   //register your products
/* Examples
   produces<ExampleData2>();

   //if do put with a label
   produces<ExampleData2>("label");
 
   //if you want to put into the Run
   produces<ExampleData2,InRun>();
*/
  //now do what ever other initializati on is needed
  produces<std::vector<reco::Muon> >();  
}


TightMuonProducer::~TightMuonProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
TightMuonProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
/* This is an event example
   //Read 'ExampleData' from the Event
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);

   //Use the ExampleData to create an ExampleData2 which 
   // is put into the Event
   std::auto_ptr<ExampleData2> pOut(new ExampleData2(*pIn));
   iEvent.put(pOut);
*/

/* this is an EventSetup example
   //Read SetupData from the SetupRecord in the EventSetup
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
*/
 
   edm::Handle<std::vector<reco::Muon> > muons;
   iEvent.getByLabel(muonCollectionTag_,muons);

   edm::Handle<std::vector<reco::Vertex> > vtx;
   iEvent.getByLabel(vertexCollectionTag_, vtx);
   const reco::Vertex pv = vtx.product()->operator[](0);

   std::vector<reco::Muon> tightMuons;
   for(std::vector<reco::Muon>::const_iterator recomuon_it=muons->begin(); recomuon_it!=muons->end(); ++recomuon_it){



//     std::cout <<  " ++++++++++++++++++++++++++++++++" << std::endl;
//     if (recomuon_it->isPFMuon()) std::cout << "isPF()" << std::endl;
//     if (recomuon_it->isGlobalMuon()) std::cout << "isGlobal()" << std::endl;
//     if ( recomuon_it->isPFMuon() && recomuon_it->isGlobalMuon() ) {
//     std::cout <<  "muon.numberOfMatchedStations() "  <<  recomuon_it->numberOfMatchedStations() << std::endl;
//     std::cout <<  "muon.innerTrack()->hitPattern().trackerLayersWithMeasurement() "  <<  recomuon_it->innerTrack()->hitPattern().trackerLayersWithMeasurement() << std::endl;
//     std::cout <<  "muon.innerTrack()->hitPattern().numberOfValidPixelHits() "  <<  recomuon_it->innerTrack()->hitPattern().numberOfValidPixelHits()  << std::endl;
//     std::cout <<  "muon.muonBestTrack()->dxy(vtx.position()) "  <<  recomuon_it->muonBestTrack()->dxy(pv.position()) << std::endl;
//     std::cout <<  "muon.muonBestTrack()->dz(vtx.position()) "  <<  recomuon_it->muonBestTrack()->dz(pv.position()) << std::endl;
//     }
//     std::cout <<  " ++++++++++++++++++++++++++++++++" << std::endl;


      if ( muon::isTightMuon(*recomuon_it,pv)  ){ 
       tightMuons.push_back(*recomuon_it);
     }
   }      
   // the output
   std::auto_ptr<std::vector<reco::Muon> > tightMuonCollection( new std::vector<reco::Muon> (tightMuons) );
   iEvent.put(tightMuonCollection);
}

// ------------ method called once each job just before starting event loop  ------------
void 
TightMuonProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TightMuonProducer::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
TightMuonProducer::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
TightMuonProducer::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
TightMuonProducer::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
TightMuonProducer::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TightMuonProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TightMuonProducer);
