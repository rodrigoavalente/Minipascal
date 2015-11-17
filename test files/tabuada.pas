program tabuada;

var i, x : integer;
var y : real;

begin
	x := 2;
	y := 2.5;
	i := 0;
	while i <= 10 do
		begin
            writeln(i + " x " + x + " = " + (i * x));
            i := i + 1;
		end;
end.
