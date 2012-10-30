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
	"name": "Tagged Format (.tagged-*)",
	"author": "Samuel Williams",
	"version": (0, 3),
	"blender": (2, 6, 4),
	"location": "File > Export > Tagged Format (.tagged-*)",
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

def face_to_triangles(face):
	triangles = []
	if len(face.vertices) == 4:
		triangles.append((face.vertices[0], face.vertices[1], face.vertices[2],))
		triangles.append((face.vertices[2], face.vertices[3], face.vertices[0],))
	else:
		triangles.append(face.vertices)

	return triangles

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

def write_mesh(output, data_object, flip_uv_coordinates):
	output.write("{0}: mesh triangles\n".format(data_object.name))
	mesh = data_object.to_mesh(bpy.context.scene, True, "PREVIEW")

	# From Blender 2.6.3, we now need to use tessfaces:
	mesh.update(calc_tessface=True)

	# Transform the mesh data into world coordinates:
	world_matrix = data_object.matrix_world
	mesh.transform(world_matrix)
	
	vertices = Vertices()
	triangles = []
	
	# We might not have uv_coordinates, so we check here:
	if mesh.tessface_uv_textures:
		vertex_format = "3p3n2m"
		uv_coordinates = mesh.tessface_uv_textures[0]
		
		def generate_vertex(index, triangle, offset):
			return (
				tuple(mesh.vertices[triangle[offset]].co),
				tuple(mesh.vertices[triangle[offset]].normal),
				tuple(flipped_coordinates(uv_coordinates.data[index].uv[offset], flip_uv_coordinates)),
			)
	else:
		vertex_format = "3p3n"
		
		def generate_vertex(index, triangle, offset):
			return (
				tuple(mesh.vertices[triangle[offset]].co),
				tuple(mesh.vertices[triangle[offset]].normal),
			)
	
	for index, face in enumerate(mesh.tessfaces):
		for triangle in face_to_triangles(face):
			face_vertices = [generate_vertex(index, triangle, offset) for offset in range(3)]
			
			triangles.append([vertices.get(vertex) for vertex in face_vertices])
	
	output.write("\tindices: array 2u\n")
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
				" ".join([str(i) for i in axis.rotation_quaternion])
			))
		
		output.write("\tend\n")
		
	output.write("end\n\n")
	bpy.data.meshes.remove(mesh)

def write_tagged_format_text(filepath, flip_uv_coordinates):
	output = open(filepath, "w")
	
	names = []
	for data_object in bpy.context.selected_objects:
		if data_object.type == 'MESH':
			write_mesh(output, data_object, flip_uv_coordinates)
			names.append(data_object.name)
	
	output.write("top: dictionary\n")
	for name in names:
		output.write("\t{0}: ${0}\n".format(name))
	output.write("end\n")
	
	output.close()

class TaggedFormatMeshExporter(bpy.types.Operator):
	'''Save triangle mesh data'''
	bl_idname = "export.tagged_format_mesh"
	bl_label = "Export Tagged Format Mesh"

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
	self.layout.operator(TaggedFormatMeshExporter.bl_idname, text="Tagged Format (.tft)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_export)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
	register()
