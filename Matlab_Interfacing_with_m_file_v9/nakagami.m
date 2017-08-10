function WLAN=nakagami(scale,shape)
h=ProbDistUnivParam('nakagami',[scale,shape]);
i=random(h,1);
WLAN=i;

