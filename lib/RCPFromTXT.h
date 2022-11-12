#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "TF1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TColor.h"
#include "TVirtualPad.h"
#include "../lib/ErrorHandler.h"

using namespace std;

class RCPFromTXT
{

private:

	double max_y{0};
	double min_y{1e30};
	TCanvas *canv;
	TLegend *legend;
	TH1F *hr;

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

	bool IsIgnore(const int line)
	{
		for (int x: ignore_lines)
		{
			if (x == line) 
			{
				cout << "Line " << line << " was ignored" << endl;
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
	
	RCPFromTXT(string title, const float min_x, const float max_x, string XaxisTitle, string YaxisTitle)
	{
		canv = new TCanvas(title.c_str(), title.c_str(), 900, 900);
		
		hr = new TH1F("hr", "hr", 100, min_x, max_x); 
		
		gStyle->SetOptStat(0);

		hr->SetTitle(title.c_str());
		hr->GetXaxis()->SetLabelSize(0.025);
		hr->GetYaxis()->SetLabelSize(0.025);
		
		hr->GetXaxis()->SetTitle(XaxisTitle.c_str());
		hr->GetYaxis()->SetTitle(YaxisTitle.c_str());

		hr->GetXaxis()->SetTitleSize(0.025);
		hr->GetYaxis()->SetTitleSize(0.025);

		legend = new TLegend(0.7, 0.70, 0.98, 0.98);
	}
	
	void AddIgnoreLine(int line)
	{
		ignore_lines.push_back(line);
	}

	void AddEntry(string input_file_name, string legend_entry, Color_t color, const int marker_style)
	{
		CheckInputFile(input_file_name);

		ifstream input_file(input_file_name.c_str());
		
		graphs.push_back(new TGraphErrors());

		float x_val, y_val, y_err;
	
		int count = 0;
		
		cout << "From file: " << input_file_name << endl;
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
	
	void AddEntryAverage(string input_file_name, string legend_entry, Color_t color, const int marker_style)
	{
		CheckInputFile(input_file_name);

		ifstream input_file(input_file_name.c_str());
		
		graphs.push_back(new TGraphErrors());

		float x_val, y_val1, y_val2, y_err1, y_err2;
	
		int count = 0;
		
		cout << "From file: " << input_file_name << endl;
		while (input_file >> x_val >> y_val1 >> y_val2 >> y_err1 >> y_err2)
		{
			cout << "	" << x_val << " " << y_val1 << " " << y_val2 << " " << y_err1 << " " << y_err2 << endl;
			const float y_val = (y_val1 + y_val2);
			const float y_err = sqrt((y_err1+y_err2))*y_val/100.;
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

	void AddEntryRCP(string input_file_name1, string input_file_name2, string legend_entry, const float ncoll1, const float ncoll2, Color_t color, const int marker_style)
	{
		CheckInputFile(input_file_name1);
		CheckInputFile(input_file_name2);
		
		ifstream input_file1(input_file_name1.c_str());
		ifstream input_file2(input_file_name2.c_str());

		graphs.push_back(new TGraphErrors());

		float x_val1, y_val1, y_err1, x_range1;
		float x_val2, y_val2, y_err2, x_range2;

		int count = 0;
		int graph_count = 0;
		
		cout << "From files: " << input_file_name1 << " and " << input_file_name2 << endl;

		while (input_file1 >> x_val1 >> y_val1 >> y_err1 >> x_range1 && input_file2 >> x_val2 >> y_val2 >> y_err2 >> x_range2)
		{
			cout << "From file 1:" << " " << x_val1 << " " << y_val1 << " " << y_err1 << " " << x_range1 << endl;
			cout  << "From file 2:" << " " << x_val2 << " " << y_val2 << " " << y_err2 << " " << x_range2 << endl;
			if (IsIgnore(count+1))
			{
				count++;
				continue;
			}

			const float y_val = y_val1/y_val2*ncoll2/ncoll1*0.8/0.36;
			const float y_err = sqrt(y_err1*y_err1 + y_err2*y_err2)*y_val/100.;
			
			graphs.back()->AddPoint(x_val1, y_val);
			graphs.back()->SetPointError(graph_count, 0, y_err);

			count++;
			graph_count++;

			CheckMaximumY(y_val);
		}

		SetGraphStyle(graphs.back(), color, marker_style);
		legend->AddEntry(graphs.back(), legend_entry.c_str(), "p");

		ResetIgnoreLines();
	} 

	void WriteResults(string output_file_name, string draw_option = "p")
	{
		canv->cd();

		hr->SetMinimum(min_y/1.1);
		hr->SetMaximum(max_y*1.1);

		hr->Draw("AXIS");

		for (int count = 0; count < graphs.size(); count++)
		{
			graphs[count]->Draw(draw_option.c_str());
		}

		legend->Draw();
		
		canv->SaveAs(output_file_name.c_str());
	}
};
