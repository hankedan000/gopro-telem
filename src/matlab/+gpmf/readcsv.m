function [telem] = readcsv(csv_file_path)
%READ reads in gpmf telemetry data from CSV file
%   Detailed explanation goes here
table = csvread(csv_file_path,1,0);% ignore first row because it contains headings

telem = struct();
telem.n_samps = size(table,1);
telem.dt = table(2,1) - table(1,1);
telem.accl_raw = table(:,2:4);
telem.gyro_raw = table(:,5:7);
telem.grav_raw = table(:,8:10);
telem.cori_raw = quaternion(table(:,11:14));
telem.gps_ll_raw = table(:,15:16);

end

