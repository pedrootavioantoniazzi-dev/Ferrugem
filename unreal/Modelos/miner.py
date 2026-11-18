import sys,math;sys.path.insert(0,sys.argv[-1]);from base import *
P=sys.argv[-1];limpar()
aco=mat('Aco','3A3F47',.5,.7);acoesc=mat('AcoEsc','23272D',.55,.7);lar=mat('Laranja','E8641E',.55);ama=mat('Amarelo','F2C230',.5)
broca=mat('Broca','B9C0CA',.3,.6);luz=mat('Luz','FFD24A',.3,0,3);pret=mat('Preto','15171A',.6)
# base com pes e faixas de perigo
cubo((0,0,.07),(.48,.42,.07),acoesc,.02)
for x in (-.4,.4):
    for y in (-.34,.34):cubo((x,y,.05),(.09,.09,.05),aco,.015)
faixa(-.42,.42,-.425,.08,.035)
faixa(-.36,.36,.42,.08,.035,eixo='y')
# broca em espiral (aneis girados) apontando para baixo, a esquerda
for i in range(6):
    r=.2-.03*i;z=.62-.085*i
    cone((-.18,0,z),r,r*.7,.09,broca,rot=(0,0,math.radians(i*25)),v=8)
cone((-.18,0,.12),.05,0,.12,broca,rot=(math.pi,0,0),v=8)
# cabeca laranja da broca
cil((-.18,0,.78),.24,.16,lar,v=8,bev=.02)
cil((-.18,0,.68),.25,.05,acoesc,v=8)
cil((-.18,0,.89),.25,.05,acoesc,v=8)
for i in range(8):
    a=i*math.pi/4;cubo((-.18+.245*math.cos(a),.245*math.sin(a),.78),(.02,.045,.045),ama if i%2==0 else pret,0,rot=(0,0,a))
cil((-.18,0,.97),.18,.12,lar,v=8,bev=.02)
cil((-.18,0,1.05),.07,.06,acoesc,v=8)
cubo((-.18,-.22,.97),(.02,.08,.025),luz,0)
# braco e pistao
cubo((.05,0,.55),(.06,.12,.33),aco,.02,rot=(0,math.radians(-25),0))
cubo((.0,0,.92),(.16,.1,.06),lar,.02)
cil((.0,0,.92),.07,.24,acoesc,rot=(math.pi/2,0,0),v=12)
cil((.1,-.12,.45),.035,.42,ama,rot=(0,math.radians(-20),0),v=10)
# motor laranja com grade e cano
cubo((.26,0,.33),(.17,.22,.19),lar,.025)
for k in range(4):cubo((.43,0,.42-.05*k),(.01,.12,.012),pret,0)
cubo((.26,0,.53),(.1,.12,.02),acoesc,.01)
cil((.3,.1,.62),.04,.18,acoesc,v=10)
cil((.3,.1,.73),.05,.04,aco,v=10)
F=juntar('miner');exportar(F,P,'miner');foto(P,'miner',alvo=(0,0,.5),cam=(.9,-2.6,1.5))
