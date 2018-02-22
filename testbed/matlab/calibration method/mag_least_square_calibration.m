function [offset,gain,rotation] = mag_least_square_calibration(data)

% simple calibration method 2
H = [data.mx, data.my, data.mz, - data.my.^2, - data.mz.^2, ones(size(data.mx))];
w = data.mx.^2;
X = (H'*H)\H'*w;

% Calculate offsets 
offset(1) = X(1)/2;
offset(2) = X(2)/(2*X(4));
offset(3) = X(3)/(2*X(5));

% Calculate sensitivity 
temp = X(6) + offset(1)^2 + X(4)*offset(2)^2 + X(5)*offset(3)^2;
gain(1) = sqrt(temp);
gain(2) = sqrt(temp / X(4));
gain(3) = sqrt(temp / X(5));

% Rescale
ave = sum(gain)/3;
gain(1) = ave/gain(1);
gain(2) = ave/gain(2);
gain(3) = ave/gain(3);

% Default rotation matrix
rotation = eye(3,3);
end