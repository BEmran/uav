function [data,LS,EF,SC] = read_row_data(file_name)

d = readtable(file_name);
d = d(1:end-1,:);
data = table2struct(d,'ToScalar',true);

%% Correct data
%time0 = data.time(1);
%data.time = data.time - time0;
data.time = data.time * 1e-6;
%% remove high values
data.mx(abs(data.mx) > 200) = 0.0;
data.my(abs(data.my) > 200) = 0.0;
data.mz(abs(data.mz) > 200) = 0.0;
%% applay mag calibration using different methods
[offset,gain,rotation] = mag_least_square_calibration(data);
LS.offset = offset;
LS.gain = gain;
LS.rotation = rotation;
[offset,gain,rotation] = mag_ellipsoid_fit_calibration(data);
EF.offset = offset;
EF.gain = gain;
EF.rotation = rotation;
[offset,gain,rotation] = mag_simple_calibration(data);
SC.offset = offset;
SC.gain = gain;
SC.rotation = rotation;
end