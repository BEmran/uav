function [offset,rotation] = acc_least_square_calibration(file_name)
%%
% Calculate rotaion missalignment and offset gains from
% acc-calibration data
% new_acc = rotation * (row_acc - offset).

%% read file
d = readtable(file_name);
data = table2struct(d,'ToScalar',true);

%% Split data into Matrices
% A matrix contatins the measured Acceleration data + column of 1
A = [data.Var6 data.Var7 data.Var8 ones(length(data.Var8),1)];
% B matrix contatins the correct Acceleration data
B = [data.Var2 data.Var3 data.Var4];
% Find parameter using least square error
X = ((A'*A)^-1*A'*B)';
% A   = X(:,1:3) * acc + X(:,4)  <- this how the method is defined

%% Calculate the rotation and offset
% A   =    R     * (A - off)     <- this how we are going to use in RPi
% R   = X(:,1:3)
% off = - R \ X(:,4)

rotation =   X(:,1:3);
offset   = - X(:,1:3) \ X(:,4);
