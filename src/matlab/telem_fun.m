function [] = telem_fun(csv_file_path)
telem = gpmf.readcsv(csv_file_path);

%% plot accelerometer data
figure();
plot(telem.accl.samples);
title('Accelerometer');
xlabel('sample');
ylabel('acceleration (m/s^2)');
legend({'x','y','z'},'Location','northwest');

%% plot gyroscope data
figure();
plot(telem.gyro.samples);
title('Gyroscope');
xlabel('sample');
ylabel('gyro (rad/s)');
legend({'x','y','z'},'Location','northwest');

%% plot gravity data
figure();
plot(telem.grav.samples);
title('Gravity');
xlabel('sample');
ylabel('gravity (G)');
legend({'x','y','z'},'Location','northwest');

%% plot derivatives of acceleration and gravity against each other
accl_d1 = diff(telem.accl.samples)/(1.0/telem.accl.rate_hz);
grav_d1 = diff(telem.grav.samples * 9.81)/(1.0/telem.grav.rate_hz);
figure();
subplot(2,1,1);
plot(accl_d1(:,1));
title('Derivative of Acceleration (x component)');
subplot(2,1,2);
plot(grav_d1(:,1));
title('Derivative of Gravity (x component)');

%% plot camera orientation data
figure();
plot(telem.cori.samples);
title('Camera Orientation');
xlabel('sample');
ylabel('unit');
legend({'w','x','y','z'},'Location','northwest');

%% plot GPS lat/lon data
figure();
subplot(2,1,1);
plot(telem.gps.latlon(:,1));
title('GPS Latitude');
xlabel('sample');
ylabel('latitude (dec deg)');
subplot(2,1,2);
plot(telem.gps.latlon(:,2));
title('GPS Longitude');
xlabel('sample');
ylabel('longitude (dec deg)');

%% compute and plot smoothed acceleration
accl_smooth10 = smoothMovingAvg(telem.accl.samples, 10);
accl_smooth100 = smoothMovingAvg(telem.accl.samples, 100);
accl_smooth200 = smoothMovingAvg(telem.accl.samples, 200);
figure();
componentNames = ['X','Y','Z'];
n_comps = length(componentNames);
for cc=1:n_comps
    subplot(n_comps,1,cc);
    hold on;
    plot(telem.accl.samples(:,cc));
    plot(accl_smooth10(:,cc));
    plot(accl_smooth100(:,cc));
    plot(accl_smooth200(:,cc));
    hold off;
    title(sprintf('Smoothed Acceleration %c', componentNames(cc)));
    xlabel('sample');
    ylabel('acceleration (m/s^2)');
    legend({'accl_raw','accl_smooth10','accl_smooth100','accl_smooth200'},'Location','northwest');
end

%% compute and plot rotation
rot_raw = zeros(size(telem.gyro.samples));
inst_rot = [0,0,0];
gyro_dt = 1.0/telem.gyro.rate_hz;
gyro_nsamps = size(telem.gyro.samples,1);
for ii = 1:gyro_nsamps
  inst_rot = inst_rot + telem.gyro.samples(ii,:) * gyro_dt;
  rot_raw(ii,:) = inst_rot;
end

figure();
plot(rot_raw);
title('Rotation (calculated)');
xlabel('sample');
ylabel('rotation (rad)');
legend({'x','y','z'},'Location','northwest');

%% compute and plot velocity
accl_rate = telem.accl.rate_hz;
grav_rate = telem.grav.rate_hz;
if abs(accl_rate - grav_rate) > 1.0
    warning("'accl' and 'grav' rates are different. can't compute velocity.");
else
    vel_raw = zeros(size(telem.accl.samples));
    inst_vel = [0,0,0];
    dt = 1.0 / accl_rate;% grav has sample rate
    nsamps = size(telem.accl.samples,1);
    for ii = 1:nsamps
      inst_vel = inst_vel + (telem.accl.samples(ii,:) - telem.grav.samples(ii,:) * 9.81) * dt;
      vel_raw(ii,:) = inst_vel;
    end
    
    figure();
    plot(vel_raw);
    title('Velocity (calculated)');
    xlabel('sample');
    ylabel('velocity (m/s)');
    legend({'x','y','z'},'Location','northwest');
end

end
