#!BPY
#  ***** GPL LICENSE BLOCK *****
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#  All rights reserved.
#  ***** GPL LICENSE BLOCK *****
"""
This script exports a Mesh to a Tagged Model File Format (TMFF).

The tagged model file format is very simple binary data which stores an index list along with an array of vertices. Each vertex (by default) includes the position, normal and mapping (UV texture coordinates).
"""

bl_info = {
	"name": "Tagged Model Format (.tagged-model-*)",
	"author": "Samuel Williams",
	"version": (0, 2),
	"blender": (2, 5, 8),
	"location": "File > Export > Tagged Model File Format (.tagged-model-*)",
	"description": "Import-Export Tagged Model File Format",
	"warning": "",
	"category": "Import-Export"
}

import bpy

from bpy.props import StringProperty, BoolProperty

def face_to_triangles(face):
	triangles = []
	if len(face.vertices) == 4:
		triangles.append((face.vertices[0], face.vertices[1], face.vertices[2],))
		triangles.append((face.vertices[2], face.vertices[3], face.vertices[0],))
	else:
		triangles.append(face.vertices)

	return triangles

class Vertex(object):
	def __init__(self, vertex, mapping, face):
		self._vertex = vertex
		self._mapping = mapping
		self._face = face
	
	def position(self):
		return self._vertex.co
	
	def normal(self):
		return self._vertex.normal
		#return self._face.normal
	
	def mapping(self):
		return self._mapping
	
	def __hash__(self):
		return hash(self.__key__())
	
	def __eq__(self, other):
		return self.__key__() == other.__key__()
	
	def __ne__(self, other):
		return self.__key__() != other.__key__()
	
	def __lt__(self, other):
		return self.__key__() < other.__key__()
	
	def __le__(self, other):
		return self.__key__() <= other.__key__()
	
	def __gt__(self, other):
		return self.__key__() > other.__key__()
	
	def __ge__(self, other):
		return self.__key__() >= other.__key__()
	
	def __key__(self):
		return (tuple(self.position()), tuple(self.normal()), tuple(self.mapping()),)

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

def write_tagged_model_text(filepath):
	scene = bpy.context.scene
	output = open(filepath, "w")
	
	for data_object in bpy.context.selected_objects:
		output.write("top: mesh triangles\n")
		mesh = data_object.to_mesh(scene, True, "PREVIEW")
		
		vertices = Vertices()
		triangles = []
		world_matrix = data_object.matrix_world
		uv_coordinates = mesh.uv_textures[0]
		for index, face in enumerate(mesh.faces):
			for triangle in face_to_triangles(face):
				# Get the 3 vertices
				face_vertices = [Vertex(mesh.vertices[triangle[i]], uv_coordinates.data[index].uv[i], face) for i in range(3)]
				triangles.append([vertices.get(vertex) for vertex in face_vertices])
		
		output.write("\tindices: array 2u\n")
		for triangle in triangles:
			output.write("\t\t{0} {1} {2}\n".format(*triangle))
		output.write("\tend\n")
		
		output.write("\tvertices: array 3p3n2m\n")
		
		for vertex in vertices:
			position = world_matrix * vertex.position()
			normal = world_matrix * vertex.normal()
			mapping = vertex.mapping()
			
			output.write("\t\t{0} {1} {2}\n".format(
				" ".join([str(i) for i in position]), 
				" ".join([str(i) for i in normal]),
				" ".join([str(i) for i in mapping])
			))
		output.write("\tend\n")
		
		output.write("end\n")
		bpy.data.meshes.remove(mesh)
	
	output.close()

class TMFExporter(bpy.types.Operator):
	'''Save triangle mesh data'''
	bl_idname = "export.tagged_model_text"
	bl_label = "Export Tagged Model"

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

	triangulate = BoolProperty(
			name="Triangulate",
			description="Triangulate quads",
			default=True,
			)

	def execute(self, context):
		write_tagged_model_text(self.filepath)

		return {'FINISHED'}

	def invoke(self, context, event):
		if not self.filepath:
			self.filepath = bpy.path.ensure_ext(bpy.data.filepath, ".tagged-model-text")
		wm = context.window_manager
		wm.fileselect_add(self)
		return {'RUNNING_MODAL'}

def menu_export(self, context):
	self.layout.operator(TMFExporter.bl_idname, text="Tagged Model Format (.tagged-model-text)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_export)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
	register()
