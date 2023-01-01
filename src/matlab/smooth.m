function [Y] = smooth(X)
  Y = zeros(size(X));
  for dd = 1:size(X,2)
    Y(:,dd) = smooth_windowed(X(:,dd),[0.25;0.5;0.75;0.8;1.0;0.8;0.75;0.5;0.25]);
  end
end

function [y] = smooth_windowed(x, weights)
  w_len = length(weights);
  d_len = length(x);
  w_center_idx = floor(w_len / 2) + 1;
  weight_sum = sum(weights);
  y = zeros(size(x));
  charging = true;
  windowed_samps = zeros(size(weights));
  for ii = 1:d_len
    windowed_samps(end+1) = x(ii);
    if length(windowed_samps) > w_len
      windowed_samps(1) = [];
    end
    weighted_samps = windowed_samps .* weights;
    if ii >= w_center_idx
      y(ii - w_center_idx + 1) = sum(weighted_samps) / weight_sum;
    end
  end
end
