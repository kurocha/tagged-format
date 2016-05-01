
Block = function(buffer, offset) {
	// Block:
	// byte 0:4 -> Tag
	// byte 4:12 -> Size (64-bits)
	// ... remainder of block ...
	
	this.buffer = buffer;
	this.offset = offset;
	
	// Initially we don't know the size of the block, but it must at least contain the tag and size information:
	this.data = new DataView(this.buffer, this.offset, 12);
	
	this.tagName = Block.Extract.fixedString(this, 0, 4);
	this.byteSize = Block.Extract.byteSize(this);
	
	// Reallocate the dataview with the updated size:
	this.data = new DataView(this.buffer, this.offset, this.byteSize);
	
	// Extract specific data from the block structure:
	var blockExtractor = Block.Tags[this.tagName];
	
	if (blockExtractor)
		Block.Tags[this.tagName](this);
	
	return this;
}

Block.OutOfRangeError = function(message) {
	this.name = "Out of range error";
	this.message = message;
	
	return this;
}

Block.Extract = {}

Block.Extract.fixedString = function(block, offset, length) {
	var value = "";
	
	for (var i = 0; i < length; i++) {
		var character = block.data.getInt8(offset + i);
		
		// Stop if we hit a NUL byte.
		if (character == 0) break;
		
		value += String.fromCharCode(character);
	}
	
	return value;
}

Block.Extract.byteSize = function(block) {
	if (block.data.getUint32(4 + 4, true) != 0)
		throw new Block.OutOfRangeError("Size greater than 32-bits");
	
	return block.data.getUint32(4, true);
}

Block.Extract.magic = function(block) {
	// Header:
	// byte 12:16 -> Magic
	// byte 16:24 -> Top offset
	
	return block.data.getUint32(12);
}

Block.Extract.offsets = function(block, offset, count) {
	count = count || 1;
	
	// For the purposes of this code we assume offsets and lengths are actually 32-bit, which is incorrect. JavaScript can't handle 64-bit integers. We throw an exception if this assumption is faulty as we can't reasonably do anything else.
	var offsetBuffer = new Uint32Array(block.buffer, block.offset + offset, (count || 1) * 2);
	
	var offsets = [];
	for (var i = 0; i < count; i += 1) {
		offsets.push(offsetBuffer[i*2]);
		//console.log("Offset:", offsetBuffer[i], offsetBuffer[i+1]);
		
		if (offsetBuffer[i*2+1] != 0) {
			throw new Block.OutOfRangeError("Offset greater than 32-bits");
		}
	}
	
	return offsets;
}

Block.Extract.elements = function(block, offset, elementSize, callback) {
	var arraySize = block.byteSize - offset;
	var count = arraySize / elementSize;
	
	for (var i = 0; i < count; i += 1) {
		callback(offset + elementSize * i, i);
	}
}

Block.Tags = {};

Block.Header = function(block) {
	block.magic = Block.Extract.magic(block);
	block.topOffset = Block.Extract.offsets(block, 16)[0];
}

Block.Tags['HDR3'] = Block.Header;

Block.OffsetTable = function(block) {
	block.offsetTable = {}
	
	Block.Extract.elements(block, 12, 32 + 8, function(offset) {
		var name = Block.Extract.fixedString(block, offset, 32);
		var offset = Block.Extract.offsets(block, offset + 32, 1)[0];
		
		block.offsetTable[name] = offset;
	});
}

Block.Tags['#OFS'] = Block.OffsetTable;

Block.Mesh = function(block) {
	block.layout = (new Uint32Array(block.buffer, block.offset + 12))[0];
	
	var offsets = Block.Extract.offsets(block, 16, 4)
	
	block.indicesOffset = offsets[0];
	block.verticesOffset = offsets[1];
	block.axesOffset = offsets[2];
	block.metadataOffset = offsets[3];
}

Block.Tags['MESH'] = Block.Mesh;

Block.Index16 = function(block) {
	block.elements = new Uint16Array(block.buffer, block.offset + 12, (block.byteSize - 12) / 2);
	block.dataType = 'uint16';
}

Block.Tags['IN16'] = Block.Index16;

Block.VertexP3N3M2 = function(block) {
	block.format = [
		{name: 'position', dataType: 'float32', count: 3},
		{name: 'normal', dataType: 'float32', count: 3},
		{name: 'mapping', dataType: 'float32', count: 2},
	];
	
	block.vertexSize = 8*4;
	
	block.elements = new DataView(block.buffer, block.offset + 12, block.byteSize - 12);
}

Block.Tags['3320'] = Block.VertexP3N3M2;

Block.VertexP3N3M2B4 = function(block) {
	block.format = [
		{name: 'position', dataType: 'float32', count: 3},
		{name: 'normal', dataType: 'float32', count: 3},
		{name: 'mapping', dataType: 'float32', count: 2},
		{name: 'bones', dataType: 'uint8', count: 4},
		{name: 'weights', dataType: 'float32', count: 4}
	];
	
	block.vertexSize = 12*4 + 4*1;
	
	block.elements = new DataView(block.buffer, block.offset + 12, block.byteSize - 12);
}

Block.Tags['3324'] = Block.VertexP3N3M2B4;

Block.Skeleton = function(block) {
	var offsets = Block.Extract.offsets(block, 16, 2)
	
	block.bonesOffset = offsets[0];
	block.sequencesOffset = offsets[1];
}

Block.Tags['SKEL'] = Block.Skeleton;
/*
Block.Skeleton.Bone = function(block) {
	block.bones = [];
	
	Block.Extract.elements(block, 12, 32+4+16*4, function(offset) {
		var name = Block.Extract.fixedString(block, offset, 32);
		var parent = Block.Extract
	});
	
	
	// An optional bone name.
	FixedString<> name;

	// The index of the parent node.
	alignas(4) BoneID parent;

	/// The bind pose transform from the parent to the child bone.
	float32 transform[16];
}

Block.Tags['BONE'] = Block.SkeletonBone;

Block.Tags['ANIM'] = Block.SkeletonAnimation;

Block.Tags['KEYF'] = Block.SkeletonKeyFrame;
*/
Reader = function(buffer) {
	this.buffer = buffer;
	
	return this;
}

Reader.prototype.header = function() {
	return new Block(this.buffer, 0);
}

Reader.prototype.blockAtOffset = function(offset) {
	return new Block(this.buffer, offset);
}

Reader.load = function(url, callback) {
	var xhr = new XMLHttpRequest();

	xhr.open('GET', url, true);
	xhr.responseType = 'arraybuffer';

	xhr.onload = function(e) {
		callback(new Reader(this.response), url);
	};
	
	xhr.onerror = function(e) {
		callback(null, url)
	}
	
	xhr.send();
}

TaggedFormat = {
	glTypeForDataType: function(gl, typeName) {
		if (typeName == 'uint8')
			return gl.UNSIGNED_BYTE;
	
		if (typeName == 'int8')
			return gl.BYTE;
	
		if (typeName == 'uint16')
			return gl.UNSIGNED_SHORT;
	
		if (typeName == 'int16')
			return gl.SHORT;
	
		if (typeName == 'uint32')
			return gl.UNSIGNED_INT;

		if (typeName == 'int32')
			return gl.INT;
	
		if (typeName == 'float32')
			return gl.FLOAT;
	
		return null;
	},
	
	dataTypeLength: function(typeName) {
		if (typeName == 'uint8' || typeName == 'int8')
			return 1;

		if (typeName == 'uint16' || typeName == 'int16')
			return 2;
		
		if (typeName == 'uint32' || typeName == 'int32')
			return 4;
	
		if (typeName == 'float32')
			return 4;
	
		return 0;
	}
}

TaggedFormat.Model = function(gl, reader, mesh) {
	this.gl = gl;
	this.meshBlock = mesh;
	
	this.indicesBlock = reader.blockAtOffset(mesh.indicesOffset);
	this.verticesBlock = reader.blockAtOffset(mesh.verticesOffset);
	
	// create VBO and IBO
	this.ibo = gl.createBuffer();
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.ibo);
	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, this.indicesBlock.elements, gl.STATIC_DRAW);
	
	this.indicesType = TaggedFormat.glTypeForDataType(gl, this.indicesBlock.dataType)
	
	this.vbo = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, this.vbo);
	gl.bufferData(gl.ARRAY_BUFFER, this.verticesBlock.elements, gl.STATIC_DRAW);
	
	this.attributes = [];
	
	var offset = 0;
	for (var i in this.verticesBlock.format) {
		var attribute = this.verticesBlock.format[i];
		console.log(attribute);
		
		this.attributes.push({
			name: attribute.name,
			stride: this.verticesBlock.vertexSize,
			normalized: false,
			count: attribute.count,
			offset: offset,
			type: TaggedFormat.glTypeForDataType(gl, attribute.dataType)
		});
		
		offset = offset + TaggedFormat.dataTypeLength(attribute.dataType) * attribute.count;
	}
	
	return this;
}

TaggedFormat.Model.prototype.draw = function() {
	// setup interleaved VBO and IBO
	this.gl.bindBuffer(gl.ARRAY_BUFFER, this.vbo);
	this.gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.ibo);
	
	for (var i in this.attributes) {
		var attribute = this.attributes[i];
		
		this.gl.enableVertexAttribArray(i);
		this.gl.vertexAttribPointer(i, attribute.count, attribute.type, attribute.normalized, attribute.stride, attribute.offset);
	}

	// draw the buffer
	this.gl.drawElements(this.gl.TRIANGLES, this.indicesBlock.elements.length, this.indicesType, 0);
}