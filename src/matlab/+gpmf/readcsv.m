function [telem] = readcsv(csv_file_path)
%READ reads in gpmf telemetry data from CSV file
%   Detailed explanation goes here
table = csvread(csv_file_path,1,0);% ignore first row because it contains headings

telem = struct();
telem.accl = struct();
telem.gyro = struct();
telem.grav = struct();
telem.cori = struct();
telem.gps = struct();

isCombined = contains(csv_file_path,'_combined');
if isCombined
    n_samps = size(table,1);
    rate_hz = (n_samps - 1) / (table(end,1) - table(1,1));

    telem.accl.rate_hz = rate_hz;
    telem.accl.samples = table(:,2:4);

    telem.gyro.rate_hz = rate_hz;
    telem.gyro.samples = table(:,5:7);

    telem.grav.rate_hz = rate_hz;
    telem.grav.samples = table(:,8:10);

    telem.cori.rate_hz = rate_hz;
    telem.cori.samples = table(:,11:14);
    telem.cori.samples_q = quaternion(table(:,11:14));

    telem.gps.rate_hz = rate_hz;
    telem.gps.latlon = table(:,15:16);
    telem.gps.altitude = table(:,17);
    telem.gps.speed2D = table(:,18);
    telem.gps.speed3D = table(:,19);
else
    n_samps = table(1,:);
    rates_hz = table(2,:);

    telem.accl.rate_hz = rates_hz(1);
    telem.accl.samples = table(3:3+n_samps(1)-1,1:3);

    telem.gyro.rate_hz = rates_hz(4);
    telem.gyro.samples = table(3:3+n_samps(4)-1,4:6);

    telem.grav.rate_hz = rates_hz(7);
    telem.grav.samples = table(3:3+n_samps(7)-1,7:9);

    telem.cori.rate_hz = rates_hz(10);
    telem.cori.samples = table(3:3+n_samps(10)-1,10:13);
    telem.cori.samples_q = quaternion(telem.cori.samples);

    telem.gps.rate_hz = rates_hz(14);
    telem.gps.latlon = table(3:3+n_samps(14)-1,14:15);
    telem.gps.altitude = table(3:3+n_samps(14)-1,16);
    telem.gps.speed2D = table(3:3+n_samps(14)-1,17);
    telem.gps.speed3D = table(3:3+n_samps(14)-1,18);
end

end

