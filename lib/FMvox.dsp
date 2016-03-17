declare name "FMVox";
import("filter.lib");
ts  = 1 << 16;
fs  = float(ts);
ts1 = ts+1;
ct  = (+(1) ̃_ ) - 1;
fct = float(ct);
sc  = fct
*
(2
*
PI)/fs:sin;
sm = fct
*
(2
*
PI)/fs:sin:/(2
*
PI);
dec(x) = x-floor(x);
pha(f) = f/float(SR):(+:dec)  ̃ _;
tbl(t,p)= s1+dec(f)
*
(s2-s1)
with {
f = p
*
fs;
i = int(f);
s1 = rdtable(ts1,t,i);
s2 = rdtable(ts1,t,i+1); };
fupho(f0,a,b,c) = (even+odd):
*
(a)
with {
cf  = c/f0;
ci = floor(cf);
ci1 = ci+1;
isEven= if((fmod(ci,2)<1),1,0);
ef  = if(isEven,ci,ci1);
of  = if(isEven,ci1,ci);
frac = cf-ci;
comp = 1-frac;
oa  = if(isEven,frac,comp);
ea  = if(isEven,comp,frac);
ph  = pha(f0);
m  = tbl(sm,ph):
*
(b);
even = ea:
*
(tbl(sc,(dec(ef
*
ph))+m));
odd = oa:
*
(tbl(sc,(dec(of
*
ph))+m));};
frame(c) = (w  ̃ _ )
with {
rst(y)= if(c,-y,1);
w(x)  = x+rst(x); };
demux(i,ctr,x) = coef
with {
trig = (ctr==i);
coef  = (
*
(1-trig)+x
*
trig)  ̃ _;};
formant(f_num,ctlStream) = fsig
with {
ctr = frame(ctlStream<0);
co(i) = demux(i,ctr,ctlStream);
f0  = 1;
a  = f0+1+f_num
*
3;
b  = a+1;
c  = a+2;
fsig  = fupho(co(f0), co(a),
co(b), co(c)); };
nf = 4;
process = _<:par(i,nf,formant(i)):>_;