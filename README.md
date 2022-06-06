# obj2js

> A command line tool for converting Alias Wavefront OBJ models into Javascript arrays.  These arrays can then be used directly with WebGL.

## Command Line Arguments
<pre>
Command Line Options:
     -i, --input               The input OBJ file.
     -o, --output              The output Javascript file.
     -v, --variable-name       The variable name for the JavaScript object.
    -xt, --exclude-textures    Exclude parsing of textures.
    -xn, --exclude-normals     Exclude parsing of normals.
     -h, --help                Show program usage.
</pre>
## Example
To convert a model of a submarine to a packed Javascript array, one can use the following command:
<pre>
obj2js -i sub.obj -v submarine -o ./sub.js
</pre>
The output that will be produced will include a variable, called `submarine`, that contains an object
literal where each key is the name of a group from the OBJ model.  The value for each key is a packed
array that can be used as an interleaved buffer in WebGL.

This is what the output will typically look like:
<pre>
export const submarine = {
	"leftTorpedo": [
		//      position-X,      position-Y,      position-Z,       texture-U,       texture-V,        normal-X,        normal-Y,        normal-Z

		//----------------------------------------------------------------------------------------------------------------------------------------------
		     +2.4418880939,   +0.2001850009,   +8.6402635574,   +0.8407999873,   -8.2299995422,   -0.5066000223,   +0.6973000169,   -0.5070999861,
			 ...
		     +2.4548199177,   -0.3702639937,  +12.3106203079,   +0.7470999956,   -9.3709001541,   +0.7346000075,   -0.6784999967,   -0.0000000000
	],
	"rightTorpedo": [
		//      position-X,      position-Y,      position-Z,       texture-U,       texture-V,        normal-X,        normal-Y,        normal-Z

		//----------------------------------------------------------------------------------------------------------------------------------------------
		     -2.5527191162,   +0.2001850009,   +8.6402635574,   +0.1594000012,   -8.2299995422,   +0.5066000223,   +0.6973000169,   -0.5070999861,
			 ...
		     -2.4201400280,   -0.5278009772,  +12.4418945312,   +0.2694999874,   -9.6858997345,   -0.7346000075,   -0.6784999967,   -0.0000000000
	],
	"rightProp": [
		//      position-X,      position-Y,      position-Z,       texture-U,       texture-V,        normal-X,        normal-Y,        normal-Z

		//----------------------------------------------------------------------------------------------------------------------------------------------
		     -2.5971369743,   -2.3391780853,  -12.2025508881,   -3.2283000946,  -13.3086996078,   -0.0024999999,   +0.1694000065,   +0.9854999781,
			 ...
		     -0.9460229874,   -1.3603270054,  -12.0654439926,   -3.4022998810,  -11.3542003632,   +0.3091000021,   -0.5320000052,   -0.7882999778
	],
	"divePlane": [
		//      position-X,      position-Y,      position-Z,       texture-U,       texture-V,        normal-X,        normal-Y,        normal-Z

		//----------------------------------------------------------------------------------------------------------------------------------------------
		     -4.6156439781,   +0.2010059953,   -9.4357690811,   -0.3531000018,   +0.1446000040,   -0.2179999948,   +0.9657999873,   +0.1402000040,
			 ...
		     +1.7919930220,   -0.1990430057,  -11.7112655640,   +0.8349999785,   +0.5665000081,   -0.6786000133,   -0.7074999809,   +0.1973000020
	],
	"hull": [
		//      position-X,      position-Y,      position-Z,       texture-U,       texture-V,        normal-X,        normal-Y,        normal-Z

		//----------------------------------------------------------------------------------------------------------------------------------------------
		     -2.0695068836,   -1.1689950228,   -7.5195469856,   -1.3229999542,   -0.1370999962,   -0.7986000180,   -0.6018999815,   +0.0001000000,
			 ...
		     +2.9505500793,   -2.4220430851,   -5.3518409729,   +2.1491999626,   -0.2763999999,   +0.9218000174,   -0.3871999979,   +0.0200999994
	],
	"leftProp": [
		//      position-X,      position-Y,      position-Z,       texture-U,       texture-V,        normal-X,        normal-Y,        normal-Z

		//----------------------------------------------------------------------------------------------------------------------------------------------
		     +2.2041239738,   -2.3391780853,  -12.2025508881,   +4.2579002380,   -1.0342999697,   -0.0024999999,   +0.1694000065,   +0.9854999781,
			 ...
		     +2.2135109901,   -3.6035590172,  -12.0698223114,   +4.2536001205,   -1.3152999878,   -0.6122999787,   -0.0000000000,   -0.7906000018
	]
};
</pre>

## License
<pre>
 Copyright (C) 2016-2022 by Joseph A. Marrero. http://www.manvscode.com/

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
</pre>
