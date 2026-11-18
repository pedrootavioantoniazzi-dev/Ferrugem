# Ferramentas para modelar as maquinas do Ferrugem no Blender (versao caprichada)
import bpy, bmesh, math
from mathutils import Vector, Matrix
def limpar():
    bpy.ops.wm.read_factory_settings(use_empty=True)
def srgb(h):
    h=h.lstrip('#');v=[int(h[i:i+2],16)/255 for i in (0,2,4)]
    return tuple(x/12.92 if x<=.04045 else ((x+.055)/1.055)**2.4 for x in v)
_M={}
def mat(n,h,r=.55,m=0,e=0):
    if n in _M:return _M[n]
    M=bpy.data.materials.new(n);M.use_nodes=True;b=M.node_tree.nodes['Principled BSDF'];c=srgb(h)
    b.inputs['Base Color'].default_value=(*c,1);b.inputs['Roughness'].default_value=r;b.inputs['Metallic'].default_value=m
    if e:b.inputs['Emission Color'].default_value=(*c,1);b.inputs['Emission Strength'].default_value=e
    _M[n]=M;return M
OBJS=[]
def _fim(o,m,bev,seg=3):
    if bev:
        b=o.modifiers.new('b','BEVEL');b.width=bev;b.segments=seg;b.limit_method='ANGLE';b.harden_normals=False
    if m:o.data.materials.append(m)
    OBJS.append(o);return o
def cubo(loc,esc,m,bev=.015,rot=(0,0,0),seg=3):
    bpy.ops.mesh.primitive_cube_add(location=loc,rotation=rot);o=bpy.context.object;o.scale=esc
    bpy.ops.object.transform_apply(scale=True);return _fim(o,m,bev,seg)
def cil(loc,r,h,m,rot=(0,0,0),v=24,bev=0,seg=2):
    bpy.ops.mesh.primitive_cylinder_add(vertices=v,radius=r,depth=h,location=loc,rotation=rot);return _fim(bpy.context.object,m,bev,seg)
def cone(loc,r1,r2,h,m,rot=(0,0,0),v=24,bev=0):
    bpy.ops.mesh.primitive_cone_add(vertices=v,radius1=r1,radius2=r2,depth=h,location=loc,rotation=rot);return _fim(bpy.context.object,m,bev)
def esfera(loc,r,m,seg=16):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=seg,ring_count=seg//2,radius=r,location=loc);return _fim(bpy.context.object,m,0)
def parafuso(loc,normal='y',r=.012,m=None):
    rot={'x':(0,math.pi/2,0),'y':(math.pi/2,0,0),'z':(0,0,0)}[normal]
    return cil(loc,r,.012,m or mat('Parafuso','9AA2AC',.3,.9),rot=rot,v=6)
def cortar(alvo,corte,op='DIFFERENCE'):
    b=alvo.modifiers.new('bool','BOOLEAN');b.object=corte;b.operation=op;b.solver='EXACT'
    bpy.context.view_layer.objects.active=alvo
    for md in list(alvo.modifiers):bpy.ops.object.modifier_apply(modifier=md.name)
    OBJS.remove(corte) if corte in OBJS else None;bpy.data.objects.remove(corte)
def faixa_diag(centro,larg,alt,prof,eixo='x',n=None,ang=40):
    # faixa de perigo com listras diagonais (recortada no tamanho certo)
    am=mat('Amarelo','F2C230',.45);pr=mat('Preto','17191C',.6)
    cx,cy,cz=centro
    esc=(larg/2,prof/2,alt/2) if eixo=='x' else (prof/2,larg/2,alt/2)
    fundo=cubo(centro,esc,pr,0)
    n=n or max(3,int(larg/0.07));passo=larg/n
    for i in range(0,n+2,2):
        p=-larg/2+passo*i
        loc=(cx+p,cy,cz) if eixo=='x' else (cx,cy+p,cz)
        rot=(0,math.radians(ang),0) if eixo=='x' else (math.radians(-ang),0,0)
        lst=cubo(loc,(passo/2,prof/2+.003,alt) if eixo=='x' else (prof/2+.003,passo/2,alt),am,0,rot=rot)
        caixa=cubo(centro,(esc[0],esc[1]+.004,esc[2]) if eixo=='x' else (esc[0]+.004,esc[1],esc[2]),None,0)
        cortar(lst,caixa,'INTERSECT')
    return fundo
def helice(centro,r0,r1,z0,z1,voltas,larg,esp,m,passos=120):
    # lamina em espiral (rosca da broca), afinando de r0 ate r1
    me=bpy.data.meshes.new('helice');bm=bmesh.new();cx,cy=centro[0],centro[1];vs=[]
    for i in range(passos+1):
        t=i/passos;a=t*voltas*2*math.pi;r=r0+(r1-r0)*t;z=z0+(z1-z0)*t;ri=max(r-larg*(1-t*.6),0.005)
        c,s=math.cos(a),math.sin(a)
        q=[bm.verts.new((cx+ri*c,cy+ri*s,z)),bm.verts.new((cx+r*c,cy+r*s,z)),bm.verts.new((cx+r*c,cy+r*s,z+esp)),bm.verts.new((cx+ri*c,cy+ri*s,z+esp))]
        vs.append(q)
    for i in range(passos):
        a,b=vs[i],vs[i+1]
        for k in range(4):bm.faces.new((a[k],a[(k+1)%4],b[(k+1)%4],b[k]))
    bm.faces.new(vs[0][::-1]);bm.faces.new(vs[-1])
    bm.to_mesh(me);bm.free();o=bpy.data.objects.new('helice',me);bpy.context.collection.objects.link(o)
    o.data.materials.append(m);OBJS.append(o);return o
def cano_curvo(p0,meio,p1,r,m):
    cu=bpy.data.curves.new('cano','CURVE');cu.dimensions='3D';cu.bevel_depth=r;cu.bevel_resolution=4;cu.use_fill_caps=True
    sp=cu.splines.new('BEZIER');sp.bezier_points.add(2)
    for bp,p in zip(sp.bezier_points,(p0,meio,p1)):bp.co=p;bp.handle_left_type=bp.handle_right_type='AUTO'
    o=bpy.data.objects.new('cano',cu);bpy.context.collection.objects.link(o)
    bpy.context.view_layer.objects.active=o;o.select_set(True);bpy.ops.object.convert(target='MESH');o=bpy.context.object
    o.data.materials.append(m);OBJS.append(o);return o
def juntar(nome):
    for o in OBJS:
        bpy.context.view_layer.objects.active=o
        for md in list(o.modifiers):bpy.ops.object.modifier_apply(modifier=md.name)
    bpy.ops.object.select_all(action='DESELECT')
    for o in OBJS:o.select_set(True)
    bpy.context.view_layer.objects.active=OBJS[0];bpy.ops.object.join();F=bpy.context.object;F.name=nome
    bpy.ops.object.shade_auto_smooth(angle=math.radians(35)) if hasattr(bpy.ops.object,'shade_auto_smooth') else bpy.ops.object.shade_smooth()
    for md in list(F.modifiers):bpy.ops.object.modifier_apply(modifier=md.name)
    print('TRIANGULOS',sum(len(p.vertices)-2 for p in F.data.polygons));return F
def exportar(F,pasta,nome):
    bpy.ops.object.select_all(action='DESELECT');F.select_set(True);bpy.context.view_layer.objects.active=F
    bpy.ops.export_scene.fbx(filepath=f'{pasta}/{nome}.fbx',use_selection=True,apply_scale_options='FBX_SCALE_UNITS',axis_forward='X',axis_up='Z',mesh_smooth_type='FACE')
def foto(pasta,nome,alvo=(0,0,.5),cam=(.9,-2.6,1.5),lente=55):
    bpy.ops.object.camera_add(location=cam);c=bpy.context.object;c.data.lens=lente
    bpy.ops.object.empty_add(location=alvo);t=c.constraints.new('TRACK_TO');t.target=bpy.context.object;t.track_axis='TRACK_NEGATIVE_Z';t.up_axis='UP_Y'
    bpy.context.scene.camera=c
    for (e,rx,rz,cor) in ((3.4,40,25,'FFF1DD'),(1.2,55,205,'C8D8FF'),(1.6,75,120,'FFFFFF')):
        bpy.ops.object.light_add(type='SUN');L=bpy.context.object;L.data.energy=e;L.data.color=srgb(cor);L.data.angle=math.radians(8);L.rotation_euler=(math.radians(rx),0,math.radians(rz))
    w=bpy.data.worlds.new('W');bpy.context.scene.world=w;w.use_nodes=True;bg=w.node_tree.nodes['Background'];bg.inputs[0].default_value=(*srgb('2A3445'),1);bg.inputs[1].default_value=.9
    s=bpy.context.scene;s.render.engine='CYCLES';s.cycles.samples=96;s.cycles.device='CPU';s.render.resolution_x=1000;s.render.resolution_y=900
    s.render.film_transparent=True
    try:s.view_settings.view_transform='AgX'
    except Exception:s.view_settings.view_transform='Filmic'
    s.render.filepath=f'{pasta}/{nome}.png';bpy.ops.render.render(write_still=True)
