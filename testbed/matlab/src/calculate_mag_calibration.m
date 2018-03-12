function [LS,EF,SC] = calculate_mag_calibration(data)
%% Calculate mag calibration using different methods

% least square 
[offset,rotation] = mag_least_square_calibration(data);
LS.offset = offset;
LS.rotation = rotation;

% Ellipsoid fit
[offset,rotation] = mag_ellipsoid_fit_calibration(data);
EF.offset = offset;
EF.rotation = rotation;

% simple method
[offset,rotation] = mag_simple_calibration(data);
SC.offset = offset;
SC.rotation = rotation;