close all
clear
clc
%% Read data
file_name = 'row_data_lsm_slow_rot.txt';
[r_lsm] = read_row_data(file_name);
file_name = 'row_data_mpu_slow_rot.txt';
[r_mpu] = read_row_data(file_name);

%% Calculate Calibration values
[lsmLS,lsmEF,lsmSC] = calculate_mag_calibration(r_lsm);
[mpuLS,mpuEF,mpuSC] = calculate_mag_calibration(r_mpu);

%% Apply calibration
c_lsm.mag = apply_calibration ( r_lsm.mx, r_lsm.my, r_lsm.mz, lsmEF.offset, lsmEF.rotation);
c_mpu.mag = apply_calibration ( r_mpu.mx, r_mpu.my, r_mpu.mz, mpuEF.offset, mpuEF.rotation);

%%
figure('name','mag')
subplot(221)
title('x vs y ')
hold on
plot(c_lsm.mag.x,c_lsm.mag.y,'rx')
plot(c_mpu.mag.x,c_mpu.mag.y,'gx')
plot([-100 100],[0 0],'k',[0 0],[-100 100],'k')
hold off
legend('lsm','mpu')
grid minor
axis square

subplot(222)
title('x vs z ')
hold on
plot(c_lsm.mag.x,c_lsm.mag.z,'rx')
plot(c_mpu.mag.x,c_mpu.mag.z,'gx')
plot([-100 100],[0 0],'k',[0 0],[-100 100],'k')
hold off
legend('lsm','mpu')
grid minor
axis square

subplot(223)
title('y vs z ')
hold on
plot(c_lsm.mag.y,c_lsm.mag.z,'rx')
plot(c_mpu.mag.y,c_mpu.mag.z,'gx')
plot([-100 100],[0 0],'k',[0 0],[-100 100],'k')
hold off
legend('lsm','mpu')
grid minor
axis square

subplot(224)
lsm_mag = sqrt(c_lsm.mag.x.^2 + c_lsm.mag.y.^2 + c_lsm.mag.z.^2);
mpu_mag = sqrt(c_mpu.mag.x.^2 + c_mpu.mag.y.^2 + c_mpu.mag.z.^2);

hold on
plot(r_lsm.time,lsm_mag,'r')
plot(r_mpu.time,mpu_mag,'g')
hold off
legend('lsm','mpu')
grid minor
