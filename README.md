Tagged Format
=============

The Tagged Format is a simple generic text/binary format. It is primarily designed for storing and accessing
game data in binary format for quick loading.

It supports a variety of different purposes, but is primarily designed for geometry. It is designed to be a 
very simple and to act as a bridge between typical rendering formats (e.g. vertex arrays) and model editors 
(e.g. Blender).

[![Build Status](https://secure.travis-ci.org/ioquatix/tagged-format.png)](http://travis-ci.org/ioquatix/tagged-format)

Build and Install
-----------------

Use Teapot to build and install Tagged Format:

	$ cd tagged-format
	$ sudo gem install teapot
	$ teapot build Library/TaggedFormat variant-debug

Currently, only Mac OS X and Linux are supported using standards conformant C++11 compilers.

File Format
-----------

The Tagged file format is designed to be application specific rather than generic. Many generic file formats 
already exist (e.g. Wavefront OBJ) but they are cumbersome to use because they lack the ability to be 
customised without a significant amount of work.

Rather than try to design a one-size fits all general file format, you are encouraged to fork the Tagged format
on a per-application basis and build specific resource formats that match your exact needs. Cases where you 
might want to customise the behaviour include unusual vertex formats (e.g. multiple texture coordinates, 
additional per-vertex attributes, etc) or embedded binary data (e.g. animation data, texture data, shaders).

The Tagged binary format consists of a sequential set of blocks, where each block has a tag and size. The basic
file has a header which includes an offset to the top block. By default, a variety of block types are included
in the specification, including a `Table`, `Mesh`, `Axis`, a variety of vertex formats and `External` 
references.

The Tagged text format is well defined and is compiled to a binary format using the included `tagged-format-convert`, much 
like how an assembler converts symbolic code to binary machine code.

Tagged Format Tool
------------------

Included in this implementation is the `tagged-format-convert` which converts a text format into a binary format. The text 
format is primarily used as an export format and is typically converted into the binary format before being
used.

Here is an example of a simple 10x10 square made by two triangles:

	top: mesh triangles
		indices: array index16
			0 1 2 3
		end

		vertices: array vertex-p3n3m2
			 0  0 0 0 0 1 0 0
			 0 10 0 0 0 1 0 1
			10  0 0 0 0 1 1 0
			10 10 0 0 0 1 1 1
		end
	end

To assemble this to the binary TMF format, simply run:

	tagged-format-convert --text-to-binary input.tft output.tfb

You can check the results by running:

	$ tagged-format-convert --dump-binary output.tfb
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

Loading the file is very simple and fast as data can be loaded almost directly into graphics memory. Use the 
`TaggedFormat::Reader` to load a model from a data buffer (typically loaded from disk using `mmap`).

	Model model = load_model(state->resource_loader, "base/models/baseplate.tfb");

	// Read header:
	auto header = model.reader.header();
	auto dictionary_block = model.reader.block_at_offset<TaggedFormat::Table>(header->top_offset);

	auto offset = dictionary_block->offset_named("baseplate");
	auto mesh_block = model.reader.block_at_offset<TaggedFormat::Mesh>(offset);

	// mesh_block provides data offsets for indices, vertices and other mesh-related data:
	_mesh = load_mesh(model, mesh_block);
	
	_mesh->render();

The Dream framework includes an example of how to load and render models using the tagged model format.

Caveats
-------

The binary format is platform specific. It would be possible to adjust data ordering (e.g. endian) however 
this goes against the spirit of the Tagged binary format: a simple loader that provides platform and 
application specific data that can be quickly loaded into the graphics card.

In the case where you need some form of platform independence, there are two options: Either integrate the 
`tagged-format-convert` into your application development toolchain to produce target-specific binary 
versions of your model data, or integrate the parser directly into your application and cache binary versions
of the model data as required.

Future Work
-----------

- Provide target specific options to `tagged-format-convert` for generating platform specific (e.g. endian) binary data.
- Develop a simple Cocoa application for visualising the model data structure and rendering meshes.
- Complete animation related data-structures and provide examples.

## License

Released under the MIT license.

Copyright, 2012, by [Samuel G. D. Williams](http://www.codeotaku.com/samuel-williams).

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
