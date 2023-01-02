function [] = pose_fun(csv_file_path)
telem = gpmf.readcsv(csv_file_path);

%% plot accelerometer data
patch = poseplot(telem.cori.samples_q(1));
cori_nsamps = size(telem.cori.samples_q,1);
for ii = 2:20:cori_nsamps
    q = telem.cori.samples_q(ii);
    set(patch,Orientation=q); 
    drawnow
end
end
