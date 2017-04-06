#ifndef PROBE_LIST_360_H__
#define PROBE_LIST_360_H__

///> probe list G70
#define PROBE_INFO_SIZE  128
#ifndef EMP_440
enum EProbeList360 {P35CV, P65VMC, P75LVS, P35MC, P65MC, PT30P16A,PT35D40JSNR, PT35D40JNDK};
unsigned char Probe35CV[PROBE_INFO_SIZE] = { //3.5CV
    // black-white
    '3',	'5',	'C',	'5',	'0',	'J',	'0',	'0',	'0',	'0',
    'C',	1,		28,		2,		56,		72,		0,		48,		0,		0,
    0,		40,		2,		50,		0,		70,		2,		20,		0,		80,
    2,		10,		1,		0,		1,		80,		1,		20,		1,		50,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0x5a,

    //color
    '3',	'.',	'5',	'C',	'V',	0,	0,		0,		0,		0,
    'C',	1,		28,		2,		56,		72,		0,		48,		0,		0,
    0,		40,		1,		20,		3,		0,		0,		54,		0,		66,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    'E',		'M',		'P',		0x5a,
};

unsigned char Probe65VMC[PROBE_INFO_SIZE] = { //6.5VMC //TV
    // black-white
    '6',   '5',    'C',    '1',    '0',    'J',    '0',	    '0',	'0',	'0',
    'C',    1,		28,		2,		56,		150,	0,		10,		0,		0,
    1,		0,		1,		20,     1,		20,     1,      0,		1,     50,
    0,      90,    1,     80,		0,      70,		0,      0,		0,		0,
    0,     0,		0,		0,		0,		0,		0,		0,		0,     0,
    0,     0,		0,		0,		0,		0,		0,		0,		0,     0,
    0,     0,		0,		0x5a,

    //color
    '6',	'.',	'5',	'V',	'M',	'C',	0,		0,		0,		0,
    't',	1,		28,		2,		56,		150,	0,		10,		0,		0,
    1,		0,		1,		80,		1,		20,		0,		98,		1,		16,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    'E',		'M',		'P',		0x5a,
};
unsigned char Probe75LVS[PROBE_INFO_SIZE] = { //7.5LVS
    // black-white
    '7',   '5',    'L',    '4',    '0',    'J',		'0',	'0',	'0',	'0',
    'L',    1,		28,		2,		56,		38,		0,		0,		0,		0,
    1,		0,		1,		20,		1,		30,		1,		0,		1,		50,
    0,		90,		1,		70,		0,		80,		2,		0,		0,		80,
    0,     0,		0,		0,		0,		0,		0,		0,		0,     0,
    0,     0,		0,		0,		0,		0,		0,		0,		0,     0,
    0,     0,		0,		0x5a,

    //color
    '7',	'.',	'5',	'L',	'V',	'S',	0,		0,		0,		0,
    'l',	1,		28,		2,		56,		38,		0,		0,		0,		 0,
    1,		0,		2,		40,		1,		20,		1,		18,		1,		44,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    'E',		'M',		'P',		0x5a,
};
#endif
unsigned char Probe35MC[PROBE_INFO_SIZE] = { //3.5MC
    // black-white
    '3',   '5',    'C',    '2',    '0',    'G',		'0',	'0',	'0',	'0',
    'C',    0,		96,		1,		92,		88,		0,		21,		0,		0,
    0,		50,		2,		50,		0,		70,		2,		20,		1,		0,
    1,		80,		0,		0,		0,		0,		0,		0,		0,		0,
    0,     0,		0,		0,		0,		0,		0,		0,		0,     0,
    0,     0,		0,		0,		0,		0,		0,		0,		0,     0,
    0,     0,		0,		0x5a,

    //color
    '3',	'.',	'5',	'M',	'C',	0,	    0,		0,		0,		0,
    'N',	0,		96,		1,		92,		88,		0,		21,		0,		 0,
    0,		50,		1,		20,		3,		0,		0,		54,		0,		66,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    'E',		'M',		'P',		0x5a,
};
unsigned char Probe65MC[PROBE_INFO_SIZE] = { //6.5MC
    // black-white
    '6',	'5',	'C',	'1',	'5',	'G',	'0',	'0',	'0',	'0',
    'C',	0,		80,		1,		60,		98,		0,		15,		0,		0,
    1,		20,		1,		0,		1,		50,		0,		90,		1,		80,
    0,		70,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0x5a,

    //color-65C15D
    '6',	'.',	'5',	'M',	'C',	0,	0,		0,		0,		0,
    'n',	0,		80,		1,		60,		98,		0,		15,		0,		 0,
    1,		20,		1,		80,		1,		20,		0,		98,		1,		16,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    'E',		'M',		'P',		0x5a,
};

//30P16A  同340
#if 0
unsigned char Probe35D40J[PROBE_INFO_SIZE] = { //volume convex
    // black-white
    '3',	'5',	'D',	'4',	'0',	'J',	'0',	'0',	'0',	'0',
    'C',	1,		28,		2,		56,		68,		20,		40,		0,		0,
    0,		60,		2,		30,		0,		70,		2,		20,		0,		80,
    2,		10,		0,		90,		2,		0,		1,		0,		1,		80,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0x5a,

    //color
    //'V',	'1',	'5',	'1',	'9',	0,	    0,		0,		0,		0,
    '3',	'5',	'D',	'4',	'0',	'J',	    0,		0,		0,		0,
    'V',	1,		28,		2,		56,		68,		20,		40,		0,		0,
    0,		60,		1,		0,		3,		0,		0,		54,		0,		66,
    0,		0,		0,		0,		0,		0,		18,		3,		75,		30,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0x5a,
};
#endif

#endif
