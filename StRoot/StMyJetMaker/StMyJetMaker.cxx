#include "StMyJetMaker.h"

//C++ includes
#include <cassert>

//FastJet stuff
#include "FJ_includes.h"
//ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TFile.h"
//My StRoot includes
#include "StRoot/TStarEventClass/TStarArrays.h"
#include "StRoot/TStarEventClass/TStarJet.h"
#include "StRoot/TStarEventClass/TStarEvent.h"

using namespace fastjet;

using namespace std;

ClassImp(StMyJetMaker);

map<string, int> StMyJetMaker::fJetAlgorithm = {
    {"kt_algorithm", 0}, {"cambridge_algorithm", 1},
    {"antikt_algorithm", 2}, {"genkt_algorithm", 3},
    {"cambridge_for_passive_algorithm", 11}, {"genkt_for_passive_algorithm", 13},
    {"ee_kt_algorithm", 50}, {"ee_genkt_algorithm", 53},
    {"plugin_algorithm", 99}, {"undefined_jet_algorithm", 999}
};

map<string, int> StMyJetMaker::fJetRecombScheme = {
    {"E_scheme", 0}, {"pt_scheme", 1}, {"pt2_scheme", 2}, {"Et_scheme", 3},
    {"Et2_scheme", 4}, {"BIpt_scheme", 5}, {"BIpt2_scheme", 6}, 
    {"external_scheme", 99}
};

map<string, int> StMyJetMaker::fJetAreaType = {
    {"invalid_area", -1}, {"active_area", 0}, 
    {"active_area_explicit_ghosts", 1}, {"one_ghost_passive_area", 10},
    {"passive_area", 11}, {"voronoi_area", 20}
};

//User Info class to include non-kinematic details of the particles into the pseudojet objects...
class StMyJetMaker::StPseudoJetUserInfo : public TStarVector,
                            public PseudoJet::UserInfoBase {
public:
	StPseudoJetUserInfo(const TStarVector vec): TStarVector(vec) {}
    int getIndex() const {return TStarVector::index();}
    short getCharge() const {return TStarVector::charge();}
};

class StMyJetMaker::StPseudoJetContainer {
public:
    StPseudoJetContainer() {}
    ~StPseudoJetContainer() {}
    void add(const TStarVector vec) {
        fPseudoJets.push_back(PseudoJet(vec.px(), vec.py(), vec.pz(), vec.energy()));
        fPseudoJets.back().set_user_info(new StPseudoJetUserInfo(vec));
        //if(vec.ClassName()=="TStarGenTrack")isGenLevel = true;
    }
    void set(const vector<PseudoJet> vec) {
        clear();
        for(auto& v : vec) fPseudoJets.push_back(v);
    }
    PseudoJet& at(unsigned int i) {return fPseudoJets.at(i);}
    void clear() {fPseudoJets.clear();}
    vector<PseudoJet> getPseudoJets() {return fPseudoJets;}
    unsigned int numberOfPseudoJets() {return fPseudoJets.size();}

    PseudoJet& operator[](unsigned int i) {return fPseudoJets[i];}

    void print(){
        for(auto& v : fPseudoJets)
            cout<<"pt: "<<v.pt()<<" eta: "<<v.eta()<<" phi: "<<v.phi()<<" e: "<<v.e()<<endl;
    }

private:
    vector<PseudoJet> fPseudoJets;
};

StMyJetMaker::StMyJetMaker(string name, string output, bool dodebug): 
StMaker(name.c_str()){
    doDebug = dodebug;
    if(doDebug)cout<<"StMyJetMaker::StMyJetMaker()"<<endl;

    histoFileName = output;
    histoFileName.insert(histoFileName.find(".root"), ".hist");
}

StMyJetMaker::~StMyJetMaker(){
    if(jetDef) delete jetDef;
    if(areaSpec) delete areaSpec;
    if(areaDef) delete areaDef;

    if(clustSeq) delete clustSeq;
    if(clustSeqArea) delete clustSeqArea;

    if(bkgJetDef) delete bkgJetDef;
    if(bkgEstimator) delete bkgEstimator;
    if(bkgSubtractor) delete bkgSubtractor;  

    if(jetConstituents) delete jetConstituents;
    if(fullEvent) delete fullEvent;
    if(jets) delete jets;

   // if(tsJet) delete tsJet;
}

Int_t StMyJetMaker::Init(){
    if(doDebug){cout<<"StMyJetMaker::Init()"<<endl;}

    if(!isGenLevel)assert(TStarArrays::hasArray("jets"));
    else assert(TStarArrays::hasArray("genJets"));

    jetConstituents = new StPseudoJetContainer();

    if(doBkgEst && !useSameVectorForBkg){
        fullEvent = new StPseudoJetContainer();
    }

    jets = new StPseudoJetContainer();

    jetDef = new JetDefinition((JetAlgorithm)jetAlgorithm, R, (RecombinationScheme)recombScheme, Best);

    if(doDebug){cout<<"Jet definition set: \n"<<jetDef->description()<<endl;}

    if(doAreaCalc){
        areaSpec = new GhostedAreaSpec(ghostMaxRap);
        areaDef = new AreaDefinition((AreaType)areaType, *areaSpec);
        if(doDebug)cout<<"Area definition set: \n"<<areaDef->description()<<endl;
    }

    if(doBkgEst){
        bkgJetDef = new JetDefinition((JetAlgorithm)bkgJetAlgorithm, R, (RecombinationScheme)recombScheme, Best);
        if(doDebug)cout<<"Background jet definition set: \n"<<bkgJetDef->description()<<endl;

        Selector bkgSelector = SelectorAbsRapMax(MaxRap)*(!SelectorNHardest(2));

        bkgEstimator = new JetMedianBackgroundEstimator(bkgSelector, *bkgJetDef, *areaDef);
        if(doDebug)cout<<"Background estimator set: \n"<<bkgEstimator->description()<<endl;

        if(doBkgSub){
            bkgSubtractor = new contrib::ConstituentSubtractor();
            if(doDebug)cout<<"Background subtractor set: \n"<<bkgSubtractor->description()<<endl;
        }
    }

    declareHistograms();

    return kStOK;
}

Int_t StMyJetMaker::Finish(){
    cout<< "StMyJetMaker::Finish()"<<endl;

    writeHistograms();

    return kStOK;
}

Int_t StMyJetMaker::Make(){
    if(doDebug)cout<<"StMyJetMaker::Make()"<<endl;

    if(!TStarArrays::hasEvent())return kStOK;

    Wt = TStarArrays::getEvent()->weight();
    
    clusterAndStoreJets();

    return kStOK;
}

void StMyJetMaker::Clear(Option_t *option){
    if(doDebug)cout<<"StMyJetMaker::Clear()"<<endl;
    //cout<<"Clearing StMyJetMaker"<<endl;
    Wt = 1.0;
    jets->clear();
    jetConstituents->clear();
    if(!useSameVectorForBkg)fullEvent->clear();
    if(clustSeq){
        delete clustSeq;
        clustSeq = nullptr;
    }
    if(clustSeqArea){
        delete clustSeqArea;
        clustSeqArea = nullptr;
    }
}

void StMyJetMaker::addConstituentVector(const TStarVector& vec){
    string conClassName = vec.ClassName();
    if(conClassName == "TStarGenTrack")isGenLevel = true;
    if(!doFullJet && vec.charge() == 0) return;
    if(fullEvent){fullEvent->add(vec);}
    if(vec.pt() < jetConstituentMinPt) return;
    jetConstituents->add(vec);
}

void StMyJetMaker::setJetAlgorithm(string algo){
    auto algoFinder = fJetAlgorithm.find(algo);
    assert((void("invalid jet algorithm!"), algoFinder != fJetAlgorithm.end()));
    jetAlgorithm = algoFinder->second;   
}

void StMyJetMaker::setBkgJetAlgorithm(string algo){
    auto algoFinder = fJetAlgorithm.find(algo);
    assert((void("invalid jet algorithm!"), algoFinder != fJetAlgorithm.end()));
    bkgJetAlgorithm = algoFinder->second;   
}

void StMyJetMaker::setRecombScheme(string scheme){
    auto schemeFinder = fJetRecombScheme.find(scheme);
    assert((void("invalid recombination scheme!"), schemeFinder != fJetRecombScheme.end()));
    recombScheme = schemeFinder->second;   
}

void StMyJetMaker::setAreaType(string type){
    auto typeFinder = fJetAreaType.find(type);
    assert((void("invalid area type!"), typeFinder != fJetAreaType.end()));
    areaType = typeFinder->second;   
}

int StMyJetMaker::clusterAndStoreJets(){
    unsigned int nJets = clusterJets();

    if(nJets == 0) return kStOK;

    if(!isGenLevel)TStarArrays::getEvent()->setMaxJetPt(jets->at(0).pt());
    else TStarArrays::getEvent()->setMaxGenJetPt(jets->at(0).pt());

    for(unsigned int ijet = 0; ijet < nJets; ijet++){
        PseudoJet jet = jets->at(ijet);
        if(doDebug)cout<<"Jet: "<<ijet<<" pt: "<<jet.pt()<<" eta: "<<jet.eta()<<" phi: "<<jet.phi()<<endl;
        histos1D["hJetPt"]->Fill(jet.pt(), Wt);
        histos1D["hJetEta"]->Fill(jet.eta(), Wt);
        histos1D["hJetPhi"]->Fill(jet.phi(), Wt);
        histos1D["hJetMass"]->Fill(jet.m(), Wt);
        histos1D["hNConstituents"]->Fill(jet.constituents().size(), Wt);
        histos2D["h2JetEtavPhi"]->Fill(jet.eta(), jet.phi(), jet.pt()*Wt);
        histos2D["h2JetPtvMass"]->Fill(jet.pt(), jet.m()*Wt);

        TStarJet* tsJet = nullptr;
        if(isGenLevel)tsJet = TStarArrays::addGenJet();
        else tsJet = TStarArrays::addJet();
        tsJet->setIndex(ijet);
        tsJet->setPtEtaPhiE(jet.pt(), jet.eta(), jet.phi(), jet.e());
        if(jet.has_area()){
            histos1D["hJetArea"]->Fill(jet.area(), Wt);
            histos2D["h2JetPtvArea"]->Fill(jet.pt(), jet.area(), Wt);
            tsJet->setArea(jet.area(), jet.area_4vector().px(), jet.area_4vector().py(), jet.area_4vector().pz());
            if(bkgEstimator){
                tsJet->setLocalRho(bkgEstimator->rho(jet));
                tsJet->setLocalSigma(bkgEstimator->sigma(jet));
                if(bkgSubtractor)jet = (*bkgSubtractor)(jet);
            }
        }
        for(auto& con : jet.constituents()){
            if(doAreaCalc && con.is_pure_ghost())continue;
            TStarVector conUserInfo = con.user_info<TStarVector>();
            int conIndex = conUserInfo.index(); 
	        short conCharge = conUserInfo.charge();
            histos1D["hConstituentPt"]->Fill(con.pt(), Wt);
            histos1D["hConstituentEta"]->Fill(con.eta(), Wt);
            histos1D["hConstituentPhi"]->Fill(con.phi(), Wt);
            histos1D["hConstituentCharge"]->Fill(conCharge, Wt);
            double deta = con.eta() - jet.eta();
            double dphi = jet.delta_phi_to(con);
            histos2D["h2ConstitEtavPhi"]->Fill(deta, dphi, con.pt()*Wt);

            if(isGenLevel){
                TStarArrays::getVector("genTracks", conIndex)->setJetIndex(ijet);
            }else if(conCharge != 0){
                TStarArrays::getVector("tracks", conIndex)->setJetIndex(ijet);
            }else if(conCharge == 0){
                TStarArrays::getVector("towers", conIndex)->setJetIndex(ijet);
            }else{
                cout<<"Constituent type unknown "<<conCharge<<endl;
                continue;
            }
        }
    }
    return -1;
}

unsigned int StMyJetMaker::clusterJets(){
    unsigned int nJets = 0;

    Selector JetCuts = SelectorPtMin(jetPtMin)*SelectorAbsEtaMax(jetAbsEtaMax);

    if(doDebug){
        cout<<"Jet cuts: \n"<<JetCuts.description()<<endl;
        cout<<"constituents: "<<endl;
        jetConstituents->print();
    }

    if(areaDef){
        clustSeqArea = new ClusterSequenceArea(jetConstituents->getPseudoJets(), *jetDef, *areaDef);
        Selector no_ghost = !SelectorIsPureGhost();
        jets->set(sorted_by_pt(JetCuts(no_ghost(clustSeqArea->inclusive_jets(jetPtCSMin)))));
    }else{
        clustSeq = new ClusterSequence(jetConstituents->getPseudoJets(), *jetDef);
        jets->set(sorted_by_pt(JetCuts(clustSeq->inclusive_jets(jetPtCSMin))));
    }

    nJets = jets->numberOfPseudoJets();

    histos1D["hNJets"]->Fill(nJets);

    if(doBkgEst){
        if(useSameVectorForBkg)bkgEstimator->set_particles(jetConstituents->getPseudoJets());
        else bkgEstimator->set_particles(fullEvent->getPseudoJets());

        eventRho = bkgEstimator->rho();
        eventSigma = bkgEstimator->sigma();

        TStarArrays::getEvent()->setRho(eventRho);
        TStarArrays::getEvent()->setSigma(eventSigma);
        
        if(doBkgSub){
            bkgSubtractor->set_background_estimator(bkgEstimator);
            bkgSubtractor->set_common_bge_for_rho_and_rhom(true);
        }
    }

    if(doDebug)cout<<"***************** Done clustering jets ! *******************"<<endl;

    if(doDebug){
        jetConstituents->print();
        cout<<"***************** Done clearing vectors ! *******************"<<endl;
    }

    return nJets;
}

TStarJet* StMyJetMaker::getJet(unsigned int ijet){
    assert(ijet < jets->numberOfPseudoJets());

    PseudoJet jet = jets->at(ijet);

    if(doDebug)cout<<"Jet: "<<ijet<<" pt: "<<jet.pt()<<" eta: "<<jet.eta()<<" phi: "<<jet.phi()<<endl;

    TStarJet* tsJet = new TStarJet(ijet, jet.pt(), jet.eta(), jet.phi(), jet.e());

    if(jet.has_area()){
        tsJet->setArea(jet.area(), jet.area_4vector().px(), jet.area_4vector().py(), jet.area_4vector().pz());
        if(bkgEstimator){
            tsJet->setLocalRho(bkgEstimator->rho(jet));
            tsJet->setLocalSigma(bkgEstimator->sigma(jet));
            if(bkgSubtractor)jet = (*bkgSubtractor)(jet);
        }
    }
    
    for(auto& con : jet.constituents()){
        if(doAreaCalc && con.is_pure_ghost())continue;
        int conIndex = con.user_info<StPseudoJetUserInfo>().getIndex();
        //short conCharge = con.user_info<StPseudoJetUserInfo>().getCharge();
        if(doDebug)cout<<"Constituent: "<<conIndex<<" pt: "<<con.pt()<<" eta: "<<con.eta()<<" phi: "<<con.phi()<<endl;
    }
    return tsJet;
}

double StMyJetMaker::getDeltaR(double eta1, double phi1, double eta2, double phi2){
    double deta = fabs(eta1 - eta2);
    double dphi = fabs(phi1 - phi2);
    if(dphi > TMath::Pi()) dphi = 2*TMath::Pi() - dphi;
    return sqrt(deta*deta + dphi*dphi);
}

double StMyJetMaker::getDeltaR(PseudoJet& jet1, PseudoJet& jet2){
    return getDeltaR(jet1.eta(), jet1.phi(), jet2.eta(), jet2.phi());
}

void StMyJetMaker::writeHistograms(){
    if(histoFileName != ""){
        histOut = new TFile(histoFileName.c_str(), "UPDATE");
        histOut->cd();
        histOut->mkdir(GetName());
        histOut->cd(GetName());
        for(const auto& hist : histos1D){
            hist.second->Write();
            if(doDebug)cout<<"Wrote "<<hist.first<<endl;
        }
        for(const auto& hist : histos2D){
            hist.second->Write();
            if(doDebug)cout<<"Wrote "<<hist.first<<endl;
        }
        histOut->cd();
        histOut->Write();
        histOut->Close();
        if(doDebug)cout<<"Wrote histograms to "<<histoFileName<<endl;
    }
}

void StMyJetMaker::declareHistograms(){
    double jetPtMin = 0, jetPtMax = 60; int jetPtBins = 60;
    double jetEtaMin = -1, jetEtaMax = 1; int jetEtaBins = 20;
    double jetPhiMin = 0, jetPhiMax = 2*TMath::Pi(); int jetPhiBins = 20;
    double jetMassMin = 0, jetMassMax = 1; int jetMassBins = 20;
    double jetAreaMin = 0, jetAreaMax = 1; int jetAreaBins = 20;
    double jetConstituentPtMin = 0, jetConstituentPtMax = 30; int jetConstituentPtBins = 30;
    double jetConstituentEtaMin = -1, jetConstituentEtaMax = 1; int jetConstituentEtaBins = 40;
    double jetConstituentPhiMin = 0, jetConstituentPhiMax = 2*TMath::Pi(); int jetConstituentPhiBins = 130;

    histos1D["hNJets"] = new TH1F("hNJets", "Number of Jets", 11, -0.5, 10.5);
    histos1D["hJetPt"] = new TH1F("hJetPt", "Jet Pt", jetPtBins, jetPtMin, jetPtMax);
    histos1D["hJetEta"] = new TH1F("hJetEta", "Jet Eta", jetEtaBins, jetEtaMin, jetEtaMax);
    histos1D["hJetPhi"] = new TH1F("hJetPhi", "Jet Phi", jetPhiBins, jetPhiMin, jetPhiMax);
    histos1D["hJetArea"] = new TH1F("hJetArea", "Jet Area", jetAreaBins, jetAreaMin, jetAreaMax); 
    histos1D["hJetMass"] = new TH1F("hJetMass", "Jet Mass", jetMassBins, jetMassMin, jetMassMax);
    histos1D["hNConstituents"] = new TH1F("hNConstituents", "Number of Constituents", 21, -0.5, 20.5);

    histos2D["h2JetPtvArea"] = new TH2F("h2JetPtvArea", "Jet Pt vs Area", 
                                    jetPtBins, jetPtMin, jetPtMax, jetAreaBins, jetAreaMin, jetAreaMax);
    histos2D["h2JetPtvMass"] = new TH2F("h2JetPtvMass", "Jet Pt vs Mass", 
                                    jetPtBins, jetPtMin, jetPtMax, jetMassBins, jetMassMin, jetMassMax);
    histos2D["h2JetPtvNConstituents"] = new TH2F("h2JetPtvNConstituents", "Jet Pt vs Number of Constituents", 
                                    jetPtBins, jetPtMin, jetPtMax, 21, -0.5, 20.5);
    histos2D["h2JetEtavPhi"] = new TH2F("h2JetEtavPhi", "Jet Eta vs Phi", 
                                    jetEtaBins, jetEtaMin, jetEtaMax, jetPhiBins, jetPhiMin, jetPhiMax);

    histos1D["hConstituentPt"] = new TH1F("hConstituentPt", "Constituent Pt", 
                                    jetConstituentPtBins, jetConstituentPtMin, jetConstituentPtMax);
    histos1D["hConstituentEta"] = new TH1F("hConstituentEta", "Constituent Eta", 
                                    jetConstituentEtaBins, jetConstituentEtaMin, jetConstituentEtaMax);
    histos1D["hConstituentPhi"] = new TH1F("hConstituentPhi", "ConstituentPhi", 
                                    jetConstituentPhiBins, jetConstituentPhiMin, jetConstituentPhiMax);
    histos1D["hConstituentCharge"] = new TH1F("hConstituentCharge", "Constituent Charge", 3, -1.5, 1.5);

    histos2D["h2ConstitEtavPhi"] = new TH2F("h2ConstitEtavPhi", "Constit #Delta Eta vs #Delta Phi", 40, -1, 1, 40, -1, 1);

    for(const auto& hist : histos1D){
        hist.second->Sumw2();
    }
    for(const auto& hist : histos2D){
        hist.second->Sumw2();
    }
    if(doDebug)cout<<"StMyAnalysisMaker::initHistos() - DONE"<<endl;
}
