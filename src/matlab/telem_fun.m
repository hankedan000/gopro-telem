function [] = telem_fun(csv_file_path)
telem = gpmf.readcsv(csv_file_path);

%% plot accelerometer data
figure();
plot(telem.accl_raw);
title('Accelerometer (raw)');
xlabel('sample');
ylabel('acceleration (m/s^2)');
legend({'x','y','z'},'Location','northwest');

%% plot gyroscope data
figure();
plot(telem.gyro_raw);
title('Gyroscope (raw)');
xlabel('sample');
ylabel('gyro (rad/s)');
legend({'x','y','z'},'Location','northwest');

%% plot gravity data
figure();
plot(telem.grav_raw);
title('Gravity (raw)');
xlabel('sample');
ylabel('gravity (G)');
legend({'x','y','z'},'Location','northwest');

%% plot camera orientation data
figure();
plot(telem.cori_raw);
title('Camera Orientation (raw)');
xlabel('sample');
ylabel('unit');
legend({'x','y','z','w'},'Location','northwest');

%% compute and plot smoothed acceleration
accl_smooth = smooth(telem.accl_raw);
figure();
plot(accl_smooth);
title('Accelerometer (smooth)');
xlabel('sample');
ylabel('acceleration (m/s^2)');
legend({'x','y','z'},'Location','northwest');

%% compute rotation
rot_raw = zeros(size(telem.gyro_raw));
inst_rot = [0,0,0];
for ii = 1:telem.n_samps
  inst_rot = inst_rot + telem.gyro_raw(ii,:) * telem.dt;
  rot_raw(ii,:) = inst_rot;
end

%% plot rotation data
figure();
plot(rot_raw);
title('Rotation (raw)');
xlabel('sample');
ylabel('rotation (rad)');
legend({'x','y','z'},'Location','northwest');

%% compute velocity
vel_raw = zeros(size(telem.accl_raw));
inst_vel = [0,0,0];
for ii = 1:telem.n_samps
  inst_vel = inst_vel + (telem.accl_raw(ii,:) - telem.grav_raw(ii,:) * 9.81) * telem.dt;
  vel_raw(ii,:) = inst_vel;
end

%% plot velocity data
figure();
plot(vel_raw);
title('Velocity (raw)');
xlabel('sample');
ylabel('velocity (m/s)');
legend({'x','y','z'},'Location','northwest');

end
