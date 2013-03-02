
Block = function(buffer, offset) {
	this.buffer = buffer;
	this.offset = offset;
	
	this.tagName = Block.Extract.fixedString(this, 0, 4);
	this.byteSize = Block.Extract.byteSize(this);
	
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
	var buffer = new Uint8Array(block.buffer, block.offset + offset, length);
	var value = "";
	
	for (var i = 0; i < length; i++) {
		// Stop if we hit a NUL byte.
		if (buffer[i] == 0) break;
		
		value += String.fromCharCode(buffer[i])
	}
	
	return value;
}

Block.Extract.byteSize = function(block) {
	var sizeBuffer = new Uint32Array(block.buffer, block.offset + 4, 1);
	
	return sizeBuffer[0];
}

Block.Extract.magic = function(block) {
	var magicBuffer = new Uint32Array(block.buffer, block.offset + 12, 1);
	
	return magicBuffer[0];
}

Block.Extract.offsets = function(block, offset, count) {
	count = count || 1;
	
	// For the purposes of this code we assume offsets and lengths are actually 32-bit, which is incorrect. JavaScript can't handle 64-bit integers. We throw an exception if this assumption is faulty as we can't reasonably do anything else.
	var offsetBuffer = new Uint32Array(block.buffer, block.offset + offset, (count || 1) * 2);
	
	var offsets = [];
	for (var i = 0; i < count; i += 1) {
		offsets.push(offsetBuffer[i]);
		//console.log("Offset:", offsetBuffer[i], offsetBuffer[i+1]);
		
		if (offsetBuffer[i+1] != 0) {
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

