#include <TH1F.h>
#include <TROOT.h>
#include <TFile.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <iostream>
#include <TStyle.h>
#include <TPaveText.h>
#include <THStack.h>
#include <TF1.h>
#include <TSystem.h>
#include <TGraphErrors.h>

#include <RooRealVar.h>
#include <RooArgList.h>
#include <RooChebychev.h>
#include <RooDataHist.h>
#include <RooExtendPdf.h>
#include <RooWorkspace.h>
#include <RooPlot.h>

double H_mass=125.0;
double mH_diff_cut=40.;
double mH_mean_cut=20.;

double rebin=1;
bool useRatioFit=false;
bool bReg=false;

std::string tags="TLTL_nominal"; // MMMM

double VR_lo=220.;
double VR_hi=600.;
double SR_lo=250.;
double SR_hi=600.;

double quad(double a, double b, double c=0, double d=0, double e=0, double f=0)
{
  return pow(a*a+b*b+c*c+d*d+e*e+f*f, 0.5);
}

TCanvas* comparePlots(TH1F *data, TH1F *qcd, std::string title)
{
  TCanvas *c=new TCanvas(("c"+title).c_str(), "c", 600, 700);
  TPad *p_2=new TPad("p_2", "p_2", 0, 0, 1, 0.3);
  TPad *p_1=new TPad("p_1", "p_1", 0, 0.3, 1, 1);
  p_1->Draw();
  p_2->Draw();
  p_1->cd();
  // p_1->SetLogy();
  qcd->SetTitle((title+"; m_{X} (GeV)").c_str());
  double s=data->Integral(data->FindBin(200.), data->FindBin(1200.))/qcd->Integral(qcd->FindBin(200.), qcd->FindBin(1200.));
  qcd->Scale(s);
  qcd->Draw("HIST");
  data->Draw("Ep9 SAME");
  TLegend *leg=new TLegend(0.5, 0.9, 0.9, 0.7);
  leg->AddEntry(qcd, "Sideband Region");
  leg->AddEntry(data, "Central Region");
  leg->Draw();
  p_2->cd();
  p_2->SetGridy();
  TH1F *h_ratio=(TH1F*)data->Clone("h_ratio");
  h_ratio->SetTitle(("Data/MC Ratio "+title+" ; data/MC").c_str());
  h_ratio->Divide(qcd);                          
  h_ratio->SetMinimum(-1.); h_ratio->SetMaximum(3.);                  
  h_ratio->Draw();                                                    
  qcd->Scale(1./s); 
  p_1->cd(); 
  return c;                           
}

TCanvas* comparePlots2(RooPlot *plot_bC, RooPlot *plot_bS, TH1F *data, TH1F *qcd, std::string title)
{
  TCanvas *c=new TCanvas(("c_RooFit_"+title).c_str(), "c", 1000, 1000);
  TPad *p_2=new TPad("p_2", "p_2", 0, 0, 1, 0.3);
  TPad *p_1=new TPad("p_1", "p_1", 0, 0.3, 1, 1);
  p_1->Draw();
  p_2->Draw();
  p_1->cd();
  plot_bC->Draw();
  plot_bS->Draw("same");
  p_2->cd();
  p_2->SetGridy();
  TH1F *h_ratio=(TH1F*)data->Clone("h_ratio");
  h_ratio->SetTitle(("VR/VR-SB Ratio "+title+" ; VR/VR-SB Ratio").c_str());
  h_ratio->Divide(qcd); 
  h_ratio->GetXaxis()->SetRangeUser(VR_lo, VR_hi);                         
  h_ratio->SetMinimum(-1.); h_ratio->SetMaximum(3.);                  
  h_ratio->Draw();
  p_1->cd(); 
  return c;                           
} 

void BackgroundPrediction_Kinematic_GaussExp()
{

  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(000000000);
  
  if (bReg) tags=tags+"_bReg";
  
  const unsigned int nPoints=4;
  double mass[nPoints]={90., 107.5, 142.5, 160.};
  double n_SB[nPoints], n_SR[nPoints];
  double ratio[nPoints];
  double errorsY[nPoints], errorsX[nPoints];
  double ratioAt125=-1, errorAt125=-1;
  
  // TFile *f_MMMM_a=new TFile((tags+"/a/Histograms_8TeVData2012BCD_Skim.root").c_str());
  // TFile *f_MMMM_a=new TFile((tags+"/a/Histograms_BJetPlusX_Run2012BCD_Skim.root").c_str());
  TFile *f_MMMM_a=new TFile((tags+"/a_KinFit/Histograms_BJetPlusX_Run2012BCD_Skim.root").c_str());
  // TFile *f_MMMM_a=new TFile((tags+"/a_KinFit/Histograms_BJetPlusX_Run2012BCD_Skim_selected_bTagged_.root").c_str());
  TH1F *h_mX_CR2_a=(TH1F*)f_MMMM_a->Get("h_mX_CR2");
  TH1F *h_mX_CR4_a=(TH1F*)f_MMMM_a->Get("h_mX_CR4");
  TH1F *h_mX_SR_a=(TH1F*)f_MMMM_a->Get("h_mX_SR");
  ratioAt125=h_mX_SR_a->GetSumOfWeights()/(h_mX_CR2_a->GetSumOfWeights()+h_mX_CR4_a->GetSumOfWeights());
  
  // === MMMM/b ===
  // TFile *f_MMMM_b=new TFile((tags+"/b/Histograms_8TeVData2012BCD_Skim.root").c_str());
  // TFile *f_MMMM_b=new TFile((tags+"/b/Histograms_BJetPlusX_Run2012BCD_Skim.root").c_str());
  TFile *f_MMMM_b=new TFile((tags+"/b_KinFit/Histograms_BJetPlusX_Run2012BCD_Skim.root").c_str());
  // TFile *f_MMMM_b=new TFile((tags+"/b_KinFit/Histograms_BJetPlusX_Run2012BCD_Skim_selected_bTagged_.root").c_str());
  TH1F *h_mX_CR2_b=(TH1F*)f_MMMM_b->Get("h_mX_CR2");
  TH1F *h_mX_CR4_b=(TH1F*)f_MMMM_b->Get("h_mX_CR4");
  TH1F *h_mX_SR_b=(TH1F*)f_MMMM_b->Get("h_mX_SR");
  n_SB[0]=(h_mX_CR2_b->GetSumOfWeights()+h_mX_CR4_b->GetSumOfWeights());
  n_SR[0]=h_mX_SR_b->GetSumOfWeights();
  
  if (useRatioFit)
  {
    // === MMMM/d ===
    // TFile *f_MMMM_d=new TFile((tags+"/d/Histograms_8TeVData2012BCD_Skim.root").c_str());
    TFile *f_MMMM_d=new TFile((tags+"/d/Histograms_BJetPlusX_Run2012BCD_Skim.root").c_str());
    // TFile *f_MMMM_d=new TFile((tags+"/d/Histograms_BJetPlusX_Run2012BCD_Skim_selected_bTagged_.root").c_str());
    TH1F *h_mX_CR2_d=(TH1F*)f_MMMM_d->Get("h_mX_CR2");
    TH1F *h_mX_CR4_d=(TH1F*)f_MMMM_d->Get("h_mX_CR4");
    TH1F *h_mX_SR_d=(TH1F*)f_MMMM_d->Get("h_mX_SR");
    n_SB[1]=(h_mX_CR2_d->GetSumOfWeights()+h_mX_CR4_d->GetSumOfWeights());
    n_SR[1]=h_mX_SR_d->GetSumOfWeights();
  
    // === MMMM/e ===
    // TFile *f_MMMM_e=new TFile((tags+"/e/Histograms_8TeVData2012BCD_Skim.root").c_str());
    TFile *f_MMMM_e=new TFile((tags+"/e/Histograms_BJetPlusX_Run2012BCD_Skim.root").c_str());
    // TFile *f_MMMM_e=new TFile((tags+"/e/Histograms_BJetPlusX_Run2012BCD_Skim_selected_bTagged_.root").c_str());
    TH1F *h_mX_CR2_e=(TH1F*)f_MMMM_e->Get("h_mX_CR2");
    TH1F *h_mX_CR4_e=(TH1F*)f_MMMM_e->Get("h_mX_CR4");
    TH1F *h_mX_SR_e=(TH1F*)f_MMMM_e->Get("h_mX_SR");
    n_SB[2]=(h_mX_CR2_e->GetSumOfWeights()+h_mX_CR4_e->GetSumOfWeights());
    n_SR[2]=h_mX_SR_e->GetSumOfWeights();
  
    // === MMMM/c ===
    // TFile *f_MMMM_c=new TFile((tags+"/c/Histograms_8TeVData2012BCD_Skim.root").c_str());
    TFile *f_MMMM_c=new TFile((tags+"/c/Histograms_BJetPlusX_Run2012BCD_Skim.root").c_str());
    // TFile *f_MMMM_c=new TFile((tags+"/c/Histograms_BJetPlusX_Run2012BCD_Skim_selected_bTagged_.root").c_str());
    TH1F *h_mX_CR2_c=(TH1F*)f_MMMM_c->Get("h_mX_CR2");
    TH1F *h_mX_CR4_c=(TH1F*)f_MMMM_c->Get("h_mX_CR4");
    TH1F *h_mX_SR_c=(TH1F*)f_MMMM_c->Get("h_mX_SR");
    n_SB[3]=(h_mX_CR2_c->GetSumOfWeights()+h_mX_CR4_c->GetSumOfWeights());
    n_SR[3]=h_mX_SR_c->GetSumOfWeights();
  
    for (unsigned int i=0; i<nPoints; ++i)
    {
      ratio[i]=n_SR[i]/n_SB[i];
      errorsY[i]=ratio[i]*pow(1./n_SR[i]+1./n_SB[i], 0.5);
      errorsX[i]=0.;
    }
  
    TGraphErrors *g_ratio=new TGraphErrors(nPoints, mass, ratio, errorsX, errorsY);
    g_ratio->SetTitle("SR/SB ratio");
    TCanvas *c_ratio=new TCanvas("c_ratio", "c_ratio", 700, 700);
    g_ratio->SetMinimum(0.); g_ratio->SetMaximum(2.);
    g_ratio->Draw("A*");
    TF1 *f_ratio=new TF1("f_ratio", "pol1");
    g_ratio->Fit(f_ratio);
    c_ratio->SaveAs(("c_ratio_"+tags+".png").c_str());
  
    ratioAt125=f_ratio->Eval(125.);
    errorAt125=(errorsY[0]+errorsY[1]+errorsY[2]+errorsY[3])/4.;
  }
  
  std::cout<<"ratioAt125 = "<<ratioAt125<<" +- "<<errorAt125<<std::endl;
  
  std::cout<<" = MMMM b ======================================== "<<std::endl;
  TH1F *h_mMMMMb_3Tag_CR2=(TH1F*)f_MMMM_b->Get("h_mX_CR2");
  TH1F *h_mMMMMb_3Tag_CR4=(TH1F*)f_MMMM_b->Get("h_mX_CR4");
  TH1F *h_mMMMMb_3Tag_SR=(TH1F*)f_MMMM_b->Get("h_mX_SR");
  double bS=h_mMMMMb_3Tag_SR->GetSumOfWeights();
  std::cout<<"Number of events in MMMM b signal region = "<<bS<<std::endl;
  TH1F *h_mMMMMb_3Tag_CR24=(TH1F*)h_mMMMMb_3Tag_CR2->Clone("h_mX_CR24");
  h_mMMMMb_3Tag_CR24->Add(h_mMMMMb_3Tag_CR4);
  h_mMMMMb_3Tag_CR24->Rebin(rebin);
  h_mMMMMb_3Tag_SR->Rebin(rebin);
  h_mMMMMb_3Tag_CR24->SetLineColor(kRed);
  h_mMMMMb_3Tag_SR->SetLineColor(kBlue);
  double bC=h_mMMMMb_3Tag_CR24->GetSumOfWeights();
  std::cout<<"bC = "<<bC<<", bS = "<<bS<<std::endl;
  // Fit both MMMMb curves to Crystal Balls and compute Kolmogorov
  h_mMMMMb_3Tag_CR24->SetMaximum(h_mMMMMb_3Tag_CR24->GetMaximum()*1.3);
  h_mMMMMb_3Tag_CR24->SetTitle(("Kinematic Extrapolation in "+tags+" Validation Region; m_{X} GeV").c_str());
  h_mMMMMb_3Tag_SR->Scale(bC/bS);
  // Do the fits using RooFit
  gSystem->Load("../PDFs/GaussExp_cxx.so");
  RooRealVar x("x", "m_{X} (GeV)", VR_lo-100., VR_hi+100.);
  // bC
  RooRealVar bC_p0("bC_p0", "bC_p0", 300., 500.);
  RooRealVar bC_p1("bC_p1", "bC_p1", 40., 100.1);
  RooRealVar bC_p2("bC_p2", "bC_p2", 0.1, 10.1);
  GaussExp bC_fit("bC_fit", "bC Fit", x, bC_p0, bC_p1, bC_p2);
  h_mMMMMb_3Tag_CR24->GetXaxis()->SetRangeUser(VR_lo-100., VR_hi+100.);
  RooDataHist bC_data("bC_data", "bC Data", RooArgList(x), h_mMMMMb_3Tag_CR24);
  bC_fit.fitTo(bC_data, RooFit::Range(VR_lo, VR_hi));
  RooPlot *bC_plot=x.frame();
  bC_data.plotOn(bC_plot, RooFit::MarkerColor(kRed));
  bC_fit.plotOn(bC_plot, RooFit::LineColor(kRed));
  // bS
  RooRealVar bS_p0("bS_p0", "bS_p0", 300., 500.);
  RooRealVar bS_p1("bS_p1", "bS_p1", 40., 100.1);
  RooRealVar bS_p2("bS_p2", "bS_p2", 0.1, 10.1);
  GaussExp bS_fit("bS_fit", "bS Fit", x, bS_p0, bS_p1, bS_p2);
  h_mMMMMb_3Tag_SR->GetXaxis()->SetRangeUser(VR_lo-100., VR_hi+100.);
  RooDataHist bS_data("bS_data", "bS Data", RooArgList(x), h_mMMMMb_3Tag_SR);
  bS_fit.fitTo(bS_data, RooFit::Range(VR_lo, VR_hi)); // RooFit::SumW2Error(kTRUE), 
  RooPlot *bS_plot=x.frame();
  bS_data.plotOn(bS_plot, RooFit::MarkerColor(kBlue));
  bS_fit.plotOn(bS_plot, RooFit::LineColor(kBlue));
  std::cout<<" === === "<<std::endl;
  std::cout<<"chi^2/ndof of bC = "<<bC_plot->chiSquare()<<std::endl;
  std::cout<<"chi^2/ndof of bS = "<<bS_plot->chiSquare()<<std::endl;
  std::cout<<" === === "<<std::endl;
  TCanvas *c_bC=comparePlots2(bC_plot, bS_plot, h_mMMMMb_3Tag_SR, h_mMMMMb_3Tag_CR24, "Kinematic Extrapolation in "+tags+" Validation Region of Data; m_{X} GeV");
  double x_mean_bC=bC_p0.getVal();
  double x_k_bC=bC_p0.getVal()+bC_p2.getVal()*bC_p1.getVal();
  TLine *l_mean_bC=new TLine(x_mean_bC, 0, x_mean_bC, h_mMMMMb_3Tag_CR24->GetMaximum()*0.8); l_mean_bC->SetLineColor(kRed); l_mean_bC->Draw();
  TLine *l_k_bC=new TLine(x_k_bC, 0, x_k_bC, h_mMMMMb_3Tag_CR24->GetMaximum()*0.8); l_k_bC->SetLineColor(kRed); l_k_bC->SetLineStyle(9); l_k_bC->Draw();
  double x_mean_bS=bS_p0.getVal();
  double x_k_bS=bS_p0.getVal()+bS_p2.getVal()*bS_p1.getVal();
  TLine *l_mean_bS=new TLine(x_mean_bS, 0, x_mean_bS, h_mMMMMb_3Tag_SR->GetMaximum()); l_mean_bS->SetLineColor(kBlue); l_mean_bS->Draw();
  TLine *l_k_bS=new TLine(x_k_bS, 0, x_k_bS, h_mMMMMb_3Tag_SR->GetMaximum()); l_k_bS->SetLineColor(kBlue); l_k_bS->SetLineStyle(9); l_k_bS->Draw();
  c_bC->SaveAs(("c_compareData_"+tags+"_VR_RooFit_GaussExp.png").c_str());
  
  // Calculate Pi and DPi and dPi -- for shape systematics
  double PbC_0=bC_p0.getVal();
  double PbC_1=bC_p1.getVal();
  double PbC_2=bC_p2.getVal();
  double dPbC_0=bC_p0.getError();
  double dPbC_1=bC_p1.getError();
  double dPbC_2=bC_p2.getError();
  double PbS_0=bS_p0.getVal();
  double PbS_1=bS_p1.getVal();
  double PbS_2=bS_p2.getVal();
  double dPbS_0=bS_p0.getError();
  double dPbS_1=bS_p1.getError();
  double dPbS_2=bS_p2.getError();
  double DPb_0=PbS_0-PbC_0;
  double DPb_1=PbS_1-PbC_1;
  double DPb_2=PbS_2-PbC_2;
  double dPb_0=quad(dPbC_0, dPbS_0);
  double dPb_1=quad(dPbC_1, dPbS_1);
  double dPb_2=quad(dPbC_2, dPbS_2);
  
  std::cout<<" = MMMM Background Prediction ==== "<<std::endl;
  TH1F *h_mMMMMa_3Tag_CR2=(TH1F*)f_MMMM_a->Get("h_mX_CR2");
  TH1F *h_mMMMMa_3Tag_CR4=(TH1F*)f_MMMM_a->Get("h_mX_CR4");
  TH1F *h_mMMMMa_3Tag_SR;
  if (tags!="MMMM") h_mMMMMa_3Tag_SR=(TH1F*)f_MMMM_a->Get("h_mX_SR");
  TH1F *h_mMMMMa_3Tag_CR24=(TH1F*)h_mMMMMa_3Tag_CR2->Clone("h_mX_CR24");
  h_mMMMMa_3Tag_CR24->Add(h_mMMMMa_3Tag_CR4);
  h_mMMMMa_3Tag_CR24->Rebin(rebin);
  h_mMMMMa_3Tag_CR24->SetLineColor(kBlack);
  if (tags!="MMMM") h_mMMMMa_3Tag_SR->Rebin(rebin);
  if (tags!="MMMM") h_mMMMMa_3Tag_SR->SetLineColor(kBlue);
  TH1F *h_mMMMMa_3Tag_SR_Prediction=(TH1F*)h_mMMMMa_3Tag_CR24->Clone("h_mMMMMa_3Tag_SR_Prediction");
  double aC=h_mMMMMa_3Tag_CR24->GetSumOfWeights();
  // Get the scale of the prediction right
  std::cout<<"bS/bC = "<<bS/bC<<std::endl;
  std::cout<<"ratioAt125 = "<<ratioAt125<<", +- "<<errorAt125<<" (fract unc.) = "<<1.+errorAt125/ratioAt125<<std::endl;
  // h_mMMMMa_3Tag_SR_Prediction->Scale(ratioAt125);
  std::cout<<"Number of predicted events in 18.6 /fb = "<<h_mMMMMa_3Tag_SR_Prediction->GetSumOfWeights()*ratioAt125<<std::endl;
  // RooFit fit to background prediction
  RooRealVar x("x", "m_{X} (GeV)", SR_lo-100., SR_hi+100.);
  RooRealVar bg_p0("bg_p0", "bg_p0", 290., 390.);
  RooRealVar bg_p1("bg_p1", "bg_p1", 50., 100.1);
  RooRealVar bg_p2("bg_p2", "bg_p2", 0.1, 10.1);
  GaussExp bg("bg", "Background Prediction PDF", x, bg_p0, bg_p1, bg_p2);
  RooDataHist pred("pred", "Prediction from SB", RooArgList(x), h_mMMMMa_3Tag_SR_Prediction);
  bg.fitTo(pred, RooFit::Range(SR_lo, SR_hi));
  RooPlot *aC_plot=x.frame();
  pred.plotOn(aC_plot, RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
  bg.plotOn(aC_plot, RooFit::LineColor(kRed));
  TCanvas *c_rooFit=new TCanvas("c_rooFit", "c_rooFit", 1000, 700);
  aC_plot->Draw();
  double x_mean_aC=bg_p0.getVal();
  double x_k_aC=bg_p0.getVal()+bg_p2.getVal()*bg_p1.getVal();
  TLine *l_mean_aC=new TLine(x_mean_aC, 0, x_mean_aC, h_mMMMMa_3Tag_SR_Prediction->GetMaximum()); l_mean_aC->SetLineColor(kRed); l_mean_aC->Draw();
  TLine *l_k_aC=new TLine(x_k_aC, 0, x_k_aC, h_mMMMMa_3Tag_SR_Prediction->GetMaximum()); l_k_aC->SetLineColor(kRed); l_k_aC->SetLineStyle(9); l_k_aC->Draw();
  
  c_RooFit->Draw();
  return;
  
  // Prediction Curve with Shape Systematics
  double PaC_0=bg_p0.getVal();
  double PaC_1=bg_p1.getVal();
  double PaC_2=bg_p2.getVal();
  double dPaC_0=bg_p0.getError();
  double dPaC_1=bg_p1.getError();
  double dPaC_2=bg_p2.getError();
  double PaS_0=PaC_0*PbS_0/PbC_0;
  double PaS_1=PaC_1*PbS_1/PbC_1;
  double PaS_2=PaC_2*PbS_2/PbC_2;
  double dPaS_0=PaS_0*quad((dPaC_0/PaC_0), (dPbS_0/PbS_0), (dPbC_0/PbC_0));
  double dPaS_1=PaS_1*quad((dPaC_1/PaC_1), (dPbS_1/PbS_1), (dPbC_1/PbC_1));
  double dPaS_2=PaS_2*quad((dPaC_2/PaC_2), (dPbS_2/PbS_2), (dPbC_2/PbC_2));
  std::cout<<"(dPaC_0/PaC_0) = ("<<dPaC_0<<"/"<<PaC_0<<") = "<<(dPaC_0/PaC_0)<<"; (dPbS_0/PbS_0) = ("<<dPbS_0<<"/"<<PbS_0<<") = "<<(dPbS_0/PbS_0)<<"; (dPbC_0/PbC_0) = ("<<dPbC_0<<"/"<<PbC_0<<") = "<<(dPbC_0/PbC_0)<<std::endl;
  std::cout<<"(dPaC_1/PaC_1) = ("<<dPaC_1<<"/"<<PaC_1<<") = "<<(dPaC_1/PaC_1)<<"; (dPbS_1/PbS_1) = ("<<dPbS_1<<"/"<<PbS_1<<") = "<<(dPbS_1/PbS_1)<<"; (dPbC_1/PbC_1) = ("<<dPbC_1<<"/"<<PbC_1<<") = "<<(dPbC_1/PbC_1)<<std::endl; 
  std::cout<<"(dPaC_2/PaC_2) = ("<<dPaC_2<<"/"<<PaC_2<<") = "<<(dPaC_2/PaC_2)<<"; (dPbS_2/PbS_2) = ("<<dPbS_2<<"/"<<PbS_2<<") = "<<(dPbS_2/PbS_2)<<"; (dPbC_2/PbC_2) = ("<<dPbC_2<<"/"<<PbC_2<<") = "<<(dPbC_2/PbC_2)<<std::endl; 
  std::cout<<" Predicted PaS_0 = "<<PaS_0<<" +- "<<dPaS_0<<std::endl;
  std::cout<<" Predicted PaS_1 = "<<PaS_1<<" +- "<<dPaS_1<<std::endl;
  std::cout<<" Predicted PaS_2 = "<<PaS_2<<" +- "<<dPaS_2<<std::endl;
  RooRealVar bg_pred0;
  RooRealVar bg_pred1;
  RooRealVar bg_pred2;
  if (tags!="TLTL_nominal")
  {
    bg_pred0=new RooRealVar("bg_pred0", "bg_pred0", PaS_0-dPaS_0/2., PaS_0+dPaS_0/2.);
    bg_pred1=new RooRealVar("bg_pred1", "bg_pred1", PaS_1-dPaS_1/2., PaS_1+dPaS_1/2.);
    bg_pred2=new RooRealVar("bg_pred2", "bg_pred2", PaS_2-dPaS_2/2., PaS_2+dPaS_2/2.);
  }
  else
  {
    bg_pred0=new RooRealVar("bg_pred0", "bg_pred0", PaS_0);  bg_pred0.setError(dPaS_0);
    bg_pred1=new RooRealVar("bg_pred1", "bg_pred1", PaS_1);  bg_pred1.setError(dPaS_1);
    bg_pred2=new RooRealVar("bg_pred2", "bg_pred2", PaS_2);  bg_pred2.setError(dPaS_2);
  }
  GaussExp bg_pred_init("background_init", "Background Predicted for Signal Region", x, bg_pred0, bg_pred1, bg_pred2);
  GaussExp bg_pred("background", "Background Predicted for Signal Region", x, bg_pred0, bg_pred1, bg_pred2);
  RooPlot *aS_plot=x.frame();
  if (tags!="TLTL_nominal")
  {
    RooDataHist unblind("unblind", "Signal Region", RooArgList(x), h_mMMMMa_3Tag_SR);
    unblind.plotOn(aS_plot, RooFit::LineColor(kBlue), RooFit::MarkerColor(kBlue));
    bg_pred_init.plotOn(aS_plot, RooFit::LineColor(kGreen), RooFit::Range(SR_lo, SR_hi));
    bg_pred.fitTo(unblind, RooFit::Range(SR_lo, SR_hi));
    bg_pred.plotOn(aS_plot, RooFit::LineColor(kBlue));
    aS_plot->Draw();
    aC_plot->Draw("same");
  }
  else
  {
    bg_pred.plotOn(aC_plot, RooFit::LineColor(kGreen), RooFit::Range(SR_lo, SR_hi));
    aC_plot->Draw("same");
  }
  double x_mean_aS=bg_pred0.getVal();
  double x_k_aS=bg_pred0.getVal()+bg_pred2.getVal()*bg_pred1.getVal();
  TLine *l_mean_aS=new TLine(x_mean_aS, 0, x_mean_aS, h_mMMMMa_3Tag_SR_Prediction->GetMaximum()); l_mean_aS->SetLineColor(kGreen); l_mean_aS->Draw();
  TLine *l_k_aS=new TLine(x_k_aS, 0, x_k_aS, h_mMMMMa_3Tag_SR_Prediction->GetMaximum()); l_k_aS->SetLineColor(kGreen); l_k_aS->SetLineStyle(9); l_k_aS->Draw();
  
  std::cout<<" === === "<<std::endl;
  std::cout<<"chi^2/ndof of bC = "<<bC_plot->chiSquare()<<std::endl;
  std::cout<<"chi^2/ndof of bS = "<<bS_plot->chiSquare()<<std::endl;
  std::cout<<"chi^2/ndof of aC = "<<aC_plot->chiSquare()<<std::endl;
  std::cout<<"chi^2/ndof of aS = "<<aS_plot->chiSquare()<<std::endl;
  std::cout<<" === === "<<std::endl;
   
  c_rooFit->SaveAs(("c_compareData_"+tags+"_SR_RooFit_GaussExp.png").c_str());
  
  RooWorkspace *w=new RooWorkspace("HbbHbb");
  w->import(bg_pred);
  w->SaveAs("w_background_GaussExp.root");
  
}
  
  
  
