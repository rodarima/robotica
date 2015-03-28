se = [.2 .3 .4 .5 .6 .8 1 1.2];

%Con pulseIn (ERRONEO)
t1 = [805 1225 1638 2060 2500 3305 4155 5000] * 1e-6;

t2 = [1122 1705 2285 2848 3422 4597 5810 6970] *1e-6

p1 = polyfit(t1, se, 1)
p2 = polyfit(t2, se, 1)

x = 0:1e-4:max(t2)*1.1;

% Velocidad del sonido
temp = 20
vs = 331.3 + 0.606 * temp

hold on;
%axis([0 max6e-3 0 2]);
plot(t1, se, 'or');
plot(t2, se, 'ob');
plot(x, p1(1)*x + p1(2), 'r');
plot(x, p2(1)*x + p2(2), 'b');
plot(x, x/2*vs, 'k:');
xlabel ("t (s)");
ylabel("s (m)");
print -dpdf "-S400,300" sonar.pdf;
