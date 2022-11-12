#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TColor.h"
#include "TVirtualPad.h"
#include "../lib/ErrorHandler.h"

const float pi = 3.14159265359;

using namespace std;

class SpectraFromTXT
{

private:

	double max_y{0};
	double min_y{1e30};
	TCanvas *canv;
	TLegend *legend;
	TH1F *hr;

	TF1 *efficiency;

	vector <TGraphErrors*> graphs;

	TFile *output_file;

	vector <int> ignore_lines;	

	void SetGraphStyle(TGraphErrors* graph, Color_t color, const int marker_style) {

		graph->SetMarkerStyle(marker_style);
		graph->SetMarkerColor(color);
		graph->SetMarkerSize(4);
		graph->SetLineWidth(4);

		graph->SetLineColor(color);
	}

	void CheckMaximumY(double y_val)
	{
		if (y_val > max_y) max_y = y_val;
	}

	void CheckMinimumY(double y_val)
	{
		if (y_val < min_y && y_val > 0) min_y = y_val;
	}

	bool IsIgnoreLine(const int line)
	{
		for (int x: ignore_lines)
		{
			if (x == line)
			{	
				cout << "	Line " << line << " was ignored" << endl;
				return true;
			}
		}
		
		return false;
	}
	
	void ResetIgnoreLines()
	{
		ignore_lines.clear();
	}

public:

	void SetEfficiency(char * name, double par[10])
	{
		delete efficiency;
		
		efficiency = new TF1(name, name);
		efficiency->SetParameters(par);
	}
	
	void AddIgnoreLine(int line)
	{
		ignore_lines.push_back(line);
	}

	SpectraFromTXT(string title, const int min_x, const int max_x, string XaxisTitle, string YaxisTitle)
	{
		canv = new TCanvas(title.c_str(), title.c_str(), 800, 800);
		canv->SetLogy();
		hr = new TH1F("hr", "hr", 100, min_x, max_x); 
		
		gStyle->SetOptStat(0);

		hr->SetTitle(title.c_str());
		hr->GetXaxis()->SetLabelSize(0.025);
		hr->GetYaxis()->SetLabelSize(0.025);
		
		hr->GetXaxis()->SetTitle(XaxisTitle.c_str());
		hr->GetYaxis()->SetTitle(YaxisTitle.c_str());

		hr->GetXaxis()->SetTitleSize(0.025);
		hr->GetYaxis()->SetTitleSize(0.025);

		legend = new TLegend(0.75, 0.5, 0.98, 0.98);

		efficiency = new TF1("eff", "x*[0]");
	}

	void AddEntry(string input_file_name, string legend_entry, Color_t color, const int marker_style)
	{
		CheckInputFile(input_file_name);

		ifstream input_file(input_file_name.c_str());
		
		graphs.push_back(new TGraphErrors());

		float x_val, y_val, y_err;
	
		int count = 0;

		cout << "Reading from file " << input_file_name << ":" << endl;

		while (input_file >> x_val >> y_val >> y_err)
		{
			cout << "	" << x_val << " " << y_val << " " << y_err << endl;
			graphs.back()->AddPoint(x_val, y_val);
			graphs.back()->SetPointError(count, 0, y_err);
			count++;

			CheckMaximumY(y_val);
			CheckMinimumY(y_val);
		}

		SetGraphStyle(graphs.back(), color, marker_style);
		legend->AddEntry(graphs.back(), legend_entry.c_str(), "p");

		ResetIgnoreLines();
	} 

	void AddEntrySpectra(string input_file_name, string legend_entry, Color_t color, const int marker_style)
	{
		CheckInputFile(input_file_name);
		ifstream input_file(input_file_name.c_str());
		
		graphs.push_back(new TGraphErrors());

		float x_val, y_val, y_err, x_range;
	
		int count = 0;
		int lcount = 0;

		cout << "Reading from file " << input_file_name << ":" << endl;

		while (input_file >> x_val >> y_val >> y_err >> x_range)
		{
			if (IsIgnoreLine(lcount+1))
			{
				lcount++;
				continue;
			}
			cout << "	" << x_val << " " << y_val << " " << y_err << " " << x_range << endl;
			y_val /= pow((efficiency->Eval(x_val)), 2)*x_val*x_range*2.*pi*1e6*2.*0.225;
			
			graphs.back()->AddPoint(x_val, y_val);
			graphs.back()->SetPointError(count, 0, y_val*y_err/100.);
			
			count++;
			lcount++;

			CheckMaximumY(y_val);
			CheckMinimumY(y_val);
		}

		cout << endl;

		SetGraphStyle(graphs.back(), color, marker_style);
		legend->AddEntry(graphs.back(), legend_entry.c_str(), "p");

		ResetIgnoreLines();
	} 

	void WriteResults(string output_file_name, string draw_option = "p")
	{
		canv->cd();

		hr->SetMinimum(min_y/10);
		hr->SetMaximum(max_y*10);

		hr->Draw("AXIS");

		for (int count = 0; count < graphs.size(); count++)
		{
			graphs[count]->Draw(draw_option.c_str());
		}

		legend->Draw();
		
		canv->SaveAs(output_file_name.c_str());
	}
};
