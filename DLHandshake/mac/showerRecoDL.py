import ROOT
from larlite import larlite as fmwk
from recotool import showerreco
from ROOT import protoshower

def getShowerRecoAlgModular():
    # This function returns the default shower reco module
    # If you want to extend, customize, or otherwise alter the default
    # reco (which you should!) it's recommended that you do one of two
    # things:
    #   1) Copy this function, and change the lines you need
    #   2) Use this function as is and replace the modules with the built-in functions
    #       to ShowerRecoAlgModular
    # Then, use the return value (alg) in place of ShowerRecoAlg in your python script
    
    # It should be noted, however, that this "default" set of modules is not yet developed
    # and if you are developing you ought to be updating it here!
    
    alg = showerreco.ShowerRecoAlgModular()
    alg.SetDebug(False)
    alg.SetVerbose(False)

    # filteralgo = showerreco.FilterPFPart()
    # filteralgo.setMinNHitsAbsolute(5)
    # filteralgo.setMinNHitsLargest(10)
    
    # angle3D = showerreco.Angle3DFormula()
    # angle3D.setMaxAngleError(0.1)
    # angle3D.setValidateDirection(True)
    # angle3D.setVerbosity(True)

    angle3D = showerreco.Angle3DFromVtxQweighted()
    angle3D.setVerbosity(False)
    
    # startPt = showerreco.YPlaneStartPoint3D()
    startPt = showerreco.VertexIsStartPoint3D()
    startPt.setVerbosity(False)

    energy = showerreco.LinearEnergy()
    energy.setVerbosity(False)

    # implement position-dependent calibration
    energy.CreateResponseMap(20)
    dQdsAVG = 248.
    fin = open('/usr/local/share/dllee_unified/larlite/UserDev/RecoTool/ShowerReco3D/dqds_mc_xyz.txt','r')
    #fin = open('/home/vgenty/dqds_mc_xyz.txt','r')
    for line in fin:
        words = line.split()
        x = float(words[0])
        y = float(words[1])
        z = float(words[2])
        q = float(words[3])
        energy.SetResponse(x,y,z,dQdsAVG/q)

    energy.SetElectronLifetime(1e6) # in us DATA value
    energy.SetRecombFactor(0.62)
    #energy.SetElecGain(243.) # MCC8.0 data
    energy.SetElecGain(200.) # MCC8.0 value
    energy.setVerbosity(False)
    energy.SetFillTree(True)

    dqdx = showerreco.dQdxModule()
    dqdx.setTrunkLength(3.)

    # dqdx.SetFillTree(True)
    
    # shrFilter = showerreco.FilterShowers()
    # shrFilter.setAngleCut(15.)
    # shrFilter.setVerbosity(False)

    # alg.AddShowerRecoModule( filteralgo )
    alg.AddShowerRecoModule(angle3D)
    #alg.AddShowerRecoModule( showerreco.StartPoint3DModule() )
    #alg.AddShowerRecoModule( showerreco.NearestStartPoint3D() )

    # no longer needed... proto shower stage takes care
    alg.AddShowerRecoModule(startPt)
    alg.AddShowerRecoModule(dqdx)
    alg.AddShowerRecoModule(energy)
    alg.AddShowerRecoModule(showerreco.EstimateLength())

    #alg.AddShowerRecoModule( shrFilter )
    
    alg.PrintModuleList()
    
    return alg

def DefaultShowerReco3D(req_pdg):
    
    # Create analysis unit
    ana_unit = fmwk.ShowerReco3D()
    
    # require PDG == 11 for PFParticles (?)
    ana_unit.SetRequirePDG11(req_pdg)
    
    # Attach shower reco alg
    sralg = getShowerRecoAlgModular()
    ana_unit.AddShowerAlgo(sralg)

    return ana_unit

def DLShowerReco3D(req_pdg):
    suffix = "josh"
    shower_ana_unit=DefaultShowerReco3D(req_pdg)
    print "Load DefaultShowerReco3D @ ",shower_ana_unit

    # set ProtoShower Algo to go from data-products to a ProtoShower object
    protoshoweralg = protoshower.ProtoShowerAlgDL()
    protoshoweralg.SetVertexProducer("dl")
    shower_ana_unit.GetProtoShowerHelper().setProtoShowerAlg( protoshoweralg )
    
    shower_ana_unit.SetInputProducer("dl")
    shower_ana_unit.SetOutputProducer("showerreco")

    return shower_ana_unit


