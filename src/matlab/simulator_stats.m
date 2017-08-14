%% Spatial Uniformity

% Matrix of data corresponding to cells on test bed
%
% - 400mm separation of light bed from base of frame
% - Lights angles 5 degrees outward
data = [850 860 955;...
        950 930 1000;...
        870 834 900];
    
heatmap(data, [], [], '%0.0f', 'ColorMap', 'cool');
title('Irradiance Map (W/m^2)');

E_min = min(data(:));
E_max = max(data(:));
S_NE = 100*(E_max-E_min)/(E_max+E_min);

fprintf('%s\n', '')
disp('Spatial Uniformity Rating:');
if (S_NE <= 2.0) 
    disp('Class A')
elseif (S_NE <= 5.0)
    disp('Class B')
elseif (S_NE <= 10.0)
    disp('Class C')
end
fprintf('%s\n', '')