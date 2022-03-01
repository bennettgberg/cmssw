import ROOT

f = ROOT.TFile("pcc_Data_PixVtx_Event_90X.root")

t = f.Get("lumi/tree")

h = ROOT.TH1F("h", "h", 80, 0., 20.)

for i,e in enumerate(t):
    if i > 10000: break
    if i % 100 == 0: print(i)
    #for j in e.hfcale:
    for j in e.adc:     
        h.Fill(j)

h.Draw()
raw_input("press enter to exit")
