%Author: Devan Tormey
%Title: Simulation Initialization 
%Description:

clear; clc; close all;

% ~~~~~~~~~~~~~~~~~~
% Add Paths
% ~~~~~~~~~~~~~~~~~~
addpath(genpath(pwd))
addpath(genpath('soci-gnc/Src/'))
addpath(genpath('soci-gnc/Lib/'))
addpath(genpath('soci-gnc/Include/'))
addpath(genpath('soci-gnc/Test/'))

init_params;



%% Solar panels
solar_panels = struct;

cellArea = 26.62; % cm^2
Jmpp = 17.4; % mA / cm^2
temperature = 28; % degrees C
currentLossEOL = 0.99;
temperatureCurrentCoef = 0.005 * cellArea; % mA / degrees C
Impp = cellArea * Jmpp * currentLossEOL + (temperatureCurrentCoef * (temperature-28));

VmppBase = 2.39; % V
voltageLossEOL = 0.99;
temperatureVoltageCoef = -0.0063; % V / degrees C
VmppCell = VmppBase * voltageLossEOL + ((temperature-28) * temperatureVoltageCoef); % V

cellsX = 4;
cellsY = 5;
cellsZ = 2;

VmppX = cellsX * VmppCell; % V, +x face
VmppY = cellsY * VmppCell; % V, -x, +y, -y faces
VmppZ = cellsZ * VmppCell; % V, +z face

solar_panels.maxPower = (Impp) .* [ VmppX; VmppY; VmppY; VmppY; VmppZ; 0.0 ];

solar_panels.normals  = [ 1 -1  0  0  0  0
                          0  0  1 -1  0  0
                          0  0  0  0  1 -1 ];

solar_panels.num      = sum(solar_panels.maxPower>0);

solar_panels.eff = 0.9;

simParams.sensors.solar_panels = solar_panels;
clear cellArea Jmpp temperature currentLossEOL temperatureCurrentCoef Impp 
clear VmppBase voltageLossEOL temperatureVoltageCoef VmppCell
clear cellsX cellsY cellsZ VmppX VmppY VmppZ 
clear solar_panels

%% test
% simParams.sample_time_s = .1;
% simParams.sensors.sample_time_s = 1;
% simParams.sensors.actuators = 1;
% simParams.sensors.magField = 1;


test_data = struct;

test_data.a = [0.6428 0.766 0.0];
test_data.eff_3V3 = 0.85;
test_data.eff_5V = 0.9;

test_data.BAT = 200;
test_data.EPS = 200;

test_data.COM_TX = 2900;
test_data.COM_RX = 450;

test_data.RXW_Pointing = 2000; %% not used
test_data.RXW_Idle = 660; %% not used

test_data.MTQ_Idle = 0;
test_data.MTQ_Active = 4*1080 + 595;

test_data.IMG_Idle = 25;
test_data.IMG_Active = 515;

test_data.CDH_Idle = 398;
test_data.CDH_Active = 398;

test_data.SEN_AVG = 3.3 * (23 + 3*0.11 + 3*2.7);