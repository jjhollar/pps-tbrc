#include "TCanvas.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TH2.h"
#include "TStyle.h"

namespace DQM
{
  /**
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   * \date 25 Jul 2015
   */
  class GastofCanvas : public TCanvas
  {
    public:
      inline GastofCanvas() :
        TCanvas("null"), fLegend(0), fLegendX(.52), fLegendY(.76), fLegendNumEntries(0), fUpperLabel(0), fLabelsDrawn(false) {;}
      inline GastofCanvas(TString name, unsigned int width=500, unsigned int height=500, TString upper_label="") :
        TCanvas(name, "", width, height), fWidth(width), fHeight(height), fLegend(0), fLegendX(.52), fLegendY(.76), fLegendNumEntries(0),
        fUpperLabelText(upper_label), fUpperLabel(0), fLabelsDrawn(false) { Build(); }
      inline GastofCanvas(TString name, TString upper_label) :
        TCanvas(name, "", 500, 500), fWidth(500), fHeight(500), fLegend(0), fLegendX(.52), fLegendY(.76), fLegendNumEntries(0),
        fUpperLabelText(upper_label), fUpperLabel(0), fLabelsDrawn(false) { Build(); }
      inline virtual ~GastofCanvas() {
        if (fLegend) delete fLegend;
        if (fUpperLabel) delete fUpperLabel;
        if (fHist) delete fHist;
      }

      void SetUpperLabel(TString text) {
        fUpperLabelText = text;
        fUpperLabel = new TPaveText(.45, .922, .885, .952, "NDC");
        fUpperLabel->SetMargin(0.);
        fUpperLabel->SetFillColor(kWhite);
        fUpperLabel->SetLineColor(kWhite);
        fUpperLabel->SetLineWidth(0);
        fUpperLabel->SetShadowColor(kWhite);
        fUpperLabel->SetTextFont(43);
        fUpperLabel->SetTextAlign(33);
        fUpperLabel->SetTextSize(18);
        fUpperLabel->AddText(fUpperLabelText);
        fUpperLabel->Draw();
      }

      inline void FillChannel(unsigned short nino_id, unsigned short channel_id, double content) {
        const Coord c = GetCoordinates(nino_id, channel_id);
        fHist->Fill(c.x-0.5, c.y-0.5, content);
      }
      inline TH2D* Grid() { return fHist; }

      inline void Save(TString ext="png", TString path=".") {
        bool valid_ext = true;
        valid_ext |= (strcmp(ext, "png")!=0);
        valid_ext |= (strcmp(ext, "pdf")!=0);
        if (!valid_ext) return;
        DrawGrid();
        if (!fLabelsDrawn) {
          fLabel1 = new TPaveText(.112, .925, .2, .955, "NDC");
          fLabel1->AddText("GasToF");
          fLabel1->SetMargin(0.);
          fLabel1->SetFillColor(kWhite);
          fLabel1->SetLineColor(kWhite);
          fLabel1->SetLineWidth(0);
          fLabel1->SetShadowColor(kWhite);
          fLabel1->SetTextFont(63);
          fLabel1->SetTextAlign(13);
          fLabel1->SetTextSize(22);
          fLabel1->Draw();
          fLabel2 = new TPaveText(.29, .925, .36, .955, "NDC");
          fLabel2->AddText("TB2015");
          fLabel2->SetMargin(0.);
          fLabel2->SetFillColor(kWhite);
          fLabel2->SetLineColor(kWhite);
          fLabel2->SetLineWidth(0);
          fLabel2->SetShadowColor(kWhite);
          fLabel2->SetTextFont(43);
          fLabel2->SetTextAlign(13);
          fLabel2->SetTextSize(22);
          fLabel2->Draw();
          if (fLegend->GetNRows()!=0) fLegend->Draw();
          SetUpperLabel(fUpperLabelText);
          fLabelsDrawn = true;
        }
        TCanvas::SaveAs(Form("%s/%s.%s", path.Data(), TCanvas::GetName(), ext.Data()));
        c1->SetLogz();
        TCanvas::SaveAs(Form("%s/%s_logscale.%s", path.Data(), TCanvas::GetName(), ext.Data()));
      }

    private:
      inline void Build() {
        fLegend = new TLegend(fLegendX, fLegendY, fLegendX+.35, fLegendY+.12);
        fLegend->SetFillColor(kWhite);
        fLegend->SetLineColor(kWhite);
        fLegend->SetLineWidth(0);
        fLegend->SetTextFont(43);
        fLegend->SetTextSize(14);
    
        fHist = new TH2D(Form("hist_%s", TCanvas::GetName()), "", 8, 0.5, 8.5, 8, 0.5, 8.5);
      }
      inline void DrawGrid() {
        TCanvas::cd();
        gStyle->SetOptStat(0);

        TCanvas::Divide(1,2);
        c1 = (TPad*)TCanvas::GetPad(1);
        c2 = (TPad*)TCanvas::GetPad(2);
        c1->SetPad(0.,0.,1.,1.);
        c2->SetPad(0.,0.,1.,0.);
        c1->SetBottomMargin(0.1);
        c1->SetLeftMargin(0.1);
        c1->SetRightMargin(0.115);
        c1->SetTopMargin(0.1);
        TCanvas::cd(1);
        
        fHist->Draw("colz");

        fHist->SetMarkerStyle(20);
        fHist->SetMarkerSize(.87);
        fHist->SetTitleFont(43, "XYZ");
        fHist->SetTitleSize(22, "XYZ");
        //fHist->SetTitleOffset(2., "Y");
        fHist->SetLabelFont(43, "XYZ");
        fHist->SetLabelSize(24, "XY");
        fHist->SetLabelSize(15, "Z");
        fHist->SetTitleOffset(1.3, "Y");
            
        c1->SetTicks(1, 1);
        //c1->SetGrid(1, 1);
      }
      
      struct Coord { unsigned int x; unsigned int y; };
      inline Coord GetCoordinates(unsigned short nino_id, unsigned short channel_id) const {
        Coord out;
        out.x = out.y = 0;
        switch (nino_id) {
          case 0:
            switch (channel_id) {
              case 0:  out.x = 1, out.y = 1; break;
              case 1:  out.x = 2, out.y = 1; break;
              case 2:  out.x = 3, out.y = 1; break;
              case 3:  out.x = 4, out.y = 1; break;
              case 4:  out.x = 5, out.y = 1; break;
              case 5:  out.x = 6, out.y = 1; break;
              case 6:  out.x = 7, out.y = 1; break;
              case 7:  out.x = 8, out.y = 1; break;
              case 8:  out.x = 1, out.y = 2; break;
              case 9:  out.x = 2, out.y = 2; break;
              case 10: out.x = 7, out.y = 2; break;
              case 11: out.x = 8, out.y = 2; break;
              case 12: out.x = 1, out.y = 3; break;
              case 13: out.x = 8, out.y = 3; break;
              case 14: out.x = 1, out.y = 4; break;
              case 15: out.x = 8, out.y = 4; break;
              case 16: out.x = 1, out.y = 5; break;
              case 17: out.x = 8, out.y = 5; break;
              case 18: out.x = 1, out.y = 6; break;
              case 19: out.x = 8, out.y = 6; break;
              case 20: out.x = 1, out.y = 7; break;
              case 21: out.x = 2, out.y = 7; break;
              case 22: out.x = 7, out.y = 7; break;
              case 23: out.x = 8, out.y = 7; break;
              case 24: out.x = 1, out.y = 8; break;
              case 25: out.x = 2, out.y = 8; break;
              case 26: out.x = 3, out.y = 8; break;
              case 27: out.x = 4, out.y = 8; break;
              case 28: out.x = 5, out.y = 8; break;
              case 29: out.x = 6, out.y = 8; break;
              case 30: out.x = 7, out.y = 8; break;
              case 31: out.x = 8, out.y = 8; break;
            }
            break;
          case 1:
            switch (channel_id) {
              case 0:  out.x = 3, out.y = 2; break;
              case 1:  out.x = 4, out.y = 2; break;
              case 2:  out.x = 5, out.y = 2; break;
              case 3:  out.x = 6, out.y = 2; break;
              case 4:  out.x = 2, out.y = 3; break;
              case 5:  out.x = 3, out.y = 3; break;
              case 6:  out.x = 4, out.y = 3; break;
              case 7:  out.x = 5, out.y = 3; break;
              case 8:  out.x = 6, out.y = 3; break;
              case 9:  out.x = 7, out.y = 3; break;
              case 10: out.x = 2, out.y = 4; break;
              case 11: out.x = 3, out.y = 4; break;
              case 12: out.x = 4, out.y = 4; break;
              case 13: out.x = 5, out.y = 4; break;
              case 14: out.x = 6, out.y = 4; break;
              case 15: out.x = 7, out.y = 4; break;
              case 16: out.x = 2, out.y = 5; break;
              case 17: out.x = 3, out.y = 5; break;
              case 18: out.x = 4, out.y = 5; break;
              case 19: out.x = 5, out.y = 5; break;
              case 20: out.x = 6, out.y = 5; break;
              case 21: out.x = 7, out.y = 5; break;
              case 22: out.x = 2, out.y = 6; break;
              case 23: out.x = 3, out.y = 6; break;
              case 24: out.x = 4, out.y = 6; break;
              case 25: out.x = 5, out.y = 6; break;
              case 26: out.x = 6, out.y = 6; break;
              case 27: out.x = 7, out.y = 6; break;
              case 28: out.x = 3, out.y = 7; break;
              case 29: out.x = 4, out.y = 7; break;
              case 30: out.x = 5, out.y = 7; break;
              case 31: out.x = 6, out.y = 7; break;
            }
            break;
          default:
            throw Exception(__PRETTY_FUNCTION__, Form("Invalid NINO identifier fetched! NINO: %d", nino_id), JustWarning);
        }      
        return out;
      }

      TPad *c1, *c2;
      TH2D* fHist;
      double fWidth, fHeight;
      TLegend *fLegend;
      double fLegendX, fLegendY;
      unsigned int fLegendNumEntries;
      TPaveText *fLabel1, *fLabel2;
      TString fUpperLabelText;
      TPaveText *fUpperLabel;
      bool fLabelsDrawn;
  };
}
