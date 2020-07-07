close all; %clc;

format short g

sum = 0;
for i = 2:length(p_in)
    sum = sum + p_in(i);
end

orbital_average_power = sum / (length(p_in) - 1)

sum = 0;
for i = 2:length(p_out)
    sum = sum + p_out(i);
end

orbital_average_load = sum / (length(p_in) - 1)

power_budget = orbital_average_power - orbital_average_load
percent = 100 * (power_budget / orbital_average_load)