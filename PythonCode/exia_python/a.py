def calculate_ARR(T_clk, F_pwm, PSC):
    # 计算ARR
    ARR = (T_clk / (F_pwm * (PSC + 1))) - 1
    return int(ARR)


T_clk = 84e6  # 时钟频率，例如72MHz
F_pwm = 800e3  # 目标PWM频率，例如800kHz
PSC = 0  # 预分频值，默认为0

period = calculate_ARR(T_clk, F_pwm, PSC)

code_0 = 0.32 * (int(period) + 1)

code_1 = 0.64 * (int(period) + 1)

print("占空比为:", period, "0码:", code_0, "1码:", code_1, "\n")
