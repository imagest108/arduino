# Table generator

	$fftn = 1024;


	if(!open(WHA, "> tables.txt")) { exit; }
	$pi = 3.14159;
	$pi2 = $pi * 2;


	print WHA ";----------------------------------------------;\n";
	print WHA "; cos-sin and reorder table for $fftn point fft\n";
	print WHA ";----------------------------------------------;\n\n";


	print WHA "tbl_cos_sin:\n";
	$stp = $fftn / 2;
	for ($p = 0; $p < $stp; $p++) {
		$s = int(sin($p * $pi / $stp) * 32767);
		$c = int(cos($p * $pi / $stp) * 32767);
		if (($p & 7) == 0) {
			print WHA "\t.dc.w\t$c, $s";
		} else {
			print WHA ", $c, $s";
		}
		if (($p & 7) == 7) { print WHA "\n"; }
	}
	print WHA "\n";


	print WHA "tbl_bitrev:\n";
	$bp = 1; $n = $fftn;
	while ($n >>= 1) { $bp <<= 1; }
	for ($n = 0; $n < $fftn; $n++) {
		$s = 0; $f = $fftn; $r = 1;
		for ($c = 0; $c < $bp; $c++) {
			$f >>= 1;
			if ($n & $r) { $s |= $f; }
			$r <<= 1;
		}
		if (($n & 15) == 0) {
			print WHA "\t.dc.w\t$s*4";
		} else {
			print WHA ", $s*4";
		}
		if (($n & 15) == 15) { print WHA "\n"; }
	}
	print WHA "\n";

	close WHA;
