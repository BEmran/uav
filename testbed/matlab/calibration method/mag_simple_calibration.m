function [offset,gain,rotation] = mag_simple_calibration(data)

% find minmum maximum values
mx_max = max(data.mx);
mx_min = min(data.mx);
my_max = max(data.my);
my_min = min(data.my);
mz_max = max(data.mz);
mz_min = min(data.mz);

% Calculate offsets 
offset(1) = (mx_max + mx_min)/2;
offset(2) = (my_max + my_min)/2;
offset(3) = (mz_max + mz_min)/2;

% Calculate sensitivity 
gain(1) = (mx_max - mx_min)/2;
gain(2) = (my_max - my_min)/2;
gain(3) = (mz_max - mz_min)/2;

% Rescale
ave = sum(gain)/3;
gain(1) = ave/gain(1);
gain(2) = ave/gain(2);
gain(3) = ave/gain(3);

% Default rotation matrix
rotation = eye(3,3);
end