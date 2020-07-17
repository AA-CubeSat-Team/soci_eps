close all; %clc;
format short g

%% run simulation
tsim = 10800;
test_data.com_en = 0;
test_data.mtq_en = 0;
test_data.img_en = 0;
test_data.fixed_attitude = 0;
test_data.mode = 1;

for i = 1 : 7
    if (i == 1)
        test_data.fixed_attitude = 1
    end
    if (i == 2)
        test_data.fixed_attitude = 0
    end
    if (i == 3)
        test_data.mode = 0
    end
    if (i == 4)
        test_data.mode = -1
    end
    if (i == 5)
        test_data.mode = 1
        test_data.com_en = 1
    end
    if (i == 6)
        test_data.img_en = 1
    end
    if (i == 7)
        test_data.com_en = 0
        test_data.img_en = 0
        test_data.mtq_en = 1
    end
    
    runsim(tsim)
end


%% function run sim and print results
function results = runsim(tsim)
    
    sim('Main_Sim', tsim)
    
    %% find indexes for a single orbit
    %  Typically, simulation starts during an eclipse period
    %  this code trys to find the indexes of the first full orbit after that
    %  first eclipse period
    index_start = -1; % start of orbit to be analyzed
    index_eclipse = -1;
    index_end = -1; % end of orbit to be analyzed
    for i = 1 : length(p_in)
        power = p_in(i);
        if (power > 0 && index_start == -1)
            index_start = i;
        end
        if (index_start > -1)
            if (power == 0  && index_eclipse == -1)
                index_eclipse = i;
            end
        end
        if (index_eclipse > -1)
            if (power > 0 && index_end == -1)
                index_end = i-1;
            end
        end
    end

    %% find power budget
    duration = index_end - index_start;

    sum = 0;
    for i = index_start:index_end
        sum = sum + p_in(i);
    end

    orbital_average_power = sum / duration;

    sum = 0;
    for i = index_start:index_end
        sum = sum + p_out(i);
    end

    orbital_average_load = sum / duration;

    power_budget = orbital_average_power - orbital_average_load;
    power_margin = 100 * (power_budget / orbital_average_load);

    %% results
    sprintf('Analyzed data from t = %i to t = %i', tout(index_start), tout(index_end))
    sprintf('orbital_average_power = %i', orbital_average_power)
    sprintf('orbital_average_load = %i', orbital_average_load)
    sprintf('power_budget = %i', power_budget)
    sprintf('power_margin = %f', power_margin)
end