///
///	@file bdf2c.c		@brief BDF Font to C source convertor
///
///	Copyright (c) 2009, 2010 by Lutz Sammer.  All Rights Reserved.
///
///	Contributor(s):
///
///	License: AGPLv3
///
///	This program is free software: you can redistribute it and/or modify
///	it under the terms of the GNU Affero General Public License as
///	published by the Free Software Foundation, either version 3 of the
///	License.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU Affero General Public License for more details.
///
///	$Id$
//////////////////////////////////////////////////////////////////////////////

///
///	@mainpage
///		bdf2c - converts bdf font files into C include files.
///
///		The Bitmap Distribution Format (BDF) is a file format for
///		storing bitmap fonts. The content is presented as a text file
///		that is intended to be human and computer readable.
///
///	BDF input:
///	@code
///	STARTCHAR A
///	ENCODING 65
///	SWIDTH 568 0
///	DWIDTH 8 0
///	BBX 8 13 0 -2
///	BITMAP
///	00
///	38
///	7C
///	C6
///	C6
///	C6
///	FE
///	C6
///	C6
///	C6
///	C6
///	00
///	00
///	ENDCHAR
///	@endcode
///
///	The result looks like this:
///	@code
///	//  65 $41 'A'
///	//	width 8, bbx 0, bby -2, bbw 8, bbh 13
///	    ________,
///	    __XXX___,
///	    _XXXXX__,
///	    XX___XX_,
///	    XX___XX_,
///	    XX___XX_,
///	    XXXXXXX_,
///	    XX___XX_,
///	    XX___XX_,
///	    XX___XX_,
///	    XX___XX_,
///	    ________,
///	    ________,
///	@endcode
///

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define VERSION "4"			///< version of this application

//////////////////////////////////////////////////////////////////////////////

int Outline;				///< true generate outlined font
int SmartMatrix;            // modify output to be used in the SmartMatrix library

//////////////////////////////////////////////////////////////////////////////

///
///	Create our header file.
///
///	@param out	file stream for output
///
void CreateFontHeaderFile(FILE * out)
{
    register int i;

    fprintf(out,
	"// (c) 2009, 2010 Lutz Sammer, License: AGPLv3\n\n"
	"\t/// bitmap font structure\n" "struct bitmap_font {\n"
	"\tunsigned char Width;\t\t///< max. character width\n"
	"\tunsigned char Height;\t\t///< character height\n"
	"\tunsigned short Chars;\t\t///< number of characters in font\n"
	"\tconst unsigned char *Widths;\t///< width of each character\n"
	"\tconst unsigned short *Index;\t///< encoding to character index\n"
	"\tconst unsigned char *Bitmap;\t///< bitmap of all characters\n"
	"};\n\n");

    fprintf(out, "\t/// @{ defines to have human readable font files\n");
    for (i = 0; i < 256; ++i) {
	fprintf(out, "#define %c%c%c%c%c%c%c%c 0x%02X\n",
	    (i & 0x80) ? 'X' : '_', (i & 0x40) ? 'X' : '_',
	    (i & 0x20) ? 'X' : '_', (i & 0x10) ? 'X' : '_',
	    (i & 0x08) ? 'X' : '_', (i & 0x04) ? 'X' : '_',
	    (i & 0x02) ? 'X' : '_', (i & 0x01) ? 'X' : '_', i);
    }
    fprintf(out, "\t/// @}\n");
}

//////////////////////////////////////////////////////////////////////////////

///
///	Print header for c file.
///
///	@param out	file stream for output
///	@param name	font variable name in C source file
///
void Header(FILE * out, const char *name)
{
    char * headername;
    if(!SmartMatrix)
        headername = "font";
    else
        headername = "MatrixFontCommon";

    fprintf(out,
	"// Created from bdf2c Version %s, (c) 2009, 2010 by Lutz Sammer\n"
	"//\tLicense AGPLv3: GNU Affero General Public License version 3\n"
	"\n#include \"%s.h\"\n\n", VERSION, headername);

    fprintf(out,
	"\t/// character bitmap for each encoding\n"
	"static const unsigned char __%s_bitmap__[] = {\n", name);
}

///
///	Print width table for c file
///
///	@param out		file stream for output
///	@param name		font variable name in C source file
///	@param width_table	width table read from BDF file
///	@param chars		number of characters in width table
///
void WidthTable(FILE * out, const char *name, const unsigned *width_table,
    int chars)
{
    fprintf(out, "};\n\n");

    fprintf(out,
	"\t/// character width for each encoding\n"
	"static const unsigned char __%s_widths__[] = {\n", name);
    while (chars--) {
	fprintf(out, "\t%u,\n", *width_table++);
    }
}

///
///	Print encoding table for c file
///
///	@param out		file stream for output
///	@param name		font variable name in C source file
///	@param encoding_table	encoding table read from BDF file
///	@param chars		number of characters in encoding table
///
void EncodingTable(FILE * out, const char *name,
    const unsigned *encoding_table, int chars)
{
    fprintf(out, "};\n\n");

    fprintf(out,
	"\t/// character encoding for each index entry\n"
	"static const unsigned short __%s_index__[] = {\n", name);
    while (chars--) {
	fprintf(out, "\t%u,\n", *encoding_table++);
    }
}

///
///	Print footer for c file.
///
///	@param out		file stream for output
///	@param name		font variable name in C source file
///	@param width		character width of font
///	@param height		character height of font
///	@param chars		number of characters in font
///
void Footer(FILE * out, const char *name, int width, int height, int chars)
{
    fprintf(out, "};\n\n");
    fprintf(out,
	"\t/// bitmap font structure\n" "const struct bitmap_font %s = {\n",
	name);
    fprintf(out, "\t.Width = %d, .Height = %d,\n", width, height);
    fprintf(out, "\t.Chars = %d,\n", chars);

    if(!SmartMatrix)
        fprintf(out, "\t.Widths = __%s_widths__,\n", name);
    else
        fprintf(out, "\t.Widths = 0,\n");

    fprintf(out, "\t.Index = __%s_index__,\n", name);
    fprintf(out, "\t.Bitmap = __%s_bitmap__,\n", name);
    fprintf(out, "};\n\n");
}

///
///	Dump character.
///
///
void DumpCharacter(FILE * out, unsigned char *bitmap, int fontwidth, int fontheight, int fontyoffset, int charheight, int charyoffset)
{
    int x;
    int y;
    int c;

    // how many rows from the top of the font bounding box is the top of this character?
    int yoffset = fontheight - charheight + (fontyoffset - charyoffset);

    for (y = 0; y < fontheight; ++y) {
	fputc('\t', out);
	for (x = 0; x < fontwidth; x += 8) {
        // if current row is above or below the bitmap, output a blank row
        if(y < yoffset || y > yoffset + charheight)
            c = 0;
        else
            c = bitmap[(y - yoffset) * ((fontwidth + 7) / 8) + x / 8];

	    //printf("%d = %d\n", y * ((width+7)/8) + x/8, c);
	    if (c & 0x80) {
		fputc('X', out);
	    } else {
		fputc('_', out);
	    }
	    if (c & 0x40) {
		fputc('X', out);
	    } else {
		fputc('_', out);
	    }
	    if (c & 0x20) {
		fputc('X', out);
	    } else {
		fputc('_', out);
	    }
	    if (c & 0x10) {
		fputc('X', out);
	    } else {
		fputc('_', out);
	    }
	    if (c & 0x08) {
		fputc('X', out);
	    } else {
		fputc('_', out);
	    }
	    if (c & 0x04) {
		fputc('X', out);
	    } else {
		fputc('_', out);
	    }
	    if (c & 0x02) {
		fputc('X', out);
	    } else {
		fputc('_', out);
	    }
	    if (c & 0x01) {
		fputc('X', out);
	    } else {
		fputc('_', out);
	    }
	    fputc(',', out);
	}
	fputc('\n', out);
    }
}

///
///	Hex ascii to integer
///
///	@param p	hex input character (0-9a-fA-F)
///
///	@returns converted integer
///
static inline int Hex2Int(const char *p)
{
    if (*p <= '9') {
	return *p - '0';
    } else if (*p <= 'F') {
	return *p - 'A' + 10;
    } else {
	return *p - 'a' + 10;
    }
}

///
///	Rotate bitmap.
///
///	@param bitmap	input bitmap
///	@param shift	rotate counter (0-7)
///	@param width	character width
///	@param height	character height
///
void RotateBitmap(unsigned char *bitmap, int shift, int width, int height)
{
    int x;
    int y;
    int c;
    int o;

    if (shift < 0 || shift > 7) {
	fprintf(stderr, "This shift isn't supported\n");
	exit(-1);
    }

    for (y = 0; y < height; ++y) {
	o = 0;
	for (x = 0; x < width; x += 8) {
	    c = bitmap[y * ((width + 7) / 8) + x / 8];
	    bitmap[y * ((width + 7) / 8) + x / 8] = c >> shift | o;
	    o = c << (8 - shift);
	}
    }
}

///
///	Outline character.  Create an outline font from normal fonts.
///
///	@param bitmap	input bitmap
///	@param width	character width
///	@param height	character height
///
void OutlineCharacter(unsigned char *bitmap, int width, int height)
{
    int x;
    int y;
    unsigned char *outline;

    outline = alloca(((width + 7) / 8) * height);
    memset(outline, 0, ((width + 7) / 8) * height);
    for (y = 0; y < height; ++y) {
	for (x = 0; x < width; ++x) {
	    // Bit not set check surroundings
	    if (~bitmap[y * ((width + 7) / 8) + x / 8] & (0x80 >> x % 8)) {
		// Upper row bit was set
		if (y
		    && bitmap[(y - 1) * ((width + 7) / 8) +
			x / 8] & (0x80 >> x % 8)) {
		    outline[y * ((width + 7) / 8) + x / 8] |= (0x80 >> x % 8);
		    // Previous bit was set
		} else if (x
		    && bitmap[y * ((width + 7) / 8) + (x -
			    1) / 8] & (0x80 >> (x - 1) % 8)) {
		    outline[y * ((width + 7) / 8) + x / 8] |= (0x80 >> x % 8);
		    // Next bit was set
		} else if (x < width - 1
		    && bitmap[y * ((width + 7) / 8) + (x +
			    1) / 8] & (0x80 >> (x + 1) % 8)) {
		    outline[y * ((width + 7) / 8) + x / 8] |= (0x80 >> x % 8);
		    // below row was set
		} else if (y < height - 1
		    && bitmap[(y + 1) * ((width + 7) / 8) +
			x / 8] & (0x80 >> x % 8)) {
		    outline[y * ((width + 7) / 8) + x / 8] |= (0x80 >> x % 8);
		}
	    }
	}
    }
    memcpy(bitmap, outline, ((width + 7) / 8) * height);
}

///
///	Read BDF font file.
///
///	@param bdf	file stream for input (bdf file)
///	@param out	file stream for output (C source file)
///	@param name	font variable name in C source file
///
///	@todo bbx isn't used to correct character position in bitmap
///
void ReadBdf(FILE * bdf, FILE * out, const char *name)
{
    char linebuf[1024];
    char *s;
    char *p;
    int fontboundingbox_width;
    int fontboundingbox_height;
    int fontboundingbox_xoff;
    int fontboundingbox_yoff;
    int chars;
    int i;
    int j;
    int n;
    int scanline;
    char charname[1024];
    int encoding;
    int bbx;
    int bby;
    int bbw;
    int bbh;
    int width;
    unsigned *width_table;
    unsigned *encoding_table;
    unsigned char *bitmap;

    fontboundingbox_width = 0;
    fontboundingbox_height = 0;
    fontboundingbox_xoff = 0;
    fontboundingbox_yoff = 0;
    chars = 0;
    for (;;) {
	if (!fgets(linebuf, sizeof(linebuf), bdf)) {	// EOF
	    break;
	}
	if (!(s = strtok(linebuf, " \t\n\r"))) {	// empty line
	    break;
	}
	// printf("token:%s\n", s);
	if (!strcasecmp(s, "FONTBOUNDINGBOX")) {
	    p = strtok(NULL, " \t\n\r");
	    fontboundingbox_width = atoi(p);
	    p = strtok(NULL, " \t\n\r");
	    fontboundingbox_height = atoi(p);
        p = strtok(NULL, " \t\n\r");
        fontboundingbox_xoff = atoi(p);
        p = strtok(NULL, " \t\n\r");
        fontboundingbox_yoff = atoi(p);
	} else if (!strcasecmp(s, "CHARS")) {
	    p = strtok(NULL, " \t\n\r");
	    chars = atoi(p);
	    break;
	}
    }
    /*
       printf("%d * %dx%d\n", chars, fontboundingbox_width,
       fontboundingbox_height);
     */
    //
    //	Some checks.
    //
    if (fontboundingbox_width <= 0 || fontboundingbox_height <= 0) {
	fprintf(stderr, "Need to know the character size\n");
	exit(-1);
    }
    if (chars <= 0) {
	fprintf(stderr, "Need to know the number of characters\n");
	exit(-1);
    }
    if (Outline) {			// Reserve space for outline border
	fontboundingbox_width++;
	fontboundingbox_height++;
    }
    //
    //	Allocate tables
    //
    width_table = malloc(chars * sizeof(*width_table));
    if (!width_table) {
	fprintf(stderr, "Out of memory\n");
	exit(-1);
    }
    encoding_table = malloc(chars * sizeof(*encoding_table));
    if (!encoding_table) {
	fprintf(stderr, "Out of memory\n");
	exit(-1);
    }
    /*	FIXME: needed for proportional fonts.
       offset_table = malloc(chars * sizeof(*offset_table));
       if (!offset_table) {
       fprintf(stderr, "Out of memory\n");
       exit(-1);
       }
     */
    bitmap =
	malloc(((fontboundingbox_width + 7) / 8) * fontboundingbox_height);
    if (!bitmap) {
	fprintf(stderr, "Out of memory\n");
	exit(-1);
    }

    Header(out, name);

    scanline = -1;
    n = 0;
    encoding = -1;
    bbx = 0;
    bby = 0;
    bbw = 0;
    bbh = 0;
    width = INT_MIN;
    strcpy(charname, "unknown character");
    for (;;) {
	if (!fgets(linebuf, sizeof(linebuf), bdf)) {	// EOF
	    break;
	}
	if (!(s = strtok(linebuf, " \t\n\r"))) {	// empty line
	    break;
	}
	// printf("token:%s\n", s);
	if (!strcasecmp(s, "STARTCHAR")) {
	    p = strtok(NULL, " \t\n\r");
	    strcpy(charname, p);
	} else if (!strcasecmp(s, "ENCODING")) {
	    p = strtok(NULL, " \t\n\r");
	    encoding = atoi(p);
	} else if (!strcasecmp(s, "DWIDTH")) {
	    p = strtok(NULL, " \t\n\r");
	    width = atoi(p);
	} else if (!strcasecmp(s, "BBX")) {
	    p = strtok(NULL, " \t\n\r");
	    bbw = atoi(p);
	    p = strtok(NULL, " \t\n\r");
	    bbh = atoi(p);
	    p = strtok(NULL, " \t\n\r");
	    bbx = atoi(p);
	    p = strtok(NULL, " \t\n\r");
	    bby = atoi(p);
	} else if (!strcasecmp(s, "BITMAP")) {
	    fprintf(out, "// %3d $%02x '%s'\n", encoding, encoding, charname);
	    fprintf(out, "//\twidth %d, bbx %d, bby %d, bbw %d, bbh %d\n",
		width, bbx, bby, bbw, bbh);

	    if (n == chars) {
		fprintf(stderr, "Too many bitmaps for characters\n");
		exit(-1);
	    }
	    if (width == INT_MIN) {
		fprintf(stderr, "character width not specified\n");
		exit(-1);
	    }
	    //
	    //	Adjust width based on bounding box
	    //
	    if (bbx < 0) {
		width -= bbx;
		bbx = 0;
	    }
	    if (bbx + bbw > width) {
		width = bbx + bbw;
	    }
	    if (Outline) {		// Reserve space for outline border
		++width;
	    }
	    width_table[n] = width;
	    encoding_table[n] = encoding;
	    ++n;
	    if (Outline) {		// Leave first row empty
		scanline = 1;
	    } else {
		scanline = 0;
	    }
	    memset(bitmap, 0,
		((fontboundingbox_width + 7) / 8) * fontboundingbox_height);
	} else if (!strcasecmp(s, "ENDCHAR")) {
	    if (bbx) {
		RotateBitmap(bitmap, bbx, fontboundingbox_width,
		    fontboundingbox_height);
	    }
	    if (Outline) {
		RotateBitmap(bitmap, 1, fontboundingbox_width,
		    fontboundingbox_height);
		OutlineCharacter(bitmap, fontboundingbox_width,
		    fontboundingbox_height);
	    }
	    DumpCharacter(out, bitmap, fontboundingbox_width,
		fontboundingbox_height, fontboundingbox_yoff, bbh, bby);
	    scanline = -1;
	    width = INT_MIN;
	} else {
	    if (scanline >= 0) {
		p = s;
		j = 0;
		while (*p) {
		    i = Hex2Int(p);
		    ++p;
		    if (*p) {
			i = Hex2Int(p) | i * 16;
		    } else {
			bitmap[j + scanline * ((fontboundingbox_width +
				    7) / 8)] = i;
			break;
		    }
		    /* printf("%d = %d\n",
		       j + scanline * ((fontboundingbox_width + 7)/8), i); */
		    bitmap[j + scanline * ((fontboundingbox_width + 7) / 8)] =
			i;
		    ++j;
		    ++p;
		}
		++scanline;
	    }
	}
    }

    // Output width table for proportional font.
    if(!SmartMatrix)
        WidthTable(out, name, width_table, chars);
    // FIXME: Output offset table for proportional font.
    // OffsetTable(out, name, offset_table, chars);
    // Output encoding table for utf-8 support
    EncodingTable(out, name, encoding_table, chars);

    Footer(out, name, fontboundingbox_width, fontboundingbox_height, chars);
}

//////////////////////////////////////////////////////////////////////////////

///
///	Print version
///
void PrintVersion(void)
{
    printf("bdf2c Version %s, (c) 2009, 2010 by Lutz Sammer\n"
	"\tLicense AGPLv3: GNU Affero General Public License version 3\n",
	VERSION);
}

///
///	Print usage
///
void PrintUsage(void)
{
    printf("Usage: bdf2c [OPTIONs]\n"
	"\t-h or -?\tPrints this short page on stdout\n"
	"\t-b\tRead bdf file from stdin, write to stdout\n"
	"\t-c\tCreate font header on stdout\n"
	"\t-C file\tCreate font header file\n"
	"\t-n name\tName of c font variable (place it before -b)\n"
	"\t-O\tCreate outline for the font.\n");
    printf("\n\tOnly idiots print usage on stderr\n");
}

///
///	Main test program for bdf2c.
///
///
///	@param argc	number of arguments
///	@param argv	arguments vector
///
int main(int argc, char *const argv[])
{
    const char *name;

    name = "font";			// default variable name
    //
    //	Parse arguments.
    //
    for (;;) {
	switch (getopt(argc, argv, "bcC:n:hOs?-")) {
	    case 'b':			// bdf file name
		ReadBdf(stdin, stdout, name);
		continue;
	    case 'c':			// create header file
		CreateFontHeaderFile(stdout);
		break;
	    case 'C':			// create header file
	    {
		FILE *out;

		if (!(out = fopen(optarg, "w"))) {
		    fprintf(stderr, "Can't open file '%s': %s\n", optarg,
			strerror(errno));
		    exit(-1);
		}
		CreateFontHeaderFile(out);
		fclose(out);
	    }
		continue;
	    case 'n':
		name = optarg;
		continue;
	    case 'O':
		Outline = 1;
		continue;
        case 's':
        SmartMatrix = 1;
        continue;

	    case EOF:
		break;
	    case '?':
	    case 'h':			// help usage
		PrintVersion();
		PrintUsage();
		exit(0);
	    case '-':
		fprintf(stderr, "We need no long options\n");
		PrintUsage();
		exit(-1);
	    case ':':
		PrintVersion();
		fprintf(stderr, "Missing argument for option '%c'\n", optopt);
		exit(-1);
	    default:
		PrintVersion();
		fprintf(stderr, "Unkown option '%c'\n", optopt);
		exit(-1);
	}
	break;
    }
    while (optind < argc) {
	fprintf(stderr, "Unhandled argument '%s'\n", argv[optind++]);
    }

    return 0;
}
