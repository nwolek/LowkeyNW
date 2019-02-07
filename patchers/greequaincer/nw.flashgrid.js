/*
flash grid to sit behind matrixctrl
by Nathan Wolek

2019.02.07 - begun

arguments: foreground(RGB) background(RGB)

*/

inlets = 1;
mgraphics.init();
mgraphics.relative_coords = 0;
mgraphics.autofill = 0;

// color variables
var vbrgb = [0.200, 0.200, 0.200, 1.000]; // background color
var vfrgb = [0.9,0.3,0.33];	// flashing gridspace color

// tracking values
var current_flash = 10;
var vwidth = 258;
var vheight = 66;
var vcolumns = 16;
var vrows = 4;
var flashwidth = vwidth / vcolumns;
var flashheight = vheight / vrows;
var flashcolumn = 10;
var flashrow = 0;

// set colors if specified by arguments
if (jsarguments.length>1)
	vfrgb[0] = jsarguments[1]/255.;
if (jsarguments.length>2)
	vfrgb[1] = jsarguments[2]/255.;
if (jsarguments.length>3)
	vfrgb[2] = jsarguments[3]/255.;
if (jsarguments.length>4)
	vbrgb[0] = jsarguments[4]/255.;
if (jsarguments.length>5)
	vbrgb[1] = jsarguments[5]/255.;
if (jsarguments.length>6)
	vbrgb[2] = jsarguments[6]/255.;

function paint() {
	//mgraphics.move_to(200,100);
	//set_source_rgba(200, 200, 200, 200);
	//mgraphics.rectangle(0,0,vwidth,vheight);
	//mgraphics.fill();
	
	//set_source_rgb(vfrgb[0],vfrgb[1],vfrgb[2]);
	mgraphics.rectangle(flashrow*flashheight,flashcolumn*flashwidth,flashwidth,flashheight);
	mgraphics.fill();
}
	
function bang()
{
	mgraphics.redraw();
	//re();
}

function msg_int(v)
{
	current_flash = v;
	flashcolumn = current_flash / vcolumns;
	flashrow = current_flash % vcolumns;
	bang();
}