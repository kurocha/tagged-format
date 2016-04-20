#!BPY
#
# Copyright (c) 2010, 2011 Samuel G. D. Williams. <http://www.oriontransfer.co.nz>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

"""
This script exports a Mesh to a Tagged Format text file.

The tagged format is very simple storage system which stores an index list along with an array of vertices. Each vertex (by default) includes the position, normal and mapping (UV texture coordinates).
"""

bl_info = {
	"name": "Tagged Format",
	"author": "Samuel Williams",
	"version": (0, 6),
	"blender": (2, 7, 0),
	"location": "File > Export > Tagged Format",
	"description": "Import-Export Tagged Format",
	"warning": "",
	"category": "Import-Export"
}

import bpy

from bpy.props import StringProperty, BoolProperty
from collections import defaultdict

# Why isn't this a built in? Reminds me of why I love Ruby so much.
def flatten(l, ltypes=(list, tuple)):
	ltype = type(l)
	l = list(l)
	i = 0
	while i < len(l):
		while isinstance(l[i], ltypes):
			if not l[i]:
				l.pop(i)
				i -= 1
				break
			else:
				l[i:i + 1] = l[i]
		i += 1
	return ltype(l)

class Vertices(object):
	def __init__(self):
		self.vertices = []
		self.indices = {}
	
	def get(self, vertex):
		if vertex in self.indices:
			return self.indices[vertex]

		index = self.indices[vertex] = len(self.vertices)
		self.vertices.append(vertex)
		
		return index
	
	def __iter__(self):
		for item in self.vertices:
			yield item

def flipped_coordinates(coordinates, flip):
	if flip:
		coordinates[1] = 1.0 - coordinates[1]
	
	return coordinates

def convert_wxyz_quaternion(q):
	return [q[1], q[2], q[3], q[0]]

def safe_name(data_object):
	return data_object.name.replace(' ', '-')

def write_mesh(output, data_object, flip_uv_coordinates):
	output.write("{0}: mesh triangles\n".format(safe_name(data_object)))
	mesh = data_object.to_mesh(bpy.context.scene, True, "PREVIEW")

	world_matrix = data_object.matrix_world
	mesh.transform(world_matrix)
	
	mesh.calc_normals()
	mesh.calc_tessface()
	mesh.update(calc_tessface=True)

	# Transform the mesh data into world coordinates:
	vertices = Vertices()
	triangles = []
	
	# We might not have uv_coordinates, so we check here:
	if mesh.tessface_uv_textures:
		vertex_format = "vertex-p3n3m2"
		uv_coordinates = mesh.tessface_uv_textures[0]
		
		def generate_vertex(face, index, offset):
			return (
				tuple(mesh.vertices[offset].co),
				tuple(mesh.vertices[offset].normal),
				tuple(flipped_coordinates(uv_coordinates.data[face.index].uv[index], flip_uv_coordinates)),
			)
	else:
		vertex_format = "vertex-p3n3"
		
		def generate_vertex(face, index, offset):
			return (
				tuple(mesh.vertices[offset].co),
				tuple(mesh.vertices[offset].normal),
			)
	
	for face in mesh.tessfaces:
		# We assume a face represents a triangle fan:
		face_vertices = [generate_vertex(face, index, offset) for index, offset in enumerate(face.vertices)]
		
		center_vertex = vertices.get(face_vertices[0])
		previous_vertex = vertices.get(face_vertices[1])
		
		for face_vertex in face_vertices[2:]:
			next_vertex = vertices.get(face_vertex)
			triangles.append([center_vertex, previous_vertex, next_vertex])
			previous_vertex = next_vertex
	
	output.write("\tindices: array index16\n")
	for triangle in triangles:
		output.write("\t\t{0} {1} {2}\n".format(*triangle))
	output.write("\tend\n")
	
	output.write("\tvertices: array {0}\n".format(vertex_format))
	
	for vertex in vertices:
		output.write("\t\t{0}\n".format(" ".join([str(value) for value in flatten(vertex)])))
	output.write("\tend\n")
	
	axes = [axis for axis in data_object.children if axis.type == 'EMPTY']
	if len(axes):
		output.write("\taxes: array axis\n")
		
		for axis in axes:
			output.write("\t\t{0} {1} {2}\n".format(
				axis.name.rsplit('.', 2)[0],
				" ".join([str(i) for i in axis.location]),
				" ".join([str(i) for i in convert_wxyz_quaternion(axis.rotation_quaternion)])
			))
		
		output.write("\tend\n")
		
	output.write("end\n\n")
	bpy.data.meshes.remove(mesh)

def write_matrix(output, matrix, indentation = "\t"):
	# We output matricies in row-major order, so that items 13, 14, 15 are the translation.
	for column in matrix.col:
		output.write("{0}{1}\n".format(indentation, " ".join([str(i) for i in column])))

def write_camera(output, data_object, render):
	output.write("{0}: camera\n".format(safe_name(data_object)))
	
	view_matrix = data_object.matrix_world.inverted()
	projection_matrix = data_object.calc_matrix_camera(
		render.resolution_x,
		render.resolution_y,
		render.pixel_aspect_x,
		render.pixel_aspect_y,
	)
	
	output.write("\t{0} {1} {2} {3}\n".format(
		render.resolution_x, render.resolution_y,
		render.pixel_aspect_x, render.pixel_aspect_y,
	))
	
	write_matrix(output, view_matrix)
	write_matrix(output, projection_matrix)

def write_tagged_format_text(filepath, flip_uv_coordinates):
	output = open(filepath, "w")
	
	context = bpy.context
	export_objects = context.selected_objects
	render = context.scene.render
	
	names = []
	for data_object in export_objects:
		if data_object.type == 'MESH':
			write_mesh(output, data_object, flip_uv_coordinates)
			names.append(data_object.name)
		elif data_object.type == 'CAMERA':
			write_camera(output, data_object, render)
			names.append(data_object.name)
			
	output.write("top: offset-table\n")
	for name in names:
		output.write("\t{0}: ${0}\n".format(name))
	output.write("end\n")
	
	output.close()

class TaggedFormatExporter(bpy.types.Operator):
	'''Save triangle mesh data'''
	bl_idname = "export.tagged_format"
	bl_label = "Export Tagged Format"

	filepath = StringProperty(
		subtype='FILE_PATH',
	)

	check_existing = BoolProperty(
		name="Check Existing",
		description="Check and warn on overwriting existing files",
		default=True,
		options={'HIDDEN'},
	)

	apply_modifiers = BoolProperty(
		name="Apply Modifiers",
		description="Use transformed mesh data from each object",
		default=True,
	)

	flip_uv_coordinates = BoolProperty(
		name="Flip UV Coordinates",
		description="Flip the UV coordinates vertically for systems where the image origin is the upper left",
		default=True,
	)

	def execute(self, context):
		write_tagged_format_text(self.filepath, self.flip_uv_coordinates)

		return {'FINISHED'}

	def invoke(self, context, event):
		if not self.filepath:
			self.filepath = bpy.path.ensure_ext(bpy.data.filepath, ".tft")
		wm = context.window_manager
		wm.fileselect_add(self)
		return {'RUNNING_MODAL'}

def menu_export(self, context):
	self.layout.operator(TaggedFormatExporter.bl_idname, text="Tagged Format (.tft)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_export)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
	register()
