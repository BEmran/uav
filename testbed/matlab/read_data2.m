close all
clear
%clc

file_name = 'row_data_lsm1.txt';
d = readtable(file_name);
d = d(1:end-1,:);
data = table2struct(d,'ToScalar',true);

%% Correct data
time0 = data.time(1);
data.time = data.time - time0;
data.time = data.time * 1e-6;
dt = mean(data.time(2:1000)-data.time(1:1000-1))
%data.time = 0:1/390:-1+(length(data.ax)/390);
%%
data.mx(abs(data.mx) > 200) = 0.0;
data.my(abs(data.my) > 200) = 0.0;
data.mz(abs(data.mz) > 200) = 0.0;
filds = fieldnames(data);
% for i = 1:length(filds)    
%     data.mx(isnan(data.(filds{i}))) = 0.0;
% end
%% plot

subplot(311)
plot(data.time,data.gx,data.time,data.gy,data.time,data.gz)
legend('x-axis','y-axis','z-axis')
title('Gyro')
grid on

subplot(312)
plot(data.time,data.ax,data.time,data.ay,data.time,data.az)
legend('x-axis','y-axis','z-axis')
title('Accelerometer')
grid on

subplot(313)
plot(data.time,data.mx,data.time,data.my,data.time,data.mz)
legend('x-axis','y-axis','z-axis')
title('Magnatometer')
grid on

%%
X = data.mx;
Y = data.my;
Z = data.mz;
X(isnan(X)) = 0.0;
Y(isnan(Y)) = 0.0;
Z(isnan(Z)) = 0.0;
D = [X Y Z];
%[ofs,gain,rotM] = ellipsoid_fit(D,2);
[ofs,gain,rotM] = ellipsoid_fit2(D)

mx = X-ofs(1);
my = Y-ofs(2);
mz = Z-ofs(3); % translate to (0,0,0)

XYZC = [mx,my,mz]*rotM; % rotate to XYZ axes

refr = (sum(gain)/3); 
%gain = ones(3,1);
% reference radius
mx = XYZC(:,1)/gain(1)*refr; 
my = XYZC(:,2)/gain(2)*refr; 
mz = XYZC(:,3)/gain(3)*refr; 

% scale to sphere
% figure; 
% subplot(2,2,1);
% plot(mx,my,'ro',X,Y,'kx'); 
% xlabel('X'); ylabel('Y'); axis equal; grid on;
% subplot(2,2,2);
% plot(mz,my,'go',Z,Y,'kx'); 
% xlabel('Z'); ylabel('Y'); axis equal; grid on;
% subplot(2,2,3);
% plot(mx,mz,'bo',X,Z,'kx'); 
% xlabel('X'); ylabel('Z'); axis equal; grid on;
% subplot(2,2,4);
% m1 = sqrt(X.^2 + Y.^2 + Z.^2);
% m2 = sqrt(mx.^2 + my.^2 + mz.^2);
% plot(data.time,m1,'r',data.time,m2,'g')
% legend('data','corrected')
% grid minor
% axis square
%%
% simple calibration method 1
mx_max = max(data.mx);
mx_min = min(data.mx);
my_max = max(data.my);
my_min = min(data.my);
mz_max = max(data.mz);
mz_min = min(data.mz);
m_zero(1) = (mx_max + mx_min)/2;
m_zero(2) = (my_max + my_min)/2;
m_zero(3) = (mz_max + mz_min)/2;
m_sen(1) = (mx_max - mx_min)/2;
m_sen(2) = (my_max - my_min)/2;
m_sen(3) = (mz_max - mz_min)/2;
ave = sum(m_sen)/3;
m_sen(1) = ave/m_sen(1);
m_sen(2) = ave/m_sen(2);
m_sen(3) = ave/m_sen(3);
mx1 = (data.mx - m_zero(1)) / m_sen(1);
my1 = (data.my - m_zero(2)) / m_sen(2);
mz1 = (data.mz - m_zero(3)) / m_sen(3);
% simple calibration method 2
H = [data.mx, data.my, data.mz, - data.my.^2, - data.mz.^2, ones(size(data.mx))];
w = data.mx.^2;
X = (H'*H)\H'*w;
off(1) = X(1)/2;
off(2) = X(2)/(2*X(4));
off(3) = X(3)/(2*X(5));
temp = X(6) + off(1)^2 + X(4)*off(2)^2 + X(5)*off(3)^2;
scale(1) = sqrt(temp);
scale(2) = sqrt(temp / X(4));
scale(3) = sqrt(temp / X(5));
ave = sum(scale)/3;
scale(1) = ave/scale(1);
scale(2) = ave/scale(2);
scale(3) = ave/scale(3);
mx2 = (data.mx - off(1)) / scale(1);
my2 = (data.my - off(2)) / scale(2);
mz2 = (data.mz - off(3)) / scale(3);

figure
subplot(241)
title('x vs y ')
plot(data.mx,data.my,'kx',mx,my,'bx',mx1,my1,'rx',mx2,my2,'gx',[-100 100],[0 0],'k',[0 0],[-100 100],'k')
legend('data','corrected0','corrected2','corrected2')
grid minor
axis square

subplot(244)
title('x vs z ')
plot(data.mx,data.mz,'kx',mx,mz,'bx',mx1,mz1,'rx',mx2,mz2,'gx',[-100 100],[0 0],'k',[0 0],[-100 100],'k')
legend('data','corrected0','corrected2','corrected2')
grid minor
axis square

subplot(245)
title('y vs z ')
plot(data.my,data.mz,'kx',my,mz,'bx',my1,mz1,'rx',my2,mz2,'gx',[-100 100],[0 0],'k',[0 0],[-100 100],'k')
legend('data','corrected0','corrected2','corrected2')
grid minor
axis square
subplot(248)
mag = sqrt(data.mx.^2 + data.my.^2 + data.mz.^2);
m = sqrt(mx.^2 + my.^2 + mz.^2);
m1 = sqrt(mx1.^2 + my1.^2 + mz1.^2);
m2 = sqrt(mx2.^2 + my2.^2 + mz2.^2);
plot(data.time,mag,'k',data.time,m,'b',data.time,m1,'r',data.time,m2,'g')
legend('data','corrected0','corrected2','corrected2')
grid minor
axis square

subplot(2,4,[2 3 6 7])
plot3(data.mx,data.my,data.mz,'kx',mx,my,mz,'bx',mx1,my1,mz1,'rx',mx2,my2,mz2,'gx')
legend('data','corrected0','corrected2','corrected2')
grid minor
axis square