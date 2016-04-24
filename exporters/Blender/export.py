# Run as: blender -b <filename> -P <this_script> -- <scene_name> <output_path>
# e.g. blender -b cube.blend -P export.py -- Scene test.tft

import bpy, sys, os

scene_name = sys.argv[-2]
output_path = sys.argv[-1]

from io_tagged_format import safe_name, write_mesh, write_camera

def write_to_file(scene, export_objects, path, flip_uv_coordinates=True):
	render = scene.render
	
	names = []
	for data_object in export_objects:
		if data_object.type == 'MESH':
			write_mesh(output, data_object, flip_uv_coordinates)
			names.append(safe_name(data_object))
		elif data_object.type == 'CAMERA':
			write_camera(output, data_object, render)
			names.append(safe_name(data_object))
			
	output.write("top: offset-table\n")
	for name in names:
		output.write("\t{0}: ${0}\n".format(name))
	output.write("end\n")

scene = bpy.data.scenes[scene_name]

output = open(output_path, "w")
write_to_file(scene, scene.objects, output_path)
output.close()
