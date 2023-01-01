function [] = pose_fun(csv_file_path)
telem = gpmf.readcsv(csv_file_path);

%% plot accelerometer data
qi = telem.cori_raw(1);
qf = telem.cori_raw(end);
patch = poseplot(qi);
for coeff = 0:0.01:1
    q = slerp(qi,qf,coeff);
    set(patch,Orientation=q); 
    drawnow
end
end
