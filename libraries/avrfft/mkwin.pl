# Window table generator

	$fftn = 128;	# FFT samples

	if(!open(WHA, "> win.txt")) { exit; }
	$cn = 16;		# Number of items per row
	$re = 32767;	# Range
	$pi = 3.14159;


	$cn--;
	print WHA ";------------------------------------------------;\n";
	print WHA "; Various window tables for $fftn point int16 FFT.\n";
	print WHA ";------------------------------------------------;\n";

	# Hamming Window
	print WHA "\nhamming:\n";
	for($p = 0; $p < $fftn; $p++) {
		$n = int((0.54 - 0.46*cos(2*$pi * $p/$fftn)) * $re);
		&put_val;
	}

	# Hann Window
	print WHA "\nhann:\n";
	for($p = 0; $p < $fftn; $p++) {
		$n = int((0.5 - 0.5*cos(2*$pi * $p/$fftn)) * $re);
		&put_val;
	}

	# Blackman Window
	print WHA "\nblackman:\n";
	for($p = 0; $p < $fftn; $p++) {
		$n = int((0.42 - 0.5*cos(2*$pi * $p/$fftn) + 0.08*cos(4*$pi * $p/$fftn)) * $re);
		&put_val;
	}

	# Flattop Window type 1
	print WHA "\nflattop1:\n";
	for($p = 0; $p < $fftn; $p++) {
		if((4*$pi * ($p-($fftn/2)) / $fftn) == 0) { $sx = 1; }
		else { $sx = sin(4*$pi * ($p-($fftn/2)) / $fftn) / (4*$pi * ($p-($fftn/2)) / $fftn); }
		$n = int((0.54 - 0.46*cos(2*$pi * $p/$fftn)) * $sx * $re);
		&put_val;
	}

	# Flattop Window type 2
	print WHA "\nflattop2:\n";
	for($p = 0; $p < $fftn; $p++) {
		$n = int((0.2810639 - 0.5208972*cos(2*$pi * $p/$fftn) + 0.1980399*cos(4*$pi * $p/$fftn)) * $re);
		&put_val;
	}

	# Rectanguler Window
	print WHA "\nrectanguler:\n";
	for($p = 0; $p < $fftn; $p++) {
		$n = $re;
		&put_val;
	}

	exit;


sub put_val
{
		if(($p & $cn) == 0) { print WHA "\t.dc.w "; }
		print WHA $n;
		if(($p & $cn) != $cn) { print WHA ", "; }
		if(($p & $cn) == $cn) { print WHA "\n"; }
}
