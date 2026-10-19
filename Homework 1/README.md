# Circuit 1 (c1.txt)

U_d_led = 2V
U_rezistoare = 4.2V - 2V = 2.2V
R_circ = 320 ohm

I = 2.2V/320ohm = 0.006875 A

U_1 = I * R_1 = 1.5125 V
U_2 = I * R_2 = 0.6875 V

Astea 2 adunate + 2V de la led dau kirchoff 4.2 V bateria noastra


P_1 = U_1 * I = 0.006875 W
P_2 = U_2 * I = 0.0047265625 W


# Circuit 2 (c2.txt)

U_d_led = 2V

U_s = 5V

U_rezistoare = 5V - 2V = 3V

Ramura 1:
I_1 = 3V / 220 ohm = 0.013636 A

Ramura 2:
I_2 = 3V / 330 ohm = 0.009091 A

I_total = I_1 + I_2 = 0.013636 A + 0.009091 A = 0.022727 A

KCL:
I_total = I_1 + I_2 = 0.022727 A

U_R1 = I_1 * R_1 = 0.013636 A * 220 ohm = 3V
U_R2 = I_2 * R_2 = 0.009091 A * 330 ohm = 3V

KVL Ramura 1:
U_s = U_R1 + U_d_led = 3V + 2V = 5V

KVL Ramura 2:
U_s = U_R2 + U_d_led = 3V + 2V = 5V

P_1 = U_R1 * I_1 = 3V * 0.013636 A = 0.040908 W

P_2 = U_R2 * I_2 = 3V * 0.009091 A = 0.027273 W


# Circuit 3 (c3.txt)

Nu-mi iese
