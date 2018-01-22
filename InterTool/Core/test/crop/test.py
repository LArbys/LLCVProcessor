import ROOT
from larcv import larcv
from ROOT import llcv

im = llcv.InterImageManager()
im.set_verbosity(0)

adc_img = larcv.imread_gray("adc_000.jpg")
trk_img = larcv.imread_gray("trk_000.jpg")
shr_img = larcv.imread_gray("shr_000.jpg")

adc_img2d_v = ROOT.std.vector("larcv::Image2D")(3)
trk_img2d_v = ROOT.std.vector("larcv::Image2D")(3)
shr_img2d_v = ROOT.std.vector("larcv::Image2D")(3)

im._vtx_pixel_v.resize(3);

for plane in xrange(3):
    print "@plane=%d"%plane
    adc_img2d_v[plane] = adc_img
    trk_img2d_v[plane] = trk_img
    shr_img2d_v[plane] = shr_img
    im._vtx_pixel_v[plane] = ROOT.std.pair("int","int")(367,282)
    
print 
print "set adc image"
im.SetImage(adc_img2d_v,llcv.kADC)
print "...done"
print "set trk image"
im.SetImage(trk_img2d_v,llcv.kTRK)
print "...done"
print "set shr image"
im.SetImage(shr_img2d_v,llcv.kSHR)
print "...done"
print

cropx = int(100)
cropy = int(100)

ocv_img_v = im.OCVImage(llcv.kADC,cropx,cropy)

from ROOT import geo2d
pygeo = geo2d.PyDraw()

import cv2
cv2.imwrite("in0.png",larcv.as_ndarray(adc_img2d_v.front()))
cv2.imwrite("out0.png",pygeo.image(ocv_img_v.front()))









