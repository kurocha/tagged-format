# Blender Exporter

The Blender Exporter provides a script to export Mesh data to Tagged Format text.

## Install

From within Blender, click `File` -> `User Preferences`. On the window that opens, select the `Addons` tab. At the bottom of that window, click `Install Addon...` and find the file `io_tagged_format.py` which is included as part of the Blender exporter. Once the addon has been installed, enable it by checking the checkbox on the right hand side. Finally, click `Save As Default` otherwise the plugin will not be loaded again when you restart Blender.

## Export

To export a mesh, make sure it is the currently selected mesh. Then, select `File` -> `Export` -> `Tagged Format (.tft)`.

The exporter currently supports exporting multiple objects and cameras. Each mesh object has vertices exported as either `3p3n2m` or `3p3n` format.

The top level object will be a dictionary with names referencing the selected meshes. You can change the names of various objects from Blender's Outliner.

It is recommended that you modify the exporter code for your exact requirements.

### Axes

I often use attachment points to connect and animate models. In parlance, these are called Axes. To add these from Blender, from the `Add` menu, select `Empty`. This adds a 3D axis which can be manipulated. During export, these axes will be bundled along with the mesh data which can then be accessed programatically.

## License

Released under the MIT license.

Copyright, 2016, by [Samuel G. D. Williams](http://www.codeotaku.com/samuel-williams).

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
