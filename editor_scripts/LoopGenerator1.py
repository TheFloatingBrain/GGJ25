import bpy
import bmesh
import math
import mathutils

#bpyscene = bpy.context.scene
#mesh = bpy.data.meshes.new("Basic_Cube")
#basic_cube = bpy.data.objects.new("Basic_Cube",  mesh)
#bpyscene.collection.objects.link(basic_cube)
    #bpyscene.collection.objects.active = basic_cube
#asic_cube.select = True
radius=10
loop_offset=20
d2r=0.01745329
segment_count = 100
total_angle = 360
angle=(total_angle*d2r/segment_count)
#bmesh.ops.translate(bm, vec=(0, -radius, 0), verts=bm.verts)
x=0
y=0
z=0
track_width = 1
z_step=(loop_offset+track_width)/segment_count
for ii in range(0, segment_count+1):
    z+=z_step
    bm = bmesh.new()
    bmesh.ops.create_cube(bm, size=1.0)
    bmesh.ops.scale(bm, vec=(.7, .10, track_width), verts=bm.verts)
    
    dx = math.cos(angle*ii)*radius
    dy = math.cos(angle*ii)*radius
    flip_factor=math.cos(ii * math.pi / 2*segment_count)
    #flip_factor*100/segment_count
    bmesh.ops.rotate(
            bm, 
            matrix=mathutils.Euler((0.0, z_step, 0.0), 'XYZ').to_matrix(), 
            verts=bm.verts[:]
        )
    geom = bm.verts[:] + bm.edges[:] + bm.faces[:]
    bmesh.ops.spin(
            bm, 
            geom=geom, 
            axis=(0, 0, 1), 
            angle=angle*ii, 
            dvec=(0, radius, z), 
            steps=1, 
            use_duplicate=True
        )
    x+=dx
    y+=dy
    mesh = bpy.data.meshes.new("Mesh")
    bm.to_mesh(mesh)
    bm.free()
    obj = bpy.data.objects.new("Track",  mesh)
    bpy.context.collection.objects.link(obj)
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)



