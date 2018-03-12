close all
clear
clc
%% Read data
% new_mag = rotation * (row_mag - offset)
file_name = 'row_data_lsm_fast_rot.txt';
[row_mag_data] = read_row_data(file_name);
[LS, EF, SC] = calculate_mag_calibration(row_mag_data);
mag_data = apply_calibration ( row_mag_data.mx, row_mag_data.my, row_mag_data.mz, EF.offset, EF.rotation);

%% Apply accelerometer calibration
% new_acc = rotation * (row_acc - offset)
file_name = 'acc_matrix_miss_lsm.txt';
[offset,rot] = acc_least_square_calibration(file_name);

%% write txt file
new_file = 'calibrate_txt/lsm_calib_fast.txt';
fileID = fopen(new_file, 'w');

fprintf(fileID,'acc_rot:\n');
fprintf(fileID,'            %+10.7f, %+10.7f, %+10.7f\n', rot(1,1), rot(1,2), rot(1,3));
fprintf(fileID,'            %+10.7f, %+10.7f, %+10.7f\n', rot(2,1), rot(2,2), rot(2,3));
fprintf(fileID,'            %+10.7f, %+10.7f, %+10.7f\n', rot(3,1), rot(3,2), rot(3,3));
fprintf(fileID,'acc_offset:\n');
fprintf(fileID,'            %+10.7f, %+10.7f, %+10.7f\n', offset(1), offset(2), offset(3));
fprintf(fileID,'mag_rot:\n');
fprintf(fileID,'            %+10.7f, %+10.7f, %+10.7f\n', EF.rotation(1,1), EF.rotation(1,2), EF.rotation(1,3));
fprintf(fileID,'            %+10.7f, %+10.7f, %+10.7f\n', EF.rotation(2,1), EF.rotation(2,2), EF.rotation(2,3));
fprintf(fileID,'            %+10.7f, %+10.7f, %+10.7f\n', EF.rotation(3,1), EF.rotation(3,2), EF.rotation(3,3));
fprintf(fileID,'mag_offset:\n');
fprintf(fileID,'            %+10.7f, %+10.7f, %+10.7f\n', EF.offset(1), EF.offset(2),EF.offset(3));

fclose(fileID);