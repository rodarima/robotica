%data = csvread('test2.txt');
x = data(:,1)';
t = data(:,2)';
v = diff(x);
plot(t(1:end-1), v, 'o-')
