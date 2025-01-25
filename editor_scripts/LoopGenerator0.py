import bpy
import bmesh
import math

#bpyscene = bpy.context.scene
#mesh = bpy.data.meshes.new("Basic_Cube")
#basic_cube = bpy.data.objects.new("Basic_Cube",  mesh)
#bpyscene.collection.objects.link(basic_cube)
    #bpyscene.collection.objects.active = basic_cube
#asic_cube.select = True
radius=10

bm = bmesh.new()
bmesh.ops.create_cube(bm, size=1.0)
bmesh.ops.scale(bm, vec=(2.0, .10, 1.0), verts=bm.verts)
d2r=0.01745329
segment_count = 100
total_angle = 360
angle=(total_angle*d2r/segment_count)
#bmesh.ops.translate(bm, vec=(0, -radius, 0), verts=bm.verts)
geom = bm.verts[:] + bm.edges[:] + bm.faces[:]
x=0
y=0
for ii in range(0, segment_count+1):
    dx = math.cos(angle*ii)*radius
    dy = math.cos(angle*ii)*radius
    bmesh.ops.spin(
            bm, 
            geom=geom, 
            axis=(0, 1/segment_count, 1), 
            angle=angle*ii, 
            dvec=(0, radius, 0.0), 
            steps=1, 
            use_duplicate=True
        )
    x+=dx
    y+=dy



mesh = bpy.data.meshes.new("Mesh")
bm.to_mesh(mesh)
bm.free()
obj = bpy.data.objects.new("Basic_Cube",  mesh)
bpy.context.collection.objects.link(obj)
bpy.context.view_layer.objects.active = obj
obj.select_set(True)
