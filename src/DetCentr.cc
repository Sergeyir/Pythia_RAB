#include <memory>

#include "Pythia8/Pythia.h"
#include "Pythia8/HeavyIons.h"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"
	
#include "../lib/Colors.h"
#include "../lib/ProgressBar.h"
#include "../lib/ErrorHandler.h"

using namespace Pythia8;

void CheckArguments(int argc, int arg_num)
{
	if (argc != arg_num + 1)
	{
		std::cout << OutputColor.bold_red;
		std::cout << "Error: ";
		std::cout << OutputColor.reset;
		std::cout << "Number of arguments expected: " << arg_num << "; number of arguments were provided : " << argc - 1 << std::endl;
		exit(1);
	}
}

int main(int argc, char **argv) {
		
	CheckArguments(argc, 1);
	
	std::string cmnd_file = "../data/pythia_par/" + static_cast<std::string>(argv[1]) + ".cmnd";
	CheckInputFile(cmnd_file.c_str());

	//std::string centrality_file = "../data/centrality/" + argv[1] + ".txt";
	//CheckOutputFile(centrality_file.c_str());
	
	Pythia pythia;
	pythia.readFile(cmnd_file.c_str());

	std::string outfile_name = "../data/ncharged/" + static_cast<std::string>(argv[1]) + ".root";
	CheckOutputFile(outfile_name.c_str());
	TFile output(outfile_name.c_str(), "RECREATE");

	TH1F ncharged_hist("ncharged", "ncharged", 10000, 0, 10000);

	pythia.init();

	const int nEvents = 1e5;

	for (int iEvent = 0; iEvent < nEvents; ++iEvent)
	{
		ProgressBar.Fancy(((float) iEvent/nEvents));
		
		if (!pythia.next()) continue;
		
		int ncharged = 0;
		float weight = pythia.info.weight();

		for (int i = 0; i < pythia.event.size(); ++i)
		{
			if (pythia.event[i].isFinal() && pythia.event[i].isCharged()) ncharged++;
		}
		ncharged_hist.Fill(ncharged, weight);
	}
	
	ncharged_hist.Write();

	return 0;
}
