#include "Pythia8/Pythia.h"
//#include "Pythia8/HeavyIons.h"

#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TGraph.h"

using namespace Pythia8;

int main(const unsigned int argc, const char *num[]) {

	//std::array <const unsigned int, 6> centr_det = {200, 85, 41, 17, 5, 2}; //centality ranges for CuAu 200 GeV
	std::array <const unsigned int, 6> centr_det = {200, 0, 0, 0, 0, 0}; //centrality ranges for pp since pp has no centrality

	std::array <const float, 14> pt_min = {0.0, 0.0, 1.4, 1.7, 1.9, 2.1, 2.3, 2.6, 2.9, 3.4, 4.0, 4.5, 5.0, 6.5};
	std::array <const float, 14> pt_max = {8.0, 1.4, 1.7, 1.9, 2.1, 2.3, 2.6, 2.9, 3.4, 4.0, 4.5, 5.0, 6.5, 8.0};

	std::string name = "../data/RAB/pythiaPP200_Kstar";

	TH2F *nkstars_hist = new TH2F("nkstars", "nkstars", 14, 0, 14, 5, 0, 5);
	TH2F *nantikstars_hist = new TH2F("nantikstars", "nantikstars", 14, 0, 14, 5, 0, 5);

	const long long unsigned int nEvents = 1e7;

	std::array <double, 14> nkstars, nantikstars;
		
	for (int count = 0; count < pt_min.size(); count ++) {
		nkstars[count] = 0;
		nantikstars[count] = 0;
	}

	TH1D *ncharged_hist;
	TFile *output;

	std::cout << "This is the process " << num[1] << std::endl;
	
	std::string set_seed("Random:seed = ");
	set_seed += num[1];
	
	Pythia pythia;
	
	//pythia.readString("Beams:idA = 1000290630");
	//pythia.readString("Beams:idB = 1000791970");
	
	pythia.readString("Beams:eCM = 200.");
	//pythia.readString("PhaseSpace:pTHatMin = 0.");
	pythia.readString("HardQCD:all = on");
	
	//pythia.readString("HeavyIon:SigFitDefPar = 9.82,1.69,0.29,0.0,0.0,0.0,0.0,0.0");
	//pythia.readString("HeavyIon:SigFitNGen = 0");
	
	pythia.readString("Random:setSeed = on");
	pythia.readString(set_seed.c_str());
	
	pythia.init();
	
	unsigned int nbins = 200;
	
	ncharged_hist = new TH1D("ncharged", "ncharged", nbins, 0, nbins);
	
	for (unsigned long int iEvent = 0; iEvent < nEvents; ++iEvent) {
	
		unsigned int ncharged = 0;

		if (iEvent % 10000 == 0) {
			cout << "Events has passed: " << iEvent << endl;
			//system("sensors");
		}

		if (!pythia.next()) continue;
		
		double weight = pythia.info.weight();

	for (int ipart = 0; ipart < pythia.event.size(); ++ipart) {
		
		if (pythia.event[ipart].isFinal() && pythia.event[ipart].isCharged()) {
			
				double eta = pythia.event[ipart].eta();
				if (eta > 3.1 && eta < 4) ncharged++;

			}

		int d1 = pythia.event[ipart].daughter1();
		int d2 = pythia.event[ipart].daughter2();
		
		if (abs(pythia.event[ipart].id()) == 313 && (((abs(pythia.event[d1].id()) == 211 && abs(pythia.event[d2].id()) == 321)) || (abs(pythia.event[d1].id()) == 321 && abs(pythia.event[d2].id())))) {

			for (int count = 0; count < pt_min.size(); count++) {
				
				if (pythia.event[ipart].pT() > pt_min[count] && pythia.event[ipart].pT() < pt_max[count] && pythia.event[ipart].id() > 0) {nkstars[count] += 1;}
				else if (pythia.event[ipart].pT() > pt_min[count] && pythia.event[ipart].pT() < pt_max[count] && pythia.event[ipart].id() < 0) {nantikstars[count] += 1;}

			}
		}
	}

		for (int count = 0; count < centr_det.size() - 1; count++) {

			if (ncharged < centr_det[count] && ncharged > centr_det[count+1]) {	

				for (int pt_count = 0; pt_count < pt_min.size(); pt_count++) {
				
					nkstars_hist->Fill(pt_count, count, weight*nkstars[pt_count]*1e6/nEvents);
					nantikstars_hist->Fill(pt_count, count, weight*nantikstars[pt_count]*1e6/nEvents);
				}

			}

		}

		ncharged_hist->Fill(ncharged. weight);
		
		}
	
	std::cout << "kstars: {";
		
	for (int count = 0; count < pt_min.size(); count ++) {
		
		nkstars[count] = nkstars[count]*1e6/nEvents;

		std::cout << nkstars[count] << ", "; 

	}

	std::cout << "}" << endl;

	std::cout << "anti kstars: {";
		
	for (int count = 0; count < pt_min.size(); count ++) {
		
		nantikstars[count] = nantikstars[count]*1e6/nEvents;

		std::cout << nantikstars[count] << ", "; 

	}

	std::cout << "}" << endl;

	name.append("_");
	name += num[1];
	name.append(".root");
	
	output = new TFile(name.c_str(), "RECREATE");
	ncharged_hist->Write();
	nkstars_hist->Write();
	nantikstars_hist->Write();
	
	pythia.stat();
	delete output, ncharged_per_event_hist;
	
	return 0;
	
}
