#include "Pythia8/Pythia.h"
#include "Pythia8/HeavyIons.h"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TGraph.h"
	
#include "../lib/Colors.h"
#include "../lib/ProgressBar.h"
#include "../lib/ErrorHandler.h"

using namespace Pythia8;

struct
{
	float pt_min;
	float pt_max;
	
	float y_min;
	float y_max;
	
	float eta_min;
	float eta_max;
} Par;

bool IsCut(Pythia pythia, const int ipart)
{
	const float pt = pythia.event[ipart].pt();
	if (pt < Par.pt_min || pt > Par.pt_max) return true;
	
	const float y = pythia.event[ipart].rap();
	if (y < Par.y_min || y > Par.y_max) return true;
	
	const float eta = pythia.event[ipart].eta();
	if (eta < Par.eta_min || eta > Par.eta_max) return true;

	return false;
}

struct
{
	std::vector <float> min;
	std::vector <float> max;
} Pt;

struct
{
	std::vector<const int> id = {313};
} Part;
	
void RunCentr(const char *par)
{
	std::cout << "Current launch will only calulate spectra" << endl;
	
	std::string cmnd_file = "../data/pythia_par/" + par[1] + ".cmnd";
	CheckInputFile(cmnd_file.c_str());

	std::string centrality_file = "../data/centrality/" + par[1] + ".txt";
	CheckInputFile(centrality_file.c_str());
	
	Pythia pythia;
	pythia.readFile(cmnd_file.c_str());

	pythia.init();

	TH1F *spectra = new TH1F("spectra", "spectra", (pt.max - pt.min)*10, pt.min, pt.max);
	
	for (unsigned long int iEvent = 0; iEvent < nEvents; ++iEvent)
	{
		if (!pythia.next()) continue;
		
		const float weight = pythia.info.weight();

		for (int ipart = 0; ipart < pythia.event.size(); ++ipart)
		{
			if(IsCut(pythia, ipart)) continue;
			
			if (abs(pythia.event[ipart].id()) == 313)
			{
				for (int count = 0; count < pt.min.size(); count++)
				{
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
	
	delete output, ncharged_per_event_hist;
}




