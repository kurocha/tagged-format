Tagged Model Format
===================

* Author: Samuel G. D. Williams (<http://www.oriontransfer.co.nz>)
* Copyright (C) 2010, 2011 Samuel G. D. Williams.
* Released under the MIT license.

The Tagged Model Format is a simple text/binary format for model related data (e.g. indices, vertices). It is designed to be very simple and to act as a bridge between typical rendering formats (e.g. vertex arrays) and model editors (e.g. Blender).

File Format Structure
---------------------

The TMF file format is designed to be application specific rather than generic. Many generic file formats already exist (e.g. Wavefront OBJ) but they are cumbersome to use because they lack the ability to be customised without a significant amount of work.

Rather than try to design a one-size fits all general file format, you are encouraged to fork the TMF on a per-application basis and build specific resource formats that match your exact needs. Cases where you might want to customise the behaviour include unusual vertex formats (e.g. multiple texture coordinates, additional per-vertex attributes, etc) or embedded binary data (e.g. animation data, texture data, shaders).

TMF file structure consists of a sequential set of blocks, where each block has a tag and size. The basic file has a header which includes an offset to the top block. By default, a variety of block types are included in the TMF specification, including a `Dictionary`, `Mesh`, `Axis`, a variety of vertex formats and `External` references.

TMF has a text representation which is compiled to a binary format, much like how an assembler converts symbolic code to binary machine code.

TMF Tool
--------

Included in this implementation is the `tmf-tool` which converts a text format into a binary format. The text format is used primary for convenience when exporting data from existing modelling tools.

Here is an example of a simple 10x10 square made by two triangles:

	top: mesh triangles
		indices: array 2u
			0 1 2 3
		end

		vertices: array 3p3n2m
			 0  0 0 0 0 1 0 0
			 0 10 0 0 0 1 0 1
			10  0 0 0 0 1 1 0
			10 10 0 0 0 1 1 1
		end
	end

To assemble this to the binary TMF format, simply run:

	tmf-tool --text-to-binary input.tmt output.tmb

You can check the results by running:

	$ tmf-tool --dump-binary output.tmb
	<tmv2; 32 bytes; magic = 42>
	[mesh; 48 bytes; offset = 32]
		layout = triangles
		indices:
		[ind2; 24 bytes; offset = 80]
			0 1 2 3 
		vertices:
		[3320; 144 bytes; offset = 104]
			P=(0, 0, 0) N=(0, 0, 1) M=(0, 0)
			P=(0, 10, 0) N=(0, 0, 1) M=(0, 1)
			P=(10, 0, 0) N=(0, 0, 1) M=(1, 0)
			P=(10, 10, 0) N=(0, 0, 1) M=(1, 1)
		axes:
		[null: 0 bytes]

Rendering
---------

Loading the file is very simple and fast as data can be loaded almost directly into graphics memory. Use the `TaggedModelFormat::Reader` to load a model from a data buffer (typically loaded from disk using mmap).

	Model model = load_model(state->resource_loader, "base/models/baseplate.tmb");

	// Read header:
	auto header = model.reader.header();
	auto dictionary_block = model.reader.block_at_offset<TaggedModelFormat::Dictionary>(header->top_offset);

	auto offset = dictionary_block->offset_named("baseplate");
	auto mesh_block = model.reader.block_at_offset<TaggedModelFormat::Mesh>(offset);

	// mesh_block provides data offsets for indices, vertices and other mesh-related data:
	_mesh = load_mesh(model, mesh_block);
	
	_mesh->render();

The Dream framework includes an example of how to load and render models using the tagged model format.

Caveats
-------

The binary format is platform specific. It would be possible to adjust data ordering (e.g. endian) however this goes against the spirit of TMF: a simple loader that provides platform and application specific data that can be quickly loaded into the graphics card.

In the case where you need some form of platform independence, integrate the tmf-tool into your compiler toolchain to produce target-specific binary versions of your model data.

License
-------

Copyright (c) 2010, 2011 Samuel G. D. Williams. <http://www.oriontransfer.co.nz>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
