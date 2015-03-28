N=2
G=2

for i = 3:10

	name = strcat('velocidad-voltaje-', num2str(i))
	csv = strcat(name, '.csv');
	csvs = strcat(name, '-s.csv');

	data = dlmread(csv, '\t', 1, 0);


	v = [zeros(1,N) , data(:, 1)' , zeros(1,N)]*40;
	t =  data(:, N)' / 1000000;
	maxt = max(t);
	t = [zeros(1,N) , t , repmat(maxt,1,N)];

	pkg load image;

	vs = imsmooth(v, 'Gaussian', G);

	dlmwrite(csvs, [vs; t]', '\t');

endfor

system('sed -i "1iv\tt" *-s.csv')
