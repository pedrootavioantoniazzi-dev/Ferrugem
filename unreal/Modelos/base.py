# Ferramentas comuns para modelar as maquinas do Ferrugem no Blender (por codigo)
import bpy, math
def limpar():
    bpy.ops.wm.read_factory_settings(use_empty=True)
def srgb(h):
    h=h.lstrip('#');v=[int(h[i:i+2],16)/255 for i in (0,2,4)]
    return tuple(x/12.92 if x<=.04045 else ((x+.055)/1.055)**2.4 for x in v)
_M={}
def mat(n,h,r=.6,m=0,e=0):
    if n in _M:return _M[n]
    M=bpy.data.materials.new(n);M.use_nodes=True;b=M.node_tree.nodes['Principled BSDF'];c=srgb(h)
    b.inputs['Base Color'].default_value=(*c,1);b.inputs['Roughness'].default_value=r;b.inputs['Metallic'].default_value=m
    if e:b.inputs['Emission Color'].default_value=(*c,1);b.inputs['Emission Strength'].default_value=e
    _M[n]=M;return M
OBJS=[]
def _fim(o,m,bev):
    if bev:
        b=o.modifiers.new('b','BEVEL');b.width=bev;b.segments=2;b.limit_method='ANGLE'
    o.data.materials.append(m);OBJS.append(o);return o
def cubo(loc,esc,m,bev=.02,rot=(0,0,0)):
    bpy.ops.mesh.primitive_cube_add(location=loc,rotation=rot);o=bpy.context.object;o.scale=esc
    bpy.ops.object.transform_apply(scale=True);return _fim(o,m,bev)
def cil(loc,r,h,m,rot=(0,0,0),v=16,bev=0):
    bpy.ops.mesh.primitive_cylinder_add(vertices=v,radius=r,depth=h,location=loc,rotation=rot);return _fim(bpy.context.object,m,bev)
def cone(loc,r1,r2,h,m,rot=(0,0,0),v=16):
    bpy.ops.mesh.primitive_cone_add(vertices=v,radius1=r1,radius2=r2,depth=h,location=loc,rotation=rot);return _fim(bpy.context.object,m,0)
def esfera(loc,r,m,seg=12):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=seg,ring_count=seg//2,radius=r,location=loc);return _fim(bpy.context.object,m,0)
def faixa(x0,x1,y,z,alt,larg=.06,eixo='x'):
    # faixa de perigo: blocos amarelos e pretos alternados
    n=max(2,int(abs(x1-x0)/larg));p=(x1-x0)/n
    for i in range(n):
        c=x0+p*(i+.5);m=mat('Amarelo','F2C230',.5) if i%2==0 else mat('Preto','15171A',.6)
        cubo((c,y,z) if eixo=='x' else (y,c,z),(abs(p)/2,.012,alt) if eixo=='x' else (.012,abs(p)/2,alt),m,0)
def juntar(nome):
    for o in OBJS:
        bpy.context.view_layer.objects.active=o
        for md in list(o.modifiers):bpy.ops.object.modifier_apply(modifier=md.name)
    for o in OBJS:o.select_set(True)
    bpy.context.view_layer.objects.active=OBJS[0];bpy.ops.object.join();F=bpy.context.object;F.name=nome
    bpy.ops.object.shade_flat()
    print('TRIANGULOS',sum(len(p.vertices)-2 for p in F.data.polygons));return F
def exportar(F,pasta,nome):
    F.select_set(True);bpy.context.view_layer.objects.active=F
    bpy.ops.export_scene.fbx(filepath=f'{pasta}/{nome}.fbx',use_selection=True,apply_scale_options='FBX_SCALE_UNITS',axis_forward='X',axis_up='Z')
def foto(pasta,nome,alvo=(0,0,.5),dist=1.0,cam=(2.2,-1.7,1.6)):
    bpy.ops.object.camera_add(location=(cam[0]*dist,cam[1]*dist,cam[2]*dist));cam=bpy.context.object
    bpy.ops.object.empty_add(location=alvo);c=cam.constraints.new('TRACK_TO');c.target=bpy.context.object;c.track_axis='TRACK_NEGATIVE_Z';c.up_axis='UP_Y'
    bpy.context.scene.camera=cam
    bpy.ops.object.light_add(type='SUN');L=bpy.context.object;L.data.energy=3.2;L.rotation_euler=(math.radians(45),0,math.radians(30))
    bpy.ops.object.light_add(type='SUN');L=bpy.context.object;L.data.energy=1.0;L.rotation_euler=(math.radians(60),0,math.radians(200))
    bpy.ops.mesh.primitive_plane_add(size=8);bpy.context.object.data.materials.append(mat('Chao','10151C',.9))
    w=bpy.data.worlds.new('W');bpy.context.scene.world=w;w.use_nodes=True;bg=w.node_tree.nodes['Background'];bg.inputs[0].default_value=(*srgb('1A2230'),1);bg.inputs[1].default_value=.7
    s=bpy.context.scene;s.render.engine='CYCLES';s.cycles.samples=48;s.cycles.device='CPU';s.render.resolution_x=900;s.render.resolution_y=800
    s.render.filepath=f'{pasta}/{nome}.png';bpy.ops.render.render(write_still=True)
