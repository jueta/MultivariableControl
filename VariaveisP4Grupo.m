s = tf('s');

PlantaAquecimento = tf(0.256, [37 1]);
ControladorAquecimento = tf([9.25 3.95 0.1],[0.26 1 0]);

PlantaUmidade = tf(0.22, [29 1]);
ControladorUmidade = tf([5.8 3.1 0.1],[0.22 1 0]);

PlantaResfriamento = tf(-0.27,[63 1]);

GanhoFeedForward = PlantaResfriamento / PlantaAquecimento;
