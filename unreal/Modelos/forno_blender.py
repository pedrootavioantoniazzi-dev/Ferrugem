import bpy, math, sys
out=sys.argv[sys.argv.index('--')+1]
bpy.ops.wm.read_factory_settings(use_empty=True)
def mat(n,c,e=0,r=.6,m=0):
    M=bpy.data.materials.new(n);M.use_nodes=True;b=M.node_tree.nodes['Principled BSDF']
    b.inputs['Base Color'].default_value=(*c,1);b.inputs['Roughness'].default_value=r;b.inputs['Metallic'].default_value=m
    if e:b.inputs['Emission Color'].default_value=(*c,1);b.inputs['Emission Strength'].default_value=e
    return M
def srgb(h):
    h=h.lstrip('#');v=[int(h[i:i+2],16)/255 for i in (0,2,4)]
    return tuple(x/12.92 if x<=.04045 else ((x+.055)/1.055)**2.4 for x in v)
tijolo=mat('Tijolo',srgb('B8442B'),r=.8); aco=mat('Aco',srgb('2B3138'),r=.45,m=.8); fogo=mat('Fogo',srgb('FF7A1F'),e=1.6)
amarelo=mat('Amarelo',srgb('F2C230'),r=.5); preto=mat('Preto',srgb('15171A'),r=.6); boca=mat('Boca',srgb('14090A'),r=.9)
objs=[]
def cubo(nome,loc,esc,m,bev=0.03):
    bpy.ops.mesh.primitive_cube_add(location=loc);o=bpy.context.object;o.name=nome;o.scale=esc
    bpy.ops.object.transform_apply(scale=True)
    if bev:
        b=o.modifiers.new('b','BEVEL');b.width=bev;b.segments=2
    o.data.materials.append(m);objs.append(o);return o
def cil(nome,loc,r,h,m,rot=(0,0,0),v=16):
    bpy.ops.mesh.primitive_cylinder_add(vertices=v,radius=r,depth=h,location=loc,rotation=rot);o=bpy.context.object;o.name=nome
    o.data.materials.append(m);objs.append(o);return o
# base de aco com faixas de perigo
cubo('Base',(0,0,.06),(.5,.5,.06),aco)
for i in range(6):
    cubo('Faixa%d'%i,(.47,-.375+i*.15,.06),(.035,.06,.045),amarelo if i%2==0 else preto,0)
# corpo de tijolo
cubo('Corpo',(0,0,.5),(.42,.42,.38),tijolo,.04)
# linhas dos tijolos
for z in (.3,.45,.6,.75):
    cubo('Linha',(0,0,z),(.425,.425,.006),boca,0)
# boca do forno com fogo
cubo('Moldura',(.43,0,.4),(.03,.24,.2),aco,.02)
cubo('Boca',(.455,0,.38),(.012,.17,.14),boca,0)
cubo('Fogo',(.469,0,.31),(.006,.15,.08),fogo,0)
cubo('Brasa',(.469,0,.42),(.006,.09,.04),fogo,0)
# chamine
cil('Chamine',(-.22,.22,1.05),.08,.35,aco)
cil('Topo',(-.22,.22,1.24),.1,.04,aco)
# topo
cubo('Tampa',(0,0,.9),(.44,.44,.03),aco,.02)
# junta tudo num objeto so
for o in objs:o.select_set(True)
bpy.context.view_layer.objects.active=objs[0]
for o in objs:
    bpy.context.view_layer.objects.active=o
    for md in list(o.modifiers):bpy.ops.object.modifier_apply(modifier=md.name)
for o in objs:o.select_set(True)
bpy.context.view_layer.objects.active=objs[0];bpy.ops.object.join();F=bpy.context.object;F.name='furn'
tri=sum(len(p.vertices)-2 for p in F.data.polygons);print('TRIANGULOS',tri)
bpy.ops.export_scene.fbx(filepath=out+'/furn.fbx',use_selection=True,apply_scale_options='FBX_SCALE_UNITS',axis_forward='X',axis_up='Z')
# foto
bpy.ops.object.camera_add(location=(2.3,-1.6,1.7));cam=bpy.context.object
c=cam.constraints.new('TRACK_TO');bpy.ops.object.empty_add(location=(0,0,.45));c.target=bpy.context.object;c.track_axis='TRACK_NEGATIVE_Z';c.up_axis='UP_Y'
bpy.context.scene.camera=cam
bpy.ops.object.light_add(type='SUN',location=(2,-2,4));L=bpy.context.object;L.data.energy=3.5;L.rotation_euler=(math.radians(50),0,math.radians(35))
bpy.ops.mesh.primitive_plane_add(size=6);bpy.context.object.data.materials.append(mat('Chao',srgb('10151C'),r=.9))
w=bpy.data.worlds.new('W');bpy.context.scene.world=w;w.use_nodes=True;w.node_tree.nodes['Background'].inputs[0].default_value=(*srgb('1A2230'),1);w.node_tree.nodes['Background'].inputs[1].default_value=.6
s=bpy.context.scene;s.render.engine='CYCLES';s.cycles.samples=48;s.cycles.device='CPU';s.render.resolution_x=900;s.render.resolution_y=700
s.render.filepath=out+'/furn.png';bpy.ops.render.render(write_still=True)
