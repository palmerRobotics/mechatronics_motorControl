close all
Fs = 5000;
dt = 1/Fs;
stopTime = .5;
t = 0:dt:stopTime-dt;

s = 1*sin(2*pi*200*t) + 1*sin(2*pi*5000*t) + 1*sin(2*pi*2000*t);

LPcutoff = 400;

WnL = LPcutoff/(Fs/2);

bLP = fir1(2,WnL,'low')

LPfilter = filter(bLP,1,s);

hold on
plotFFT(s)
plotFFT(LPfilter)
hold off

%[A,B] = butter(2, 400/5000)
