import sys,math;sys.path.insert(0,sys.argv[-1]);from base2 import *
P=sys.argv[-1];limpar()
aco=mat('Aco','454B54',.45,.75);esc=mat('AcoEscuro','26292F',.5,.7);lar=mat('Laranja','E8601C',.5);ama=mat('Amarelo','F2C230',.45)
broca=mat('Broca','C3C9D2',.28,.85);luz=mat('Luz','FFC21A',.3,0,5);pret=mat('Preto','17191C',.6);cromo=mat('Cromo','D9DEE4',.15,1)
# ---------- BASE ----------
cubo((0,0,.06),(.5,.43,.06),esc,.02)
cubo((0,0,.135),(.46,.39,.015),aco,.008)
for x in (-.42,.42):
    for y in (-.35,.35):
        cubo((x,y,.07),(.1,.1,.07),aco,.02)
        parafuso((x,y-.101,.07),'y')
faixa_diag((0,-.432,.065),.62,.06,.006,'x')
faixa_diag((.502,0,.065),.5,.06,.006,'y')
# ---------- BROCA (rosca em espiral) ----------
DX=-.2
cone((DX,0,.36),.13,.02,.5,broca,rot=(math.pi,0,0),v=20)     # nucleo (ponta para baixo)
helice((DX,0),.2,.035,.6,.13,3.2,.12,.035,broca,160)           # lamina em espiral
cil((DX,0,.64),.15,.08,esc,v=12,bev=.01)
# ---------- CABECA ----------
cil((DX,0,.74),.26,.12,lar,v=10,bev=.015)
cil((DX,0,.68),.27,.035,esc,v=10,bev=.006)
cil((DX,0,.81),.27,.035,esc,v=10,bev=.006)
# faixa diagonal em volta da cabeca
for i in range(20):
    a=i*2*math.pi/20
    cubo((DX+.262*math.cos(a),.262*math.sin(a),.745),(.012,.03,.045),ama if i%2==0 else pret,0,rot=(math.radians(30),0,a))
cil((DX,0,.9),.24,.14,lar,v=10,bev=.015)
cil((DX,0,.99),.18,.04,esc,v=10,bev=.008)
cil((DX,0,1.03),.08,.05,aco,v=10,bev=.006)
cubo((DX-.02,-.235,.92),(.07,.012,.022),luz,.004)          # luz amarela
cubo((DX-.02,-.233,.92),(.085,.008,.035),esc,.004)
for k in range(3):cubo((DX+.12,-.17,.93-.025*k),(.04,.006,.006),pret,0,rot=(0,0,math.radians(-35)))  # respiros
# ---------- BRACO + PISTAO ----------
cubo((.0,0,.5),(.07,.14,.3),aco,.02,rot=(0,math.radians(-28),0))
cubo((-.02,0,.9),(.17,.12,.07),lar,.02)
for y in (-.13,.13):
    cil((.02,y,.88),.06,.03,esc,rot=(math.pi/2,0,0),v=16,bev=.005)
    cil((.02,y*1.08,.88),.025,.01,cromo,rot=(math.pi/2,0,0),v=10)
cil((.12,-.15,.48),.04,.32,ama,rot=(0,math.radians(-22),0),v=14,bev=.004)
cil((.06,-.15,.66),.022,.2,cromo,rot=(0,math.radians(-22),0),v=12)
cubo((.17,-.15,.3),(.05,.03,.03),lar,.008)
# ---------- MOTOR ----------
mot=cubo((.27,.02,.34),(.17,.22,.19),lar,.03)
cubo((.27,.02,.545),(.15,.2,.02),esc,.01)
for k in range(5):
    cubo((.442,.02,.45-.045*k),(.008,.13,.012),pret,0)
placa=cubo((.272,-.205,.3),(.07,.006,.07),esc,.004)
cone((.272,-.213,.3),.055,0,.004,ama,rot=(math.pi/2,0,0),v=3)
cubo((.272,-.217,.29),(.006,.002,.022),pret,0)
for z in (.2,.48):
    for x in (.14,.4):parafuso((x,-.222,z),'y')
cil((.33,.12,.6),.042,.12,esc,v=14,bev=.004)
cano_curvo((.33,.12,.65),(.33,.12,.76),(.24,.12,.8),.04,esc)
cil((.215,.12,.8),.05,.04,aco,rot=(0,math.pi/2,0),v=14)
F=juntar('miner');exportar(F,P,'miner');foto(P,'miner',alvo=(.02,0,.5),cam=(1.0,-2.7,1.55))
